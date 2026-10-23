// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME

// OUT
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// 【追加】定数バッファ：スロット7番目 (ModelRenderer.h の CONSTANT_BUF_SLOT_BEGIN_VS)
cbuffer cbCustomVS : register(b7)
{
    float4x4 g_LightVPMatrix; // ライト用 ビュー×プロジェクション行列 (FLOAT4 x 4行分)
}

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT ret;
    // 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 開始 )
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    
    // float3 → float4
    lLocalPosition.xyz = VSInput.pos;
    lLocalPosition.w = 1.0f;
    
    // ローカル座標をワールド座標に変換(剛体)
    lWorldPosition.w = 1.0f;
    lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
    ret.worldPos = lWorldPosition.xyz;

    // ワールド座標をビュー座標に変換
    lViewPosition.w = 1.0f;
    lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
    ret.vwPos.xyz = lViewPosition.xyz;
    
    // ビュー座標を射影座標に変換
    ret.svPos = mul(lViewPosition, g_base.projectionMatrix);
    // 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 終了 )

    // その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 開始 )
    ret.uv.x = VSInput.uv0.x;
    ret.uv.y = VSInput.uv0.y;

    // 法線をローカル空間からワールド空間へ変換
    ret.normal = normalize(
        mul(VSInput.norm, (float3x3) g_base.localWorldMatrix));

    ret.diffuse = VSInput.diffuse;
    ret.lightDir = float3(0.0f, 0.0f, 0.0f);

    // 【修正】ライトから見た座標を計算してピクセルシェーダへ渡す
    float4 lightSpacePos = mul(float4(ret.worldPos, 1.0f), g_LightVPMatrix);
    
    // 正投影なら W=1 ですが、透視投影に変更した時のためW除算をしておくのが安全です
    ret.lightAtPos = lightSpacePos.xyz / lightSpacePos.w;
    
    // その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 終了 )

    return ret;
}