#pragma once
#include "ImageUI.h"

class PixelMaterial;
class PixelRenderer;

class SyaderImageUI :
    public ImageUI
{
public:

    // コンストラクタ
    SyaderImageUI(Vector2 pos,Vector2 size, const std::wstring& resourceKey = L"");
    // デストラクタ
    virtual ~SyaderImageUI(void) = default;

    // UIBaseのオーバーライド
    virtual void Init(void) override;
    virtual void Draw(void) const override;

protected:

	Vector2 size_; // 描画サイズ

    std::unique_ptr<PixelMaterial> material_;
    std::unique_ptr<PixelRenderer> renderer_;

};

