#include "SentenceUI.h"
#include <DxLib.h>

#include "../../Application.h"

#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/InputManager.h"

SentenceUI::SentenceUI(void)
	:
	UIBase(),
	sentence_(L""),
	visibleCharCount_(0),
	timer_(0.0f),
	charInterval_(0.05f) // 0.05秒ごとに1文字進む
{
}

SentenceUI::~SentenceUI(void)
{

}

void SentenceUI::Update(void)
{
	// 描画されてないなら更新を抜ける
	if (!m_isDraw)
	{
		return;
	}

	auto& input = InputManager::GetInstance();
	Vector2 moPos = input.GetMousePos();

	// タイマーを更新
	//timer_ += SceneManager::GetInstance().GetDeltaTime();
	timer_ += 0.016f; // 仮のデルタタイム（1フレームあたり約16ms）

	// 全て表示しきっている場合は処理しない
	if (visibleCharCount_ >= sentence_.length())
	{
		if ((timer_ >= 20.0f))
		{
			m_isDraw = false;
		}
		return;
	}

	// 設定した間隔（charInterval_）ごとに表示文字数を増やす
	if (timer_ >= charInterval_)
	{
		timer_ = 0.0f;
		visibleCharCount_++;

		if (visibleCharCount_ >= sentence_.length())
		{
			visibleCharCount_ = sentence_.length();
		}
	}


	// マウスの前フレームの位置を更新
	agoMousePos_ = moPos;
}

void SentenceUI::Draw(void) const
{
	// 描画されてないなら抜ける
	if (!m_isDraw)
	{
		return;
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)122);
	DrawBox(
		160, Application::SCREEN_SIZE_Y - 200,
		Application::SCREEN_SIZE_X - 160,
		Application::SCREEN_SIZE_Y - 100,
		0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (sentence_.empty() || visibleCharCount_ == 0) return;

	// sentence_ から現在表示する分だけ切り出す（部分文字列を取得）
	std::wstring currentText = sentence_.substr(0, visibleCharCount_);

	// DxLibの描画関数で描画
	DrawString(160 + 10, Application::SCREEN_SIZE_Y - 200 + 10, currentText.c_str(), 0xffffff);
}

void SentenceUI::SetSentence(const std::wstring text)
{
	sentence_ = text;
	visibleCharCount_ = 0;
	timer_ = 0.0f;
	m_isDraw = true;
}

void SentenceUI::AddSentence(const std::wstring text)
{
	// 追加後の文字列を仮作成
	std::wstring nextSentence = sentence_ + text;

	// '\n' の個数をカウント
	size_t newlineCount = std::count(nextSentence.begin(), nextSentence.end(), L'\n');

	// '\n' が 5 個以上なら引数の text のみに差し替え、未満なら追記する
	if (newlineCount >= 7 || !m_isDraw)
	{
		SetSentence(text);
		return;
	}
	else
	{
		sentence_ = nextSentence;
	}

	timer_ = 0.0f;
	m_isDraw = true;
}
