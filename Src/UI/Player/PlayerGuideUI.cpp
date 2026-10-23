#include "PlayerGuideUI.h"
#include "../Object/Common/InputController.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Application.h"
#include "../ImageUI.h"

namespace
{

}

PlayerGuideUI::PlayerGuideUI(std::shared_ptr<InputController> input)
	:
	inputController_(input)
{
	float x = Application::SCREEN_SIZE_X - 300.0f;
	float y = Application::SCREEN_SIZE_Y - 30.0f;

	// 画像のUIを作成
	// 拡大率や前面描画などを指定
	auto AttackUI = std::make_shared<ImageUI>(Vector2(x, y), L"L_click");
	AttackUI->Init();
	AttackUI->SetScale(2.0f);
	auto SkillUI = std::make_shared<ImageUI>(Vector2(x + 100.0f, y), L"Key_E");
	SkillUI->Init();
	SkillUI->SetScale(2.0f);
	auto StepUI = std::make_shared<ImageUI>(Vector2(x + 200.0f, y), L"R_click");
	StepUI->Init();
	StepUI->SetScale(2.0f);

	AddOtherUI(AttackUI);
	AddOtherUI(SkillUI);
	AddOtherUI(StepUI);

	auto Fream = std::make_shared<ImageUI>(Vector2(x, y - 65.0f), L"Fream");
	Fream->Init();
	Fream->SetScale(1.4f);
	AddOtherUI(Fream);
	Fream = std::make_shared<ImageUI>(Vector2(x + 100, y - 65.0f), L"Fream");
	Fream->SetScale(1.4f);
	Fream->Init();
	AddOtherUI(Fream);
	Fream = std::make_shared<ImageUI>(Vector2(x + 200, y - 65.0f), L"Fream");
	Fream->Init();
	Fream->SetScale(1.4f);
	AddOtherUI(Fream);

	auto Attack = std::make_shared<ImageUI>(Vector2(x, y - 65.0f), L"Attack");
	Attack->Init();
	AddOtherUI(Attack);

	auto Skill = std::make_shared<ImageUI>(Vector2(x + 100.0f, y - 65.0f), L"Skill");
	Skill->Init();
	Skill->SetAngle(0.3f);
	AddOtherUI(Skill);

	auto Step = std::make_shared<ImageUI>(Vector2(x + 200.0f, y - 65.0f), L"Step");
	Step->Init();
	AddOtherUI(Step);
}

PlayerGuideUI::~PlayerGuideUI(void)
{
}

void PlayerGuideUI::Update(void)
{
	if (!m_isDraw)return;



}

void PlayerGuideUI::Draw(void) const
{
	if (!m_isDraw)return;

	// 現在の操作に合わせてガイドUIを描画する処理をここに追加する
	if(inputController_.lock())
	{
		auto input = inputController_.lock();
		if (input->IsPeripheralTriggered(InputController::PeripheralType::KEYBOARD))
		{
			// キーボード操作時のガイドUI描画
			for (auto& ui : guideUIs_)
			{
				ui.lock()->Draw();
			}

		}
		else if (input->IsPeripheralTriggered(InputController::PeripheralType::GAME_PAD))
		{
			// ゲームパッド操作時のガイドUI描画
			for (auto& ui : guideUIs_)
			{
					ui.lock()->Draw();
				
			}
		}
	}
}

void PlayerGuideUI::AddOtherUI(std::shared_ptr<UIBase> ui)
{
	otherUIs_.push_back(ui);
}

void PlayerGuideUI::AddGuideUI(std::shared_ptr<UIBase> guideUI)
{
	guideUIs_.push_back(guideUI);
}

void PlayerGuideUI::EnableDraw(void)
{
	UIBase::EnableDraw();
	for (auto& ui : otherUIs_)
	{
		ui.lock()->EnableDraw();
	}
	for (auto& ui : guideUIs_)
	{
		ui.lock()->EnableDraw();
	}
}

void PlayerGuideUI::DisableDraw(void)
{
	UIBase::DisableDraw();
	for (auto& ui : otherUIs_)
	{
		ui.lock()->DisableDraw();
	}
	for (auto& ui : guideUIs_)
	{
		ui.lock()->DisableDraw();
	}
}
