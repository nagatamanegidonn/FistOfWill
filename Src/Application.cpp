#include <iostream>
#include <string>
#include <thread>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "Lib/ImGuiWrapper.h"

#include "Manager/System/InputManager.h"
#include "Manager/System/InputTextManager.h"
#include "Manager/General/SceneManager.h"
#include "Manager/Resource/ResourceManager.h"
#include "Manager/Resource/SoundManager.h"
#include "../UI/UIManager.h"

#include "FpsControl/FpsControl.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::wstring Application::PATH_IMAGE = L"Data/Image/";
const std::wstring Application::PATH_MODEL = L"Data/Model/";
const std::wstring Application::PATH_EFFECT = L"Data/Effect/";
const std::wstring Application::PATH_SOUND = L"Data/Sound/";
const std::wstring Application::PATH_SHADER = L"Data/Shader/";
const std::wstring Application::PATH_JSON = L"Data/Json/";

const std::wstring Application::PATH_DATA		= L"Data/";
const std::wstring Application::PATH_MAP_DATA	= PATH_DATA + L"MapBig.csv";
const std::wstring Application::PATH_IMG_PLAYER1 = PATH_IMAGE + L"Player/P1/";
const std::wstring Application::PATH_IMG_PLAYER2 = PATH_IMAGE + L"Player/P2/";

Application::Application(void)
{
	isError_ = false;
	isEnd_ = false;
}

Application::Application(const Application& ins)
{
	isError_ = false;
	isEnd_ = false;
}

Application::~Application(void)
{
	isError_ = false;
	isEnd_ = false; 
	fps_ = nullptr;
}

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText(L"意志の拳");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	// ２重起動検査回避用
	SRand(GetNowCount());

	//この2つすごい大事・・・・・・
	SetDoubleStartValidFlag(true);
	SetAlwaysRunFlag(true);

	int rand = GetRand(999);
	std::wstring name = L"UDP Test";

	name += std::to_wstring(rand);
	SetMainWindowClassName(name.c_str());

	// ★ DxLib_Init() より前に設定する
	// Effekseerのネットワーク機能を初期化する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// FPS初期化
	fps_ = std::make_unique<FpsControl>(FPS_RATE);

	if (DxLib_Init() == -1)
	{
		isError_ = true;
		return;
	}

	// エフェクト関連の初期化
	InitEffekseer();

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	InputTextManager::CreateInstance();

	// リソース管理初期化
	ResourceManager::CreateInstance();

	//音の初期化
	SoundManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();

	// ImGui初期化
	ImGuiWrapper::CreateInstance();

	//UI
	auto& uiManager = UIManager::GetInstance();
}

void Application::Run(void)
{
	auto& imGuiWrapper = ImGuiWrapper::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0 && !isEnd_
#ifdef _DEBUG
		&& CheckHitKey(KEY_INPUT_ESCAPE) == 0
#endif // DEBUG
		)
	{
		InputManager::GetInstance().Update();
		InputTextManager::GetInstance().Update();

		imGuiWrapper.Update();

		//UIマネージャー
		UIManager::GetInstance().Update();

		SceneManager::GetInstance().Update();

		SceneManager::GetInstance().Draw();

		// 頂点バッファに溜まった頂点データを描画する
		// DxLibは特定条件のもと、まとめて描画する機能がある
		// そのため、ScreenFlip関数で溜まっていたDxLib分の描画が実行され、
		// imGuiの描画が上書きされてしまう場合がある
		// それを無くすために、RenderVertex関数で、溜め分を描画しておく
		// 処理速度的にはちょっと懸念
		RenderVertex();
		imGuiWrapper.Draw();


#ifdef _DEBUG
		// デバッグフレーム描画
		fps_->Draw();
#endif
		ScreenFlip();

		// フレームレートの計算
		fps_->Wait();
	}
}

void Application::Destroy(void)
{           

	ImGuiWrapper::GetInstance().Destroy();
	InputManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	SoundManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();

	// Effekseerを終了する。
	Effkseer_End();

	if (DxLib_End() == -1)
	{
		isError_ = true;
		return;
	}

	if (instance_ != nullptr)
	{
		Application* p = instance_;
		instance_ = nullptr; // 二重解放を防ぐ安全対策
		delete p;            // デストラクタが走り、recvData_ や各種std::wstringが解放される
	}
}

bool Application::IsError(void) const
{
	return isError_;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	// これが無いと2Dは絶対に出ない
	Effekseer_Set2DSetting(SCREEN_SIZE_X, SCREEN_SIZE_Y);
	// Effekseerのネットワーク機能を初期化する。
	//SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	// 画面モード変更時にグラフィックスシステムの設定やグラフィックハンドルをリセットしないようにする。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
