#include <DxLib.h>
#include "../Manager/General/LoadingManager.h"
#include "../UI/Scene/SceneUI.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
{
	// UI
	SceneUI_ = std::make_shared<SceneUI>();
	SceneUI_->Init();
	SceneUI_->EnableDraw();
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
	loading_.StartLoading();

	// シーン内のリソースを読み込む
	resMng_.SceneChangeResource(static_cast<int>(scnMng_.GetSceneID()));
}

void SceneBase::Update(void)
{
	// ローディング中の場合
	if (loading_.IsLoading())
	{
		// ローディング中の更新処理
		loading_.Update();
		return;
	}

	// サウンドのリソースの切り替え
	//sndMng_.SceneChangeResources();

	// 初期化
	Init();

	// 処理の変更
	ChangeNormal();

	//フェードイン開始
	scnMng_.StartFadeIn();
}

void SceneBase::Draw(void)
{
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

void SceneBase::OnPopScene(void)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->EnableDraw();
	}
}
