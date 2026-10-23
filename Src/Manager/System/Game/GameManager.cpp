#include <DxLib.h>
#include <fstream>
#include "../Lib/nlohmann/json.hpp"

#include "../Application.h"

#include "GameManager.h"

GameManager* GameManager::instance_ = nullptr;

GameManager::GameManager(void)
	:
	stagePath_(L"CreateStageData_Game.json"),
	enemyPath_(L"CreateEnemyData.json"),
	gameResult_(GAME_RESULT::NONE),
	capturedScreenGraph_(-1),
	ControllerId_(0),
	charId_(0),
	weponId_(0),
	IsHost_(true),
	mainScreen_(-1)
{
	ControllerId_ = DX_INPUT_PAD1;

	IsHost_ = true;
}

void GameManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new GameManager();
	}
	instance_->Init();
}

GameManager& GameManager::GetInstance(void)
{
	return *instance_;
}

void GameManager::Init(void)
{
	//接続はHost
	IsHost_ = true;
}

void GameManager::GameInit(void)
{
	//ゲーム結果初期化
	gameResult_ = GAME_RESULT::NONE;

}

void GameManager::Destroy(void)
{
	//自身のインスタンス削除
	delete instance_;
	instance_ = nullptr;
}

GameManager::GAME_RESULT GameManager::GetGameResult(void)
{
	return gameResult_;
}

void GameManager::SetGameResult(GAME_RESULT result)
{
	gameResult_ = result;
}

const void GameManager::CaptureMainScreen(void)
{
	// 古い画像があれば削除
	if (capturedScreenGraph_ != -1)
	{
		DeleteGraph(capturedScreenGraph_);
		capturedScreenGraph_ = -1;
	}

	// mainScreen_ の内容を新しいグラフィックとして保存
	capturedScreenGraph_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);

	// mainScreen_ の内容をコピー
	GetDrawScreenGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, capturedScreenGraph_, FALSE);
}
const void GameManager::DrawCapturedScreen(int _x, int _y) const
{
	if (capturedScreenGraph_ != -1)
	{
		DrawGraph(_x, _y, capturedScreenGraph_, TRUE);
	}
}

void GameManager::SetHost(bool value)
{
	IsHost_ = value;
}
