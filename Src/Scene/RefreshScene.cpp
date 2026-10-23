#include<DxLib.h>
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Utility/Measure.h"

#include "../Manager/System/InputManager.h"
#include "../Manager/System/Game/GameManager.h"
#include "../Manager/General/SceneManager.h"

#include "../UI/ImageUI.h"
#include "../UI/ButtonUI.h"
#include "../UI/Scene/SceneUI.h"

#include "RefreshScene.h"
#include "../Manager/Resource/SoundManager.h"

namespace
{
	// 音量倍率
	constexpr float SOUND_RATE = 0.8f;
	// ボタンサイズ
	const int WIDTH = 200;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	// ボタン位置
	const int B1_Y = Application::SCREEN_SIZE_Y - 100;
	const Vector2 B1_C_POS = Vector2(845, 80);

	// startボタンPos
	const int B2_Y = B1_Y;
	const Vector2 B2_C_POS = Vector2(HX, B2_Y);
	const Vector2 END_C_POS = Vector2(HX, B2_Y + 40);
}

RefreshScene::RefreshScene(void)
	:
	SceneBase()
{
}

RefreshScene::~RefreshScene(void)
{
}

void RefreshScene::Init(void)
{
	// 画像の追加
	std::shared_ptr<ImageUI> Back = nullptr;
	std::shared_ptr<ImageUI> Logo = nullptr;

	SceneManager& sns = SceneManager::GetInstance();
	switch (sns.GetGameResult())
	{
	case SceneManager::GAME_RESULT::NONE:
		break;
	case SceneManager::GAME_RESULT::GAME_CLEAR:
		// 音の設定
		SoundManager::GetInstance().Play(L"GAMECLEAR_BGM", Sound::TIMES::LOOP);
		SoundManager::GetInstance().ChengeVolume(L"GAMECLEAR_BGM", SOUND_RATE);
		Back = std::make_shared<ImageUI>(Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2), L"GameClear");
		Back->Init();
		Back->SetFrontDraw(true);
		Back->SetScale(1.0f);
		AddOtherUI(Back);
		Logo = std::make_shared<ImageUI>(Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2), L"GameClearLogo");
		Logo->Init();
		Logo->SetFrontDraw(true);
		Logo->SetScale(0.4f);
		AddOtherUI(Logo);
		break;
	case SceneManager::GAME_RESULT::GAME_OVER:
		// 音の設定
		SoundManager::GetInstance().Play(L"GAMEOVER_BGM", Sound::TIMES::LOOP);
		SoundManager::GetInstance().ChengeVolume(L"GAMEOVER_BGM", SOUND_RATE);
		Back = std::make_shared<ImageUI>(Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2), L"GameOver");
		Back->Init();
		Back->SetFrontDraw(true);
		Back->SetScale(1.0f);
		AddOtherUI(Back);
		Logo = std::make_shared<ImageUI>(Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2), L"GameOverLogo");
		Logo->Init();
		Logo->SetFrontDraw(true);
		Logo->SetScale(0.4f);
		AddOtherUI(Logo);
		break;
	}
	
	// 終了ボタンUI
	auto endUI = std::make_shared<ButtonUI>(END_C_POS, Vector2(WIDTH, HEIGHT));
	endUI->Init();
	endUI->SetText(L"タイトルに戻る");
	endUI->EnableDraw();
	endUI->SetFunc([this]() { GameEnd(); });
	SceneUI_->AddButtonUI(endUI);

	// スタートボタンUI
	auto StartUI = std::make_shared<ButtonUI>(B2_C_POS, Vector2(WIDTH, HEIGHT));
	StartUI->Init();
	StartUI->SetText(L"セレクトに戻る");
	StartUI->EnableDraw();
	StartUI->SetFunc([this]() { GameStart(); });
	SceneUI_->AddButtonUI(StartUI);
}

void RefreshScene::Update(void)
{
	// ゲーム時間進行
	SceneManager::GetInstance().ForwardGameTime();//進めるゲーム時間(GameTotalTime　+=　デルタタイム)
}

void RefreshScene::Draw(void)
{
}

void RefreshScene::Release(void)
{
}

void RefreshScene::GameStart(void)
{
	SoundManager::GetInstance().Stop(L"GAMECLEAR_BGM");
	SoundManager::GetInstance().Stop(L"GAMEOVER_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SELECT);
}

void RefreshScene::GameEnd(void)
{

	SoundManager::GetInstance().Stop(L"GAMECLEAR_BGM");
	SoundManager::GetInstance().Stop(L"GAMEOVER_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
}