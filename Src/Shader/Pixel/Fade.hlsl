#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// 定数バッファ：スロット0番目(b0と書く)
cbuffer cbParam : register(b4)
{
    float3 g_dummy;
    float g_rate;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    
    float2 uv = PSInput.uv;
    // UV座標を3倍にして、テクスチャの繰り返しを表現する
    //uv = frac(uv * 3.0f);
    
	// UV座標とテクスチャを参照して、最適な色を取得する
    float4 srcCol = tex.Sample(texSampler, uv);

	//アルファ値
    float alpha = 1.0f - (srcCol.r + srcCol.g + srcCol.b) / (3.0f);
	//alpha += 1.0f;

	//alpha+=g_rateでもいいが全体的に黒く見えるので幅を大きくする
	//1.0fの幅を増やしているのは黒の部分もじっくりとフェードさせるため
    alpha -= 1.0; //{-1.0f}～{0.0f}の状態にする
    alpha += (g_rate * 2.05f); //{0.0f}～{2.0f}の時間割合を加算する

    //return float4(g_color.r, g_color.g, g_color.b, alpha);

	//通常フェード
    return float4(0.0f, 0.0f, 0.0f, alpha);
    return float4(0.1f, 0.1f, 0.1f, g_rate);

}