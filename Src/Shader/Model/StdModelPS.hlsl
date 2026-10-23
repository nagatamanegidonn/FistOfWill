// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
    float2 g_smple;		// 繰り返したい数
    float2 g_color;		// 色
    float3 g_light_dir; // ライトの方向
    float g_time;		// 時間
    float4 g_ambient_color; // 環境光の色
}

// 【追加】シャドウマップ用のテクスチャとサンプラ（スロット1番を想定）
Texture2D shadowMapTexture : register(t1);
SamplerState shadowMapSampler : register(s1);

float4 main(PS_INPUT PSInput) : SV_TARGET0
{
	// 【追加】テクスチャを繰り返したい数を指定（例：縦横に10回ずつ繰り返す）
    float2 tilingFactor = float2(g_smple.x, g_smple.y);
    
    // 【追加】UV座標に掛け算をして、サンプリング用のUVを作る
    float2 tiledUV = PSInput.uv * tilingFactor;
	
	// テクスチャの色取得
    float4 diffuse = diffuseMapTexture.Sample(diffuseMapSampler, tiledUV);
    if (diffuse.a < 0.01f)
    {
        discard;
    }

    float dotColor = dot(PSInput.normal, -g_light_dir);

     // 【追加】影の判定処理
    float shadow = 1.0f; // 1.0 = 日向, 0.5 = 影

    // ライト空間のXY座標（-1.0 ～ 1.0）を UV座標（0.0 ～ 1.0）に変換
    float2 shadowUV;
    shadowUV.x = PSInput.lightAtPos.x * 0.5f + 0.5f;
    shadowUV.y = -PSInput.lightAtPos.y * 0.5f + 0.5f; // DirectXのためY軸を反転

    // テクスチャの範囲内（0.0 ～ 1.0）かどうかチェック
    if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f)
    {
        // 描画中ピクセルのライトからの深度値
        float currentDepth = PSInput.lightAtPos.z;

        // シャドウマップから最も手前にある遮蔽物の深度値をサンプリング
        float closestDepth = shadowMapTexture.Sample(shadowMapSampler, shadowUV).r;

        // 誤差による縞模様（シャドウアクネ）を防ぐためのバイアス値
        float bias = 0.005f;

        // 遮蔽物の方が手前にあれば（現在の深度の方が深ければ）影にする
        if (currentDepth - bias > closestDepth)
        {
            shadow = 0.5f; // 影の濃さ（必要に応じて値を調整してください）
        }
    }
    
    // 最終カラーの計算に影(shadow)を乗算する
    float3 rgb = (diffuse.rgb * dotColor * shadow) + g_ambient_color.rgb + (diffuse.rgb * 0.5);
    
    return float4(rgb, diffuse.a);
    
    return diffuse;
    
    
	//青色になるはず
    return float4(g_light_dir.x, g_light_dir.y, g_light_dir.z, 1.0f);



	//方向で色が変わる
	/*float3 lightDir = g_common.light[0].direction;
	return float4(lightDir.x, lightDir.y, lightDir.z, 1.0f);*/

    return float4(1.0f, 0.0f, 0.0f, 1.0f);

}