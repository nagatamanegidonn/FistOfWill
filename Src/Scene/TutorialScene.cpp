#include <DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/General/ActorManager.h"
#include "../Manager/Resource/SoundManager.h"
#include "../Manager/System/InputManager.h"
#include "../Manager/System/Game/CollisionManager.h"
#include "../Manager/System/Game/GameManager.h"
#include "../Manager/System/Game/TutorialManager.h"

#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/HP/HpGaugeUI.h"
#include "../UI/Scene/SceneUI.h"

#include "../Object/Player/PlayerBase.h"
#include "../Object/Player/SelfPlayer.h"
#include "../Object/Player/JoinPlayer.h"

#include "../Object/Stage/StageManager.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/Grid.h"
#include "../Object/Stage/Destination.h"

#include "TutorialScene.h"
#include "Sub/PauseScene.h"
#include "Sub/TutorialTextScene.h"

namespace
{
	// 音量倍率
	constexpr float SOUND_RATE = 0.6f;
	// UIサイズ
	const int WIDTH = 400;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	// UI位置
	const Vector2 START_C_POS = Vector2(WIDTH / 2 + 10, HEIGHT / 2 + 10);

	constexpr int MAP_SIZE = 200;
	constexpr int MAP_POS_X = MAP_SIZE / 2 + 10;
	constexpr int MAP_POS_Y = Application::SCREEN_SIZE_Y - MAP_SIZE / 2 - 10;
}


TutorialScene::TutorialScene(void)
	:
	SceneBase(),
	actorManager_(nullptr),
	stageManager_(nullptr)
{
	players_.clear();

	skyDome_ = nullptr;

	CollisionManager::CreateInstance();

}

TutorialScene::~TutorialScene(void)
{
	SceneManager::GetInstance().SetTotalGameTime(0.0f);
	SoundManager::GetInstance().AllStop();

	stageManager_->End();
	actorManager_->End();
}

void TutorialScene::Init(void)
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

	// 音の設定
	SoundManager::GetInstance().Play(L"TUTORIAL_BGM", Sound::TIMES::LOOP);
	SoundManager::GetInstance().ChengeVolume(L"TUTORIAL_BGM", SOUND_RATE);

	// アクターのスタート処理
	actorManager_->Start();

	// ゲームマネージャーのゲーム変数初期化
	GameManager::GetInstance().GameInit();
	TutorialManager::GetInstance().GameInit();

	// UI
	// weak_ptr を作成する
	std::weak_ptr<SceneUI> weakSelectUI = SceneUI_; // もしくは管理クラスの weak_ptr

}

void TutorialScene::Update(void)
{
	// ゲーム時間進行
	SceneManager::GetInstance().ForwardGameTime();//進めるゲーム時間(GameTotalTime　+=　デルタタイム)

	TutorialManager::GetInstance().Update();

	if (TutorialManager::GetInstance().IsNextTutorial())
	{
		// タイトルUIの描画を無効化して、デバッグシーンに遷移する
		SceneUI_->DisableDraw();
		auto scene = std::make_shared<TutorialTextScene>(TutorialManager::GetInstance().GetCurrentTutorialText());
		scene->Init();
		TutorialManager::GetInstance().NextTutorial();
		SceneManager::GetInstance().PushScene(scene);
		return;
	}

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
	SceneManager::GAME_RESULT result = SceneManager::GAME_RESULT::NONE;

	//プレイヤーが死亡したら
	if ((actorManager_->IsGameover() || SceneManager::GetInstance().GetGameResult() != SceneManager::GAME_RESULT::NONE))
	{
		// セレクトシーンに移動
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SELECT);
	}
	// (仮)敵が全滅したら、セレクトシーンに戻る
	else if (actorManager_->IsAllDeadEnemys())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SELECT);
	}
}

void TutorialScene::Draw(void)
{
	// 背景、ステージの描画
	skyDome_->Draw();

	// 描画
	actorManager_->Draw();
}

void TutorialScene::Release(void)
{
	actorManager_->End();
}


