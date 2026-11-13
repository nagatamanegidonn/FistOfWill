#pragma once

#include "UIBase.h"

class LoadingUI :
    public UIBase
{
public:

    //ローディング
    static constexpr int COMMA_MAX_NUM = 7;											//「now loading......」のコンマの数
    static constexpr float COMMA_TIME = 0.5f;										//「now loading......」のコンマ数を増やす時間

    // コンストラクタ
    LoadingUI(void);

    // デストラクタ
    virtual ~LoadingUI(void) = default;

    // 初期化
    virtual void Init(void) override;

    // 更新
    virtual void Update(void) override;

    // 描画
    virtual void Draw(void) const override;

private:

    // アニメーション角度
    float m_angle_;

    float m_loadTime_;

    // 回転速度
    static constexpr float ROTATION_SPEED = 2.5f;

    // ドットの数
    static constexpr int DOT_COUNT = 8;

    // 円の半径
    static constexpr float ORBIT_RADIUS = 30.0f;
};