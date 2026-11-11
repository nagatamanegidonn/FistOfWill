#include <DxLib.h>
#include "../Manager/General/LoadingManager.h"
#include "../Manager/General/SceneManager.h"
#include "../UI/Scene/SceneUI.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
{
	// UI
	SceneUI_ = std::make_shared<SceneUI>();
	SceneUI_->Init();
	SceneUI_->EnableDraw();

	// 処理の登録
	updataFunc_ = std::bind(&SceneBase::LoadingUpdate, this);
	drawFunc_ = std::bind(&SceneBase::LoadingDraw, this);
}

SceneBase::~SceneBase(void)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->Delete();
	}
}

void SceneBase::Init(void)
{
}

void SceneBase::AsyncPreLoad(void)
{
	// 非同期読み込み開始
	LoadingManager::GetInstance().StartLoading();

	// シーン内のリソースを読み込む
	//resMng_.SceneChangeResource(static_cast<int>(scnMng_.GetSceneID()));
}

void SceneBase::Update(void)
{
	updataFunc_();
}

void SceneBase::Draw(void)
{
	drawFunc_();
}

void SceneBase::Release(void)
{
}

void SceneBase::AddOtherUI(std::shared_ptr<UIBase> ui)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->AddOtherUI(ui);
	}
}

bool SceneBase::IsLoad(void) const
{
	// 非同期読み込みが完了しているか確認
	return GetASyncLoadNum() == 0;
}

void SceneBase::LoadingUpdate(void)
{
	// ローディング中の場合
	if (LoadingManager::GetInstance().IsLoading())
	{
		// ローディング中の更新処理
		LoadingManager::GetInstance().Update();
		return;
	}

	// サウンドのリソースの切り替え
	//sndMng_.SceneChangeResources();

	// 初期化
	Init();

	// 処理の変更
	// 更新関数のセット
	updataFunc_ = std::bind(&SceneBase::NormalUpdate, this);
	drawFunc_ = std::bind(&SceneBase::NormalDraw, this);

	//フェードアウト開始
	SceneManager::GetInstance().StartFade();
}

void SceneBase::NormalUpdate(void)
{
}

void SceneBase::LoadingDraw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);
}

void SceneBase::NormalDraw(void)
{
}

void SceneBase::OnPopScene(void)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->EnableDraw();
	}
}
