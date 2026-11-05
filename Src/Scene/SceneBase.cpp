#include <DxLib.h>
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
	// ‰½‚à‚µ‚È‚¢
}

void SceneBase::Update(void)
{
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
	// ”ñ“¯Šú“Ç‚Ýž‚Ý‚ªŠ®—¹‚µ‚Ä‚¢‚é‚©Šm”F
	return GetASyncLoadNum() == 0;
}

void SceneBase::OnPopScene(void)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->EnableDraw();
	}
}
