#include "LoadingManager.h"
#include <DxLib.h>
#include "SceneManager.h"
#include "../Resource/SoundManager.h"

LoadingManager* LoadingManager::instance_ = nullptr;

namespace
{
}

void LoadingManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new LoadingManager();
	}
	instance_->Init();
}

LoadingManager& LoadingManager::GetInstance(void)
{
	return *instance_;
}

void LoadingManager::Init(void)
{
}

void LoadingManager::Update(void)
{
	// ローディング経過時間更新
	auto delta = SceneManager::GetInstance().GetDeltaTime();
	loadingTime_ += delta;

	// 待機時間を超過しているか判断
	// ロードが完了したか判断
	if (GetASyncLoadNum() == 0 && loadingTime_ >= LOADING_TIME)
	{
		EndLoading();
	}
}

void LoadingManager::Draw(void)
{
	DrawFormatString(0, 0, GetColor(255, 255, 255), L"Now Loading");
}

void LoadingManager::Destroy(void)
{
	delete instance_;
}

void LoadingManager::StartLoading(void)
{
	// ローディング中フラグを立てる
	isLoading_ = true;

	// 非同期読み込みを有効にする
	SetUseASyncLoadFlag(true);

	// ローディング時間の初期化
	loadingTime_ = 0.0f;

	// 音停止
	//SoundManager::GetInstance().AllStop();
}

void LoadingManager::EndLoading()
{
	// 非同期処理を無効にする
	SetUseASyncLoadFlag(false);

	// 非同期読み込みの終了判定
	isLoading_ = false;
}