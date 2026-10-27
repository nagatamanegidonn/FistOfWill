
#include "HpGaugeUI.h"
#include "../Object/CharaStatus.h"
#include <DxLib.h>
#include <algorithm>

HpGaugeUI::HpGaugeUI(std::shared_ptr<CharaStatus> status)
	: UIBase()
	, m_charaStatus(status)
	, m_pos(Vector2(100, 100))
	, m_size(Vector2(200, 20))
	, m_value(100.0f)
	, m_maxValue(100.0f)
	, m_barColor(0x00ff00) // 初期色: 緑
{
}

HpGaugeUI::HpGaugeUI(std::shared_ptr<CharaStatus> status,Vector2 pos, Vector2 size)
	: UIBase()
	, m_charaStatus(status)
	, m_pos(pos)
	, m_size(size)
	, m_value(100.0f)
	, m_maxValue(100.0f)
	, m_barColor(0x00ff00) // 初期色: 緑
{
}

HpGaugeUI::~HpGaugeUI()
{
}

void HpGaugeUI::Update()
{
	if (m_charaStatus.expired()) return;
	// アニメーション（徐々にゲージが減る効果など）を追加したい場合はここに記述します

	// 現在値と最大値をセットして表示割合を更新する
	m_value = m_charaStatus.lock()->GetNowHP();
	m_maxValue = m_charaStatus.lock()->GetMaxHP();

}

void HpGaugeUI::Draw() const
{
	if (!m_isDraw) return;

	// 中心座標基準で描画領域を計算（ButtonUIと統一）
	Vector2 halfSize = { m_size.x / 2, m_size.y / 2 };

	int left = static_cast<int>(m_pos.x - halfSize.x);
	int top = static_cast<int>(m_pos.y - halfSize.y);
	int right = static_cast<int>(m_pos.x + halfSize.x);
	int bottom = static_cast<int>(m_pos.y + halfSize.y);

	// 割合の計算 (0.0f ~ 1.0f にクランプして安全性を確保)
	float rate = 0.0f;
	if (m_maxValue > 0.0f)
	{
		rate = std::clamp(m_value / m_maxValue, 0.0f, 1.0f);
	}

	// ゲージの中身（中身の右端X座標を割合に応じて計算）
	int gaugeRight = left + static_cast<int>(m_size.x * rate);

	// 下地（背景・ダメージ領域など：黒や暗い赤）
	DrawBox(left, top, right, bottom, 0x333333, true);

	// ゲージ本体（現在の値分を塗る）
	if (gaugeRight > left)
	{
		DrawBox(left, top, gaugeRight, bottom, m_barColor, true);
	}

	// 枠線（白枠）
	DrawBox(left, top, right, bottom, 0xffffff, false);
}