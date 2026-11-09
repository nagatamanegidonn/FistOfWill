#include "LoadingManager.h"
#include <DxLib.h>
#include "../Resource/SoundManager.h"

namespace
{
	//最低ロードフレーム
	constexpr int kMinLoadFrame = 180;
	//ロード終了後すぐに切り替わらないようにするためのフレーム数
	constexpr int kAfterLoadFrame = 30;

	//Loading座標
	constexpr float kLoadingTextPosX = 50.0f;
	constexpr float kLoadingTextPosY = 50.0f;
	//.座標
	constexpr float kLoadingPointTextPosX = kLoadingTextPosX + 280.0f;
	constexpr float kLoadingPointTextOffsetPosX = 30.0f;
	constexpr float kLoadingPointTextPosY = kLoadingTextPosY;

	//ロードテキスト座標
	constexpr float kTextPosX = 200.0f;
	constexpr float kTextPosY = 160.0f;
	constexpr float kTextPosOffsetY = 28.0f;

	//.を表示する間隔
	constexpr int kResetPointFrame = 40;
	constexpr int kPointFrame = 10;
	//フォント
	const std::wstring kNotoName = L"Noto Sans JP Light";
	constexpr int kFontSize = 20;
}

void LoadingManager::Init(void)
{
	m_loadCountFrame = 0;
	m_afterLoadFrame = 0;
	m_loadingPointCountFrame = 0;
}

void LoadingManager::Update(void)
{
	if (m_isLoading)
	{
		auto& soundManager = SoundManager::GetInstance();
		//全ての音を一時停止
		//soundManager.AllStop();

		//Loadingの.の更新
		++m_loadingPointCountFrame;
		if (m_loadingPointCountFrame > kResetPointFrame)m_loadingPointCountFrame = 0;

		//ロード数取得
		int loadNum = GetASyncLoadNum();

		//最低ロード時間
		if (m_loadCountFrame > 0)
		{
			--m_loadCountFrame;
		}

		float rate = 1.0f - (static_cast<float>(GetASyncLoadNum() + m_loadCountFrame) / m_totalLoadNum);
		rate *= 100.0f;
	}
}

void LoadingManager::Draw(void)
{
	if (m_isLoading && m_totalLoadNum > 0)
	{

	}
}

void LoadingManager::End(void)
{
}

void LoadingManager::StartLoading(void)
{
	// ローディング中フラグを立てる
	isLoading_ = true;

	// 非同期読み込みを有効にする
	SetUseASyncLoadFlag(true);

	// ローディング時間の初期化
	loadingTime_ = 0.0f;

	//ロード時間カウントリセット
	m_loadCountFrame = kMinLoadFrame;

	//ロード終了後すぐに切り替わらないようにするためのフレーム数リセット
	m_afterLoadFrame = kAfterLoadFrame;

	//音停止
	SoundManager::GetInstance().AllStop();
}

void LoadingManager::EndLoading()
{
	//現在のロード数取得
	m_totalLoadNum = GetASyncLoadNum() + m_loadCountFrame;

	//非同期ロード終了
	SetUseASyncLoadFlag(false);
}