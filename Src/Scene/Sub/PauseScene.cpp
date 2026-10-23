#include <DxLib.h>
#include "../Application.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/System/InputManager.h"

#include "../UI/Scene/SceneUI.h"
#include "../UI/ButtonUI.h"

#include "PauseScene.h"
#include "../Manager/System/Camera/CameraBase.h"
#include "../Manager/System/Camera/PauseCamera.h"

namespace
{
	// ボタンサイズ
	const int WIDTH = 200;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	const Vector2 B1_C_POS = Vector2(HX, HY - 50);
	const Vector2 B2_C_POS = Vector2(HX, HY);
}

PauseScene::PauseScene(void)
	:
	timeRate_(1.0f)
{
	// UI
	// スタートボタンUI
	auto BackUI = std::make_shared<ButtonUI>(B1_C_POS, Vector2(WIDTH, HEIGHT));
	BackUI->Init();
	BackUI->SetText(L"再開");
	BackUI->EnableDraw();
	BackUI->SetFunc([this]() { 
		ResetGame();
		});
	SceneUI_->AddButtonUI(BackUI);

	// スタートボタンUI
	auto StartUI = std::make_shared<ButtonUI>(B2_C_POS, Vector2(WIDTH, HEIGHT));
	StartUI->Init();
	StartUI->SetText(L"ゲーム終了");
	StartUI->EnableDraw();
	StartUI->SetFunc([this]() { 
		// マウスを表示状態にする
		SetMouseDispFlag(true);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		});
	SceneUI_->AddButtonUI(StartUI);

	// 現在のプレイヤーカメラ（自分自身）のポインタを用意
	auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();
	// 現在使用中の playerCamera を渡して PauseCamera に切り替える
	auto pauseCam = std::make_shared<PauseCamera>(currentCam);
	SceneManager::GetInstance().GetCameraBase().lock()->ChangeCamera(pauseCam);

	timeRate_ = SceneManager::GetInstance().GetGameSpeedRate();
	SceneManager::GetInstance().SetGameSpeedRate(0.0f);
}

PauseScene::~PauseScene(void)
{ 
}

void PauseScene::Update(void)
{
	if(InputManager::GetInstance().IsTrgDown(KEY_INPUT_Q))
	{
		ResetGame();
		return;
	}
}

void PauseScene::Draw(void)
{
	int dis = 30;
	int x = Application::SCREEN_SIZE_X - dis;
	int y = Application::SCREEN_SIZE_Y - dis;

	// 半透明の黒いフィルターを描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 122);
	DrawBox(
		0, 0,
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PauseScene::ResetGame(void) const
{
	auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();
	std::shared_ptr<PauseCamera> pauseCam = std::dynamic_pointer_cast<PauseCamera>(currentCam);
	pauseCam->Resume(); // PauseCamera を解除して元のカメラに戻す

	SceneManager::GetInstance().SetGameSpeedRate(timeRate_);
	SceneManager::GetInstance().PopScene();
}
