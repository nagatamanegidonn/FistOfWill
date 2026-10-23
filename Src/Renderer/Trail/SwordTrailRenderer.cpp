#include <DxLib.h>
#include "SwordTrailRenderer.h"

SwordTrailRenderer::SwordTrailRenderer(SwordTrailMaterial& material)
    : material_(material)
{
}

void SwordTrailRenderer::Draw(const SwordTrailData& trail) {
    // 頂点数が足りない場合は何もしない
    int count = (int)trail.GetVertexCount();
    if (count < 3) return;

    // --- 1. 行列の計算と「マテリアルへの蓄積」 ---
    // ここでセットした値は、まだ material_ 内部の vector にあるだけです
    MATRIX world, view, proj;
    GetTransformToWorldMatrix(&world);
    GetTransformToViewMatrix(&view);
    GetTransformToProjectionMatrix(&proj);

    for (int i = 0; i < 4; i++) {
        material_.SetConstBufVS(0 + i, *(FLOAT4*)&world.m[i]);
        material_.SetConstBufVS(4 + i, *(FLOAT4*)&view.m[i]);
        material_.SetConstBufVS(8 + i, *(FLOAT4*)&proj.m[i]);
    }

    // オリジナルシェーダ設定(ON)
    MV1SetUseOrigShader(true);

    // シェーダ設定(頂点)
    SetReserveVS();

    // シェーダ設定(ピクセル)
    SetReservePS();

	// 定数バッファの内容をGPUに転送
    material_.UpdateConstantBuffers();

	// 定数バッファ設定 (スロット番号はマテリアル側で固定)
    SetShaderConstantBuffer(material_.GetConstBufVS(), DX_SHADERTYPE_VERTEX, 7);
    SetShaderConstantBuffer(material_.GetConstBufPS(), DX_SHADERTYPE_PIXEL, 4);

    // テクスチャ設定
    for (auto const& [slot, handle] : material_.GetTextures()) {
        SetUseTextureToShader(slot, handle);
    }

    auto trailVertices = trail.GetVertices();

    // 2. VERTEX3DSHADER の配列を頂点の数分作成して詰め直す
    std::vector<VERTEX3DSHADER> dxVertices(count);
    for (int i = 0; i < count; ++i) {
        dxVertices[i].pos = VGet(trailVertices[i].pos.x, trailVertices[i].pos.y, trailVertices[i].pos.z);

        // 全ての要素を「明示的に」初期化してください。
        // 一つでも飛ばすと、そのメモリ領域が不定になり、色化けの原因になります。
        // dif にアルファを詰め込む

        // 線形フェード（直線的に消える）
        float rate = (float)i / (float)(count - 1);

        // 指数フェード（より自然に、スッと消える感じにするなら rate * rate などにする）
        unsigned char alpha = (unsigned char)(255 * (1.0f - rate));

        // 頂点カラーに反映（白ベースにアルファを乗せる）
        dxVertices[i].dif = GetColorU8(255, 255, 255, alpha);
        //dxVertices[i].dif = GetColorU8( 0, 0, 255,64);

        // 残りを初期化
        dxVertices[i].norm = VGet(0, 0, 0);
        dxVertices[i].u = trailVertices[i].uv.x;
        dxVertices[i].v = trailVertices[i].uv.y;
    }

    // --- カリングを無効化（両面描画） ---
    SetUseBackCulling(DX_CULLING_NONE);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    // --- 【ここが修正の核心】 ---
    // DrawPolygon3DToShader ではなく DrawPrimitive3DToShader を使います。
    // これにより、インデックスバッファなしで「三角形ストリップ」として描画できます。
    // 引数: (頂点配列, 頂点数, プリミティブ形式)
    DrawPrimitive3DToShader(dxVertices.data(), count, DX_PRIMTYPE_TRIANGLESTRIP);

    // --- 描画が終わったらカリング設定を元に戻す ---
    // 通常は左回り(LEFT)か右回り(RIGHT)のいずれかですが、
    // デフォルトに戻す場合は DX_CULLING_LEFT が一般的らしい
    SetUseBackCulling(DX_CULLING_LEFT);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 後始末
    //-----------------------------------------

    // テクスチャ解除
    const auto& textures = material_.GetTextures();
    size_t size = textures.size();
    if (size == 0)
    {
        // 前回使用分のテクスチャを引き継がないように
        SetUseTextureToShader(0, -1);
    }
    else
    {
        for (const auto& pair : textures)
        {
            SetUseTextureToShader(pair.first, -1);
        }
    }

    // 後片付け
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);

    // オリジナルシェーダ設定(OFF)
    MV1SetUseOrigShader(false);
}

void SwordTrailRenderer::SetReserveVS(void)
{
    // 定数バッファハンドル
    int constBuf = material_.GetConstBufVS();

    FLOAT4* constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBuf);
    const auto& constBufs = material_.GetConstBufsVS();

    size_t size = constBufs.size();
    for (int i = 0; i < size; i++)
    {
        if (i != 0)
        {
            constBufsPtr++;
        }
        constBufsPtr->x = constBufs[i].x;
        constBufsPtr->y = constBufs[i].y;
        constBufsPtr->z = constBufs[i].z;
        constBufsPtr->w = constBufs[i].w;
    }

    // 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
    UpdateShaderConstantBuffer(constBuf);

    // 頂点シェーダー用の定数バッファを定数バッファレジスタにセット
    SetShaderConstantBuffer(
        constBuf, DX_SHADERTYPE_VERTEX, /*CONSTANT_BUF_SLOT_BEGIN_VS*/7);

    // 頂点シェーダー設定
    SetUseVertexShader(material_.GetShaderVS());
}

void SwordTrailRenderer::SetReservePS(void)
{
    // ピクセルシェーダーにテクスチャを転送
    const auto& textures = material_.GetTextures();
    size_t size = textures.size();
    if (size == 0)
    {
        // 前回使用分のテクスチャを引き継がないように
        SetUseTextureToShader(0, -1);
    }
    else
    {
        for (const auto& pair : textures)
        {
            SetUseTextureToShader(pair.first, pair.second);
        }
    }

    // 定数バッファハンドル
    int constBuf = material_.GetConstBufPS();

    FLOAT4* constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBuf);
    const auto& constBufs = material_.GetConstBufsPS();

    size = constBufs.size();
    for (int i = 0; i < size; i++)
    {
        if (i != 0)
        {
            constBufsPtr++;
        }
        constBufsPtr->x = constBufs[i].x;
        constBufsPtr->y = constBufs[i].y;
        constBufsPtr->z = constBufs[i].z;
        constBufsPtr->w = constBufs[i].w;
    }

    // ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
    UpdateShaderConstantBuffer(constBuf);

    // ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
    SetShaderConstantBuffer(
        constBuf, DX_SHADERTYPE_PIXEL, /*CONSTANT_BUF_SLOT_BEGIN_PS*/4);

    // ピクセルシェーダー設定
    SetUsePixelShader(material_.GetShaderPS());
}
