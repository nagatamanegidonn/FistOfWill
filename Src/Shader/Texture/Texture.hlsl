#include "../Common/Pixel/PixelShader2DHeader.hlsli"

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	float2 uv = PSInput.uv;

	// UV座標とテクスチャを参照して、最適な色を取得する
	float4 srcCol = tex.Sample(texSampler, uv);
	if (srcCol.a < 0.01f)
	{
		// 描画しない(アルファテスト)
		discard;
	}

	return srcCol;
}