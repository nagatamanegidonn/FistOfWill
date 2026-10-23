struct VS_INPUT
{
    float3 Position : POSITION0;
    float4 SubPosition : POSITION1;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL0;
    float4 DiffuseColor : COLOR0;
    float4 SpecularColor : COLOR1;
    float2 TextureCoord0 : TEXCOORD0;
    float2 TextureCoord1 : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 svPos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
};

struct BASE_MATRIX
{
    matrix localWorldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ConstantBuffer : register(b7)
{
    BASE_MATRIX g_base;
};

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT ret;

    // --- 座標変換 ---
    // Position(float3) を float4(pos, 1.0) に拡張して計算
    float4 worldPos = mul(g_base.localWorldMatrix, float4(VSInput.Position, 1.0f));
    float4 viewPos = mul(g_base.viewMatrix, worldPos);
    ret.svPos = mul(g_base.projectionMatrix, viewPos);

    // --- データの受け渡し (変数名を構造体定義と一致させる) ---
    ret.uv = VSInput.TextureCoord0;
    ret.diffuse = VSInput.DiffuseColor;

    return ret;
}