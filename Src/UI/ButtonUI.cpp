#include "ButtonUI.h"
#include <DxLib.h>
#include "../Manager/System/InputManager.h"
#include "../Manager/Resource/SoundManager.h"
#include "../Manager/General/SceneManager.h"

ButtonUI::ButtonUI(void)
	:
	UIBase(),
	pos_(Vector2(100, 100)),
	posLocal_(Vector2(0, 0)),
	size_(Vector2(100, 50))
{
	text_ = L"";
	function_ = {};
}

ButtonUI::ButtonUI(Vector2 pos, Vector2 size)
	:
	UIBase(),
	pos_(pos),
	posLocal_(Vector2(0,0)),
	size_(size)
{
	text_ = L"";
	function_ = {};
}

ButtonUI::~ButtonUI(void)
{
}

void ButtonUI::Update(void)
{
}

void ButtonUI::Draw(void) const
{
	if (!m_isDraw)return;

	Vector2 halfSize = { size_.x / 2 ,size_.y / 2 };
	Vector2 pos = { pos_.x + posLocal_.x ,pos_.y + posLocal_.y };

	// 黒塗り
	DrawBox(pos.x - halfSize.x, pos.y - halfSize.y,
		pos.x + halfSize.x, pos.y + halfSize.y, 0x000000, true);
	// 白枠
	DrawBox(pos.x - halfSize.x, pos.y - halfSize.y,
		pos.x + halfSize.x, pos.y + halfSize.y, 0xffffff, false);

	if (text_ == L"")
	{
		return;
	}

	// 文字列の長さを取得する関数を wcslen（ワイド文字用）に変更
	auto StrWidth = GetDrawStringWidth(text_.c_str(), static_cast<int>(wcslen(text_.c_str())));
	int recoveryY = 8;

	// 文字の描画
	DrawFormatString(pos.x - StrWidth / 2, pos.y - recoveryY, 0xff0000, text_.c_str());
}
