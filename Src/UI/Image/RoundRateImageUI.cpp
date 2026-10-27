#include "RoundRateImageUI.h"
#include <algorithm>
#include <DxLib.h>

// コンストラクタ
RoundRateImageUI::RoundRateImageUI(
    Vector2 pos,
    const std::wstring& resourceKey)
    :
    ImageUI(pos, resourceKey),
    rate_(1.0f)
{
}

// 割合の設定
void RoundRateImageUI::SetRate(float rate)
{
    // 0.0 ～ 1.0 の範囲に制限
    rate_ = std::clamp(rate, 0.0f, 1.0f);
}

void RoundRateImageUI::Update(void)
{
    ImageUI::Update();
}

// 描画処理
void RoundRateImageUI::Draw() const
{
    if (!m_isDraw) return;
    if (handle_ < 0) return;

    // ワールド座標 + ローカル座標
    Vector2 drawPos =
    {
        pos_.x + posLocal_.x,
        pos_.y + posLocal_.y
    };

    // 割合を0～100に変換
    double percent = rate_ * 100.0;

    // 円形画像を描画
    DrawCircleGauge(
        static_cast<int>(drawPos.x), // 中心X
        static_cast<int>(drawPos.y), // 中心Y
        percent,                     // 表示割合
        handle_,                     // 画像ハンドル
        0.0,                         // 開始位置
        static_cast<double>(scale_)  // 拡大率
    );
}