// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 個人用バッファ
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX : register(b4)
{
    float3 g_light_dir;
    float g_none;
    float3 g_camera_pos;
    float g_specular_pow;
    float4 g_ambient_color;
};

float4 main(PS_INPUT PSInput) : SV_TARGET0
{
    // テクスチャのアルファ値を取得
    float4 diffuse = diffuseMapTexture.Sample(
        diffuseMapSampler,
        PSInput.uv
    );

    // 透明部分を描画しない
    if (diffuse.a < 0.01f)
    {
        discard;
    }

    // カメラ方向
    float3 camera_dir = normalize(
        g_camera_pos - PSInput.worldPos
    );

    // 視線方向と法線の内積
    float cDot = dot(
        normalize(PSInput.normal),
        camera_dir
    );

    // リムライト計算
    float rimDot = 1.0f - saturate(abs(cDot));

    // 強弱
    rimDot = pow(rimDot, g_specular_pow);

    // リムライトカラー
    float3 rimColor = g_ambient_color.rgb;

    // リムライトのみ出力
    return float4(rimColor * rimDot, rimDot);
}