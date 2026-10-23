#pragma once
#include <DxLib.h>
#include "SwordTrailMaterial.h"
#include "SwordTrailData.h"

class SwordTrailRenderer
{
public:

    SwordTrailRenderer(SwordTrailMaterial& material);
    void Draw(const SwordTrailData& trail);

private:
    SwordTrailMaterial& material_;
    // 使っていない場合は消す、使う場合はcppに実装を書く
    // void SetShaderSettings(); 


    // シェーダ設定(頂点)
    void SetReserveVS(void);

    // シェーダ設定(ピクセル)
    void SetReservePS(void);
};