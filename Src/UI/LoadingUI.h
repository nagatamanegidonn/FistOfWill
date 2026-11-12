#pragma once

#include "UIBase.h"

class LoadingUI :
    public UIBase
{
public:

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

    // 回転速度
    static constexpr float ROTATION_SPEED = 2.5f;

    // ドットの数
    static constexpr int DOT_COUNT = 8;

    // 円の半径
    static constexpr float ORBIT_RADIUS = 30.0f;
};