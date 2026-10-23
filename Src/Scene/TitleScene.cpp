#include <string>
#include <DxLib.h>
#include "../Manager/System/InputManager.h"
#include "../Manager/General/Effect/EffectManager.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/Resource/SoundManager.h"

#include "../Common/InputTextArea.h"
#include "../Utility/Utility.h"
#include "../Object/Common/InputController.h"

#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"

#include "../UI/Scene/SceneUI.h"
#include "../UI/ButtonUI.h"

#include "Sub/PauseScene.h"
#include "TitleScene.h"
#include "../Manager/System/Game/GameManager.h"

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
	const int B1_Y = Application::SCREEN_SIZE_Y - 100;
	const Vector2 B1_C_POS = Vector2(845, 80);

	// startボタンPos
	const int B2_Y = B1_Y;
	const Vector2 B2_C_POS = Vector2(HX, B2_Y);
	const Vector2 END_C_POS = Vector2(HX, B2_Y + 40);
}

TitleScene::TitleScene(void)
	:
	SceneBase()
{
	inputController_ = nullptr;
}

TitleScene::~TitleScene(void)
{
	DeleteGraph(titleImg_);
	DeleteGraph(backImg_);
	DeleteGraph(cursorImg_);

	if (ChangeUI_ != nullptr)
	{
		ChangeUI_->Delete();
	}
}

void TitleScene::Init(void)
{
	//コントローラーの登録
	inputController_ = std::make_unique<InputController>(SceneManager::GetInstance().GetControllId());
	inputController_->Update();

	cursorImg_ = LoadGraph((Application::PATH_IMAGE + L"tile_0072.png").c_str());
	backImg_ = LoadGraph((Application::PATH_IMAGE + L"Title.png").c_str());
	titleImg_ = LoadGraph((Application::PATH_IMAGE + L"TitleRogo.png").c_str());

	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

	// 背景画像
	Material_ = std::make_unique<PixelMaterial>(L"Texture.cso", 1);
	Material_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	Material_->AddTextureBuf(backImg_);
	Renderer_ = std::make_unique<PixelRenderer>(*Material_);
	Renderer_->MakeSquereVertex(
		Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);
	
	// UI
	// 終了ボタンUI
	auto endUI = std::make_shared<ButtonUI>(END_C_POS, Vector2(WIDTH, HEIGHT));
	endUI->Init();
	endUI->SetText(L"終了");
	endUI->EnableDraw();
	endUI->SetFunc([this]() { GameEnd(); });
	SceneUI_->AddButtonUI(endUI);

	// スタートボタンUI
	auto StartUI = std::make_shared<ButtonUI>(B2_C_POS, Vector2(WIDTH, HEIGHT));
	StartUI->Init();
	StartUI->SetText(L"開始");
	StartUI->EnableDraw();
	StartUI->SetFunc([this]() { GameStart(); });
	SceneUI_->AddButtonUI(StartUI);

	// 音の再生
	SoundManager::GetInstance().Play(L"TITLE_BGM", Sound::TIMES::LOOP);
	SoundManager::GetInstance().ChengeVolume(L"TITLE_BGM", SOUND_RATE);

}

void TitleScene::Update(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_Q))
	{
		// タイトルUIの描画を無効化して、デバッグシーンに遷移する
		SceneUI_->DisableDraw();
		auto scene = std::make_shared<PauseScene>();
		scene->Init();
		SceneManager::GetInstance().PushScene(scene);
		return;
	}

	auto& ins = InputManager::GetInstance();
	Vector2 moPos = ins.GetMousePos();

	// コントローラーの更新
	inputController_->Update();
}

void TitleScene::Draw(void)
{
	auto& ins = InputManager::GetInstance();

	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	// 黒背景の描画
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);

	// 背景の描画
	Renderer_->Draw();

	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);
}

void TitleScene::Release(void)
{
}

void TitleScene::GameStart(void)
{
	SoundManager::GetInstance().Stop(L"TITLE_BGM");
	SoundManager::GetInstance().Play(L"START", Sound::TIMES::ONCE, true);

	// マウスを表示状態にする
	SetMouseDispFlag(true);

	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SELECT);
}

void TitleScene::GameEnd(void)
{
	Application::GetInstance().SetIsEnd();
}
