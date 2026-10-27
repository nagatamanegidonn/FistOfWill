#include <DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Utility/Measure.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/General/ActorManager.h"
#include "../Manager/Resource/SoundManager.h"
#include "../Manager/System/InputManager.h"
#include "../Manager/System/Game/CollisionManager.h"
#include "../Manager/System/Game/GameManager.h"

#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/HP/HpGaugeUI.h"
#include "../UI/Scene/SceneUI.h"

#include "../Object/Common/Timer.h"

#include "../Object/Player/PlayerBase.h"
#include "../Object/Player/SelfPlayer.h"
#include "../Object/Player/JoinPlayer.h"

#include "../Object/Stage/StageManager.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/Grid.h"

#include "GameScene.h"
#include "Sub/PauseScene.h"

namespace
{
	// UIサイズ
	const int WIDTH = 400;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	// UI位置
	const Vector2 START_C_POS = Vector2(WIDTH / 2 + 10, HEIGHT / 2 + 10);

	// タイマーの描画位置
	constexpr int TIME_POS_X = Timer::SIZE_X / 2 + 10;
	constexpr int TIME_POS_Y = Timer::SIZE_Y / 2 + 10;
	constexpr float LIMIT_TIME = 60.0f * 5.0f;
	// 音量倍率
	constexpr float SOUND_RATE = 0.6f;

	constexpr int MAP_SIZE = 200;
	constexpr int MAP_POS_X = MAP_SIZE / 2 + 10;
	constexpr int MAP_POS_Y = Application::SCREEN_SIZE_Y - MAP_SIZE / 2 - 10;
}


GameScene::GameScene(void)
	:
	SceneBase(),
	actorManager_(nullptr),
	stageManager_(nullptr)
{
	stepCountDown_ = 0;
	players_.clear();

	skyDome_ = nullptr;
	grid_ = nullptr;

	CollisionManager::CreateInstance();
}

GameScene::~GameScene(void)
{
	SceneManager::GetInstance().SetTotalGameTime(0.0f);
	SoundManager::GetInstance().AllStop();

	stageManager_->End();
	actorManager_->End();
}

void GameScene::Init(void)
{
	// 初期化
	SceneManager::GetInstance().SetGameResult(SceneManager::GAME_RESULT::NONE);

	actorManager_ = std::make_shared<ActorManager>();
	actorManager_->Init();

	// Jsonによる敵の生成
	actorManager_->CreateActorJson(GameManager::GetInstance().GetEnemyPath());

	// ステージの生成
	stageManager_ = std::make_shared<StageManager>(actorManager_);
	stageManager_->Init();
	stageManager_->LoadStageJson(GameManager::GetInstance().GetStagePath());

	// プレイヤーの生成
	std::shared_ptr<PlayerBase> player = nullptr;
	player = std::make_shared<SelfPlayer>(-1);
	actorManager_->Entry(player);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player->GetTransform());
	skyDome_->Init();

	// 背景初期化
	grid_ = std::make_unique<Grid>(); 
	grid_->Init();

	// ゲーム開始待機時間
	stepCountDown_ = 1.5f;

	// ダウンした回数
	downCnt_ = 0;

	// 音の設定
	SoundManager::GetInstance().Play(L"BATTLE_BGM", Sound::TIMES::LOOP);
	SoundManager::GetInstance().ChengeVolume(L"BATTLE_BGM", SOUND_RATE);

	// 計測開始
	timer_ = std::make_unique<Timer>(LIMIT_TIME);
	timer_->Start();

	// アクターのスタート処理
	actorManager_->Start();

	// ゲームマネージャーのゲーム変数初期化
	GameManager::GetInstance().GameInit();
}

void GameScene::Update(void)
{
	// ゲーム時間進行
	SceneManager::GetInstance().ForwardGameTime();//進めるゲーム時間(GameTotalTime　+=　デルタタイム)

	// シーン遷移が間に合ってないプレイヤーのために待機
	float limit = stepCountDown_ - SceneManager::GetInstance().GetTotalGameTime();
	if (limit > 0.0f)
	{
		DrawFormatString(100, 100, 0xffffff, L"%f", limit);
		//return;
	}

#ifdef _DEBUG
	Measure::GetInstance().Start();
	Measure::GetInstance().Watch(L"01 Application START");
#endif

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_Q))
	{
		// タイトルUIの描画を無効化して、デバッグシーンに遷移する
		SceneUI_->DisableDraw();
		auto scene = std::make_shared<PauseScene>();
		scene->Init();
		SceneManager::GetInstance().PushScene(scene);
		return;
	}
	
	// 更新
	actorManager_->Update();

	// 衝突判定
	auto& colMng = CollisionManager::GetInstance();

	//当たり判定更新
	colMng.Update();

	//当たり判定の破棄
	colMng.Sweep();

	// ゲームの勝敗判定
	// プレイヤーが死亡したら
	if (actorManager_->IsGameover())
	{
		// ゲームの勝敗判定
		SceneManager::GAME_RESULT result = SceneManager::GAME_RESULT::GAME_OVER;
		SceneManager::GetInstance().SetGameResult(result);
		//フェードアウト
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RSULT);
	}
	// 敵が全滅したら、ゲームオーバーに
	else if (actorManager_->IsAllDeadEnemys())
	{
		// ゲームの勝敗判定
		SceneManager::GAME_RESULT result = SceneManager::GAME_RESULT::GAME_CLEAR;
		SceneManager::GetInstance().SetGameResult(result);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RSULT);
	}
}

void GameScene::Draw(void)
{
	// 背景、ステージの描画
	skyDome_->Draw();

#ifdef _DEBUG
	grid_->Draw();
#endif

	// 描画
	actorManager_->Draw();
}

void GameScene::Release(void)
{
	actorManager_->End();
	// 当たり判定管理のリセット
	//CollisionManager::GetInstance().DeleteAllCollider();
}

void GameScene::DownCountPuls(void)
{
	downCnt_ += 1;
}



