// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN (アニメーション対応のため 8FRAME に変更)
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_8FRAME

// OUT
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// ボーン行列アクセスマクロ
#define L_W_MAT g_localWorldMatrix.lwMatrix

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT ret;

    // --- 【スキンメッシュ計算】 8ボーン分を統合して行列を作成 ---
    float4 lL_W_Mat[3];

    // BONE 1～4枚目
    lL_W_Mat[0] = L_W_MAT[VSInput.blendIndices0.x + 0] * VSInput.blendWeight0.x;
    lL_W_Mat[1] = L_W_MAT[VSInput.blendIndices0.x + 1] * VSInput.blendWeight0.x;
    lL_W_Mat[2] = L_W_MAT[VSInput.blendIndices0.x + 2] * VSInput.blendWeight0.x;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices0.y + 0] * VSInput.blendWeight0.y;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices0.y + 1] * VSInput.blendWeight0.y;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices0.y + 2] * VSInput.blendWeight0.y;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices0.z + 0] * VSInput.blendWeight0.z;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices0.z + 1] * VSInput.blendWeight0.z;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices0.z + 2] * VSInput.blendWeight0.z;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices0.w + 0] * VSInput.blendWeight0.w;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices0.w + 1] * VSInput.blendWeight0.w;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices0.w + 2] * VSInput.blendWeight0.w;

    // BONE 5～8枚目 (ここが抜けると頂点が原点に飛びます)
    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices1.x + 0] * VSInput.blendWeight1.x;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices1.x + 1] * VSInput.blendWeight1.x;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices1.x + 2] * VSInput.blendWeight1.x;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices1.y + 0] * VSInput.blendWeight1.y;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices1.y + 1] * VSInput.blendWeight1.y;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices1.y + 2] * VSInput.blendWeight1.y;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices1.z + 0] * VSInput.blendWeight1.z;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices1.z + 1] * VSInput.blendWeight1.z;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices1.z + 2] * VSInput.blendWeight1.z;

    lL_W_Mat[0] += L_W_MAT[VSInput.blendIndices1.w + 0] * VSInput.blendWeight1.w;
    lL_W_Mat[1] += L_W_MAT[VSInput.blendIndices1.w + 1] * VSInput.blendWeight1.w;
    lL_W_Mat[2] += L_W_MAT[VSInput.blendIndices1.w + 2] * VSInput.blendWeight1.w;

    // --- 頂点座標変換 (ベースコードの形式に準拠) ---
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;

    lLocalPosition.xyz = VSInput.pos;
    lLocalPosition.w = 1.0f;

    // 【追加】ローカルな高さを保存 (0.0 ～ 身長 の値が入る)
    // これを渡すことで、モデルの向きに関係なく「足元からの距離」が取れます
    ret.lightAtPos.x = VSInput.pos.y;
    
    // スキン行列を使ってワールド座標へ変換
    lWorldPosition.w = 1.0f;
    lWorldPosition.x = dot(lLocalPosition, lL_W_Mat[0]);
    lWorldPosition.y = dot(lLocalPosition, lL_W_Mat[1]);
    lWorldPosition.z = dot(lLocalPosition, lL_W_Mat[2]);
    ret.worldPos = lWorldPosition.xyz;

    // ワールド座標をビュー座標に変換
    lViewPosition.w = 1.0f;
    lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
    ret.vwPos.xyz = lViewPosition.xyz;

    // ビュー座標を射影座標に変換
    ret.svPos = mul(lViewPosition, g_base.projectionMatrix);

    // --- その他、ピクセルシェーダへ引継ぎ ---
    ret.uv = VSInput.uv0;

    // 法線もスキン行列でワールド空間へ変換
    float3 worldNormal;
    worldNormal.x = dot(VSInput.norm, lL_W_Mat[0].xyz);
    worldNormal.y = dot(VSInput.norm, lL_W_Mat[1].xyz);
    worldNormal.z = dot(VSInput.norm, lL_W_Mat[2].xyz);
    ret.normal = normalize(worldNormal);

    ret.diffuse = VSInput.diffuse;
    ret.lightDir = float3(0.0f, 0.0f, 0.0f);
    // ret.lightAtPos = float3(0.0f, 0.0f, 0.0f); // この行を削除、またはコメントアウト
    
    return ret;
}