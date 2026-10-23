#include "SelectScene.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/General/ActorManager.h"
#include "../Manager/General/Effect/EffectManager.h"
#include "../Manager/System/Camera/CameraBase.h"
#include "../Manager/System/Camera/NormalCamera.h"
#include "../Manager/System/Game/GameManager.h"
#include "../Manager/System/Game/TutorialManager.h"
#include "../Manager/Resource/SoundManager.h"

#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/Scene/SceneUI.h"

#include "../Application.h"
#include "../Object/Player/SelectPlayer.h"
#include "../Object/Stage/StageManager.h"

namespace
{
	// 音量倍率
	constexpr float SOUND_RATE = 0.6f;
	// ボタンサイズ
	const int WIDTH = 200;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	// ボタン位置
	const Vector2 START_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 0);
	const Vector2 OPTION_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 1);
	const Vector2 EXIT_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 2);

	const Vector2 PAGE1_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 0);
	const Vector2 PAGE2_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 1);
	const Vector2 PAGE3_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 2);
	const Vector2 PAGE4_C_POS = Vector2(HX - (HX / 2), HY + (HEIGHT * 2) * 3);
}

SelectScene::SelectScene(void)
	:
	SceneBase()
{
	// UI
	SceneUI_ = std::make_shared<SceneUI>();
	SceneUI_->Init();
	SceneUI_->EnableDraw();
}

SelectScene::~SelectScene(void)
{
	if (ExitUI_ != nullptr)
	{
		ExitUI_->Delete();
	}
	stageManager_->End();
	actorManager_->End();
}

void SelectScene::Init(void)
{
	// 初期化
	SceneManager::GetInstance().SetGameResult(SceneManager::GAME_RESULT::NONE);

	// actorManagerの生成と初期化
	actorManager_ = std::make_shared<ActorManager>();
	actorManager_->Init();

	auto player = std::make_shared<SelectPlayer>();
	actorManager_->Entry(player);

	// ステージの生成
	stageManager_ = std::make_shared<StageManager>(actorManager_);
	stageManager_->Init();
	stageManager_->LoadStageJson(L"CreateStageData_Select.json");

	// アクターのスタート処理
	actorManager_->Start();

	// UI
	// weak_ptr を作成する
	std::weak_ptr<SceneUI> weakSelectUI = SceneUI_; // もしくは管理クラスの weak_ptr

	// スタートボタンUI
	auto StartUI = std::make_shared<ButtonUI>(START_C_POS, Vector2(WIDTH, HEIGHT));
	StartUI->Init();
	StartUI->SetText(L"出撃");
	StartUI->EnableDraw();
	StartUI->SetFunc([weakSelectUI]() {
		// 戻るボタンが押されたら、メインメニュー（0）に戻る
		weakSelectUI.lock()->ChangeMenuState(1);
		});
	SceneUI_->AddButtonUI(StartUI);

	auto DebugUI = std::make_shared<ButtonUI>(OPTION_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"チュートリアル");
	DebugUI->EnableDraw();
	DebugUI->SetFunc([weakSelectUI]() {
		// 戻るボタンが押されたら、メインメニュー（0）に戻る
		weakSelectUI.lock()->ChangeMenuState(2);
		});
	SceneUI_->AddButtonUI(DebugUI);

	// 退出ボタンUI
	auto ExitUI = std::make_shared<ButtonUI>(EXIT_C_POS, Vector2(WIDTH, HEIGHT));
	ExitUI->Init();
	ExitUI->SetText(L"タイトルに戻る");
	ExitUI->EnableDraw();
	ExitUI->SetFunc([this]() { GameExit(); });
	SceneUI_->AddButtonUI(ExitUI);

	// 出撃ページのUI
	DebugUI = std::make_shared<ButtonUI>(PAGE1_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"ステージ1");
	DebugUI->DisableDraw();
	DebugUI->SetFunc([this]() { 
		GameManager::GetInstance().SetEnemyPath(L"CreateEnemyData.json");
		GameManager::GetInstance().SetStagePath(L"CreateStageData_Game2.json");
		GameStart(); 
		});
	SceneUI_->AddButtonUI(1,DebugUI);

	DebugUI = std::make_shared<ButtonUI>(PAGE2_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"ステージ2");
	DebugUI->DisableDraw();
	DebugUI->SetFunc([this]() {
		GameManager::GetInstance().SetEnemyPath(L"CreateEnemyData2.json");
		GameManager::GetInstance().SetStagePath(L"CreateStageData_Game2.json");
		GameStart();
		});
	SceneUI_->AddButtonUI(1,DebugUI);
	// BackUI
	DebugUI = std::make_shared<ButtonUI>(PAGE3_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"戻る");
	DebugUI->DisableDraw();
	DebugUI->SetFunc([weakSelectUI]() {
		// 戻るボタンが押されたら、メインメニュー（0）に戻る
		weakSelectUI.lock()->ChangeMenuState(0);
		});
	SceneUI_->AddButtonUI(1, DebugUI);

	// チュートリアルページのUI
	DebugUI = std::make_shared<ButtonUI>(PAGE1_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"チュートリアル");
	DebugUI->DisableDraw();
	DebugUI->SetFunc([this]() {
		GameManager::GetInstance().SetEnemyPath(L"CreateTutorialEnemyData.json");
		GameManager::GetInstance().SetStagePath(L"CreateStageData_Game2.json");
		TutorialManager::GetInstance().LoadTutorialTexts(L"Data/CSV/TutorialText.csv");
		TutorialStart();
		});
	SceneUI_->AddButtonUI(2, DebugUI);

	DebugUI = std::make_shared<ButtonUI>(PAGE2_C_POS, Vector2(WIDTH, HEIGHT));
	DebugUI->Init();
	DebugUI->SetText(L"戻る");
	DebugUI->DisableDraw();
	DebugUI->SetFunc([weakSelectUI]() {
		// 戻るボタンが押されたら、メインメニュー（0）に戻る
		weakSelectUI.lock()->ChangeMenuState(0);
		});
	SceneUI_->AddButtonUI(2,DebugUI);

	// 音の再生
	SoundManager::GetInstance().Play(L"SELECT_BGM", Sound::TIMES::LOOP);
	SoundManager::GetInstance().ChengeVolume(L"SELECT_BGM", SOUND_RATE);
}

void SelectScene::Update(void)
{
	// 更新
	actorManager_->Update();
}

void SelectScene::Draw(void)
{
	// 描画
	actorManager_->Draw();
}

void SelectScene::Release(void)
{
	if (ExitUI_ != nullptr)
	{
		ExitUI_->Delete();
	}

	actorManager_->End();
}

void SelectScene::GameStart(void)
{
	SoundManager::GetInstance().Stop(L"SELECT_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
}

void SelectScene::TutorialStart(void)
{
	SoundManager::GetInstance().Stop(L"SELECT_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
}

void SelectScene::GameExit(void)
{
	SoundManager::GetInstance().Stop(L"SELECT_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
}
