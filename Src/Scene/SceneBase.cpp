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

void SceneBase::OnPopScene(void)
{
	if (SceneUI_ != nullptr)
	{
		SceneUI_->EnableDraw();
	}
}
