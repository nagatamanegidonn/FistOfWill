#pragma once

#include "../UIBase.h"
#include "../Common/Vector2.h"
#include <memory>

class CharaStatus;

class GaugeUI :
    public UIBase
{
public:

    GaugeUI(std::shared_ptr<CharaStatus> status);
    GaugeUI(std::shared_ptr<CharaStatus> status,Vector2 pos, Vector2 size);
    ~GaugeUI();

    void Update() override;
    void Draw() const override;

    // バーの色を指定する
    void SetColor(unsigned int color) { m_barColor = color; }

private:

    // プレイヤーのステータス
    std::weak_ptr<CharaStatus> m_charaStatus;

    Vector2 m_pos;
    Vector2 m_size;
    // 現在の体力
    float m_value;
	// 最大体力
    float m_maxValue;
    // ゲージの 色
    unsigned int m_barColor;
};

