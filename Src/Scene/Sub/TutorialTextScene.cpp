#include <DxLib.h>
#include "../Application.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/System/InputManager.h"
#include "../Manager/System/Game/TutorialManager.h"

#include "../UI/Scene/SceneUI.h"
#include "../UI/Text/SentenceUI.h"
#include "../UI/ButtonUI.h"

#include "TutorialTextScene.h"

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

TutorialTextScene::TutorialTextScene(const std::wstring text)
	:
	timeRate_(1.0f)
{
	// UI
	// スタートボタンUI
	auto BackUI = std::make_shared<ButtonUI>(B1_C_POS, Vector2(WIDTH, HEIGHT));
	BackUI->Init();
	BackUI->SetText(L"OK");
	BackUI->EnableDraw();
	BackUI->SetFunc([this]() {
		ResetGame();
		});
	SceneUI_->AddButtonUI(BackUI);

	// 文章UI
	auto TextUI = std::make_shared<SentenceUI>();
	TextUI->Init();
	TextUI->DisableDraw();
	TextUI->SetSentence(L"");
	SceneUI_->AddOtherUI(TextUI);
	SentenceUI_ = TextUI;

	// 現在のプレイヤーカメラ（自分自身）のポインタを用意
	auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();
	// 現在使用中の playerCamera を渡して PauseCamera に切り替える
	auto pauseCam = std::make_shared<PauseCamera>(currentCam);
	SceneManager::GetInstance().GetCameraBase().lock()->ChangeCamera(pauseCam);

	timeRate_ = SceneManager::GetInstance().GetGameSpeedRate();
	SceneManager::GetInstance().SetGameSpeedRate(0.0f);

	SentenceUI_->AddSentence(text);
}

TutorialTextScene::~TutorialTextScene(void)
{
}

void TutorialTextScene::Update(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_Q))
	{
		ResetGame();
		return;
	}
}

void TutorialTextScene::Draw(void)
{
}

void TutorialTextScene::ResetGame(void) const
{
	if (!TutorialManager::GetInstance().IsTutorialIndexValid())
	{
		// マウスを表示状態にする
		SetMouseDispFlag(true);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SELECT);
	}
	else
	{
		auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();
		std::shared_ptr<PauseCamera> pauseCam = std::dynamic_pointer_cast<PauseCamera>(currentCam);
		pauseCam->Resume(); // PauseCamera を解除して元のカメラに戻す

		SceneManager::GetInstance().SetGameSpeedRate(timeRate_);
		SceneManager::GetInstance().PopScene();
	}
}
