#pragma once
#include <string>
#include <vector>
#include <map>
#include <DxLib.h>

class SwordTrailMaterial
{
public:
    // コンストラクタ
    SwordTrailMaterial(
        std::wstring shaderFileNameVS, int constBufFloat4SizeVS,
        std::wstring shaderFileNamePS, int constBufFloat4SizePS
    );

    // デストラクタ
    ~SwordTrailMaterial(void);

    // 定数バッファを更新 (Rendererから呼ばれる用)
    void SetConstBufVS(int idx, const FLOAT4& contBuf);
    void SetConstBufPS(int idx, const FLOAT4& contBuf);

    // テクスチャを更新
    void SetTextureBuf(int slot, int texHandle);

    // ゲッター
    int GetShaderVS(void) const { return shaderVS_; }
    int GetShaderPS(void) const { return shaderPS_; }
    int GetConstBufVS(void) const { return constBufVS_; }
    int GetConstBufPS(void) const { return constBufPS_; }
    const std::vector<FLOAT4>& GetConstBufsVS(void) const { return constBufsVS_; }
    const std::vector<FLOAT4>& GetConstBufsPS(void) const { return constBufsPS_; }
    const std::map<int, int>& GetTextures(void) const { return textures_; }

    // Renderer側でUpdateShaderConstantBufferを呼ぶために必要
    void UpdateConstantBuffers();

private:
    int shaderVS_;
    int shaderPS_;
    int constBufVS_;
    int constBufPS_;
    std::vector<FLOAT4> constBufsVS_;
    std::vector<FLOAT4> constBufsPS_;
    std::map<int, int> textures_;
};