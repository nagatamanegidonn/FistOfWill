#pragma once
#include "ImageUI.h"

class RoundRateImageUI :
    public ImageUI
{
public:

    // コンストラクタ
    RoundRateImageUI(Vector2 pos, const std::wstring& resourceKey = L"");
    // デストラクタ
    virtual ~RoundRateImageUI(void) = default;

    // 更新処理
    virtual void Update(void) override;

    // UIBaseのオーバーライド
    virtual void Draw(void) const override;

    // 割合の設定（0.0f ～ 1.0f）
    void SetRate(float rate);

    // 割合の取得
    float GetRate(void) const { return rate_; }

protected:

    // 表示割合（0.0f ～ 1.0f）
    float rate_;

};