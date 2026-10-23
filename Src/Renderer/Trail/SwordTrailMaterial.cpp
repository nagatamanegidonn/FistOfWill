#include "../../Application.h" // パス解決用
#include "SwordTrailMaterial.h"

SwordTrailMaterial::SwordTrailMaterial(
    std::wstring shaderFileNameVS, int constBufFloat4SizeVS,
    std::wstring shaderFileNamePS, int constBufFloat4SizePS)
{
	std::wstring fullPathVS = Application::PATH_SHADER + shaderFileNameVS;

    // シェーダーロード
    shaderVS_ = LoadVertexShader(fullPathVS.c_str());
    shaderPS_ = LoadPixelShader((Application::PATH_SHADER + shaderFileNamePS).c_str());

    // 定数バッファ作成
    constBufVS_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * constBufFloat4SizeVS);
    constBufsVS_.resize(constBufFloat4SizeVS);

    constBufPS_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * constBufFloat4SizePS);
    constBufsPS_.resize(constBufFloat4SizePS);
}

SwordTrailMaterial::~SwordTrailMaterial(void) {
    DeleteShader(shaderVS_);
    DeleteShader(shaderPS_);
    DeleteShader(constBufVS_);
    DeleteShader(constBufPS_);
}

// 定数バッファのセット関数(頂点シェーダー用)
void SwordTrailMaterial::SetConstBufVS(int idx, const FLOAT4& contBuf) {
    if (idx < constBufsVS_.size()) constBufsVS_[idx] = contBuf;
}

// 定数バッファのセット関数(ピクセルシェーダー用)
void SwordTrailMaterial::SetConstBufPS(int idx, const FLOAT4& contBuf) {
    if (idx < constBufsPS_.size()) constBufsPS_[idx] = contBuf;
}

// テクスチャのセット関数 
void SwordTrailMaterial::SetTextureBuf(int slot, int texHandle) {
    textures_[slot] = texHandle;
}

// 定数バッファの更新関数 (Rendererから呼ばれる用)
void SwordTrailMaterial::UpdateConstantBuffers() {
    // VS
    FLOAT4* vsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBufVS_);
    for (size_t i = 0; i < constBufsVS_.size(); ++i) vsPtr[i] = constBufsVS_[i];
    UpdateShaderConstantBuffer(constBufVS_);

    // PS
    FLOAT4* psPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBufPS_);
    for (size_t i = 0; i < constBufsPS_.size(); ++i) psPtr[i] = constBufsPS_[i];
    UpdateShaderConstantBuffer(constBufPS_);
}