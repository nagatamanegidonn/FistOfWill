// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer cbParam : register(b4)
{
    float4 g_color; // index 0
    float4 g_light_param; // index 1
    float4 g_ambient; // index 2
    float4 g_ghost_param; // index 3: x = alpha
}

float4 main(PS_INPUT PSInput) : SV_TARGET0
{
    // テクスチャから色を抜く
    float4 texCol = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    
    // C++から送った透明度 (g_ghost_param.x) を適用
    float alpha = g_ghost_param.x;
    
    // テクスチャ本来の色に、少しだけ「残像カラー(g_color)」を混ぜると雰囲気が出ます
    // 一旦、テクスチャの色をそのまま出して alpha だけ適用します
    float3 finalRGB = texCol.rgb * g_color.rgb;
    
    return float4(finalRGB, texCol.a * alpha);
}