// --- ピクセルシェーダー ---
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

// マテリアルから送られてくるベースの色（C++の SetConstBufPS(0, ...)）
cbuffer ConstantBuffer : register(b4)
{
    float4 MaterialColor;
}

// 頂点シェーダーの出力構造体と一致させる
struct PS_INPUT
{
    float4 svPos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
    // 他の項目（worldPos等）が必要なら追加
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // 1. マテリアルから送られてくる色（緑など）
    float4 baseColor = MaterialColor;
    
    // 2. 頂点カラー（C++側でセットした GetColorU8(255,255,255,255)）
    // もし描画が黒いなら、ここが 0 になっています。
    float4 vColor = input.diffuse;

    // 3. 掛け合わせ（テクスチャなしの最小構成）
    float4 finalColor = baseColor * vColor;

    // 【テスト1】とにかく MaterialColor を出す
    // もしこれで緑（など）が出るなら、行列計算が直って「形」が戻った証拠です
    return input.diffuse;

    /* // 【テスト2】形が戻ったら、こちらを有効にしてください。
    // もしこれで真っ黒になるなら、頂点カラーのアルファが 0 になっています。
    float4 finalColor = MaterialColor * input.diffuse;
    return finalColor;
    */
}