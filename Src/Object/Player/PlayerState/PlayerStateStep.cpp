#include "PlayerStateStep.h"

#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"
#include "../../Common/InputController.h"
// コンポーネント
#include "../../Component/MoveApplyComponent.h"
#include "../../Component/GoalRotateComponent.h"
#include "../../Component/DrawAfterimageComponent.h"

#include "../Manager/General/SceneManager.h"

#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"

namespace
{
	static const std::wstring ANIM_STEP = L"Step";
}

PlayerStateStep::PlayerStateStep(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor),
	stepDirection_(VGet(0.0f, 0.0f, 0.0f)),
	stepTimeRate_(0.4f)
{
	m_State = STATE::STEP;
}

void PlayerStateStep::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);


	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	stepDirection_ = chara->GetTransform().GetBack();

	// ジャスト回避ならスローモーションに
	if (chara->GetIsJust())SceneManager::GetInstance().SetGameSpeedRate(stepTimeRate_);

	chara->GetAnimController().lock()->Play(ANIM_STEP, false);

	// 移動コンポーネントを止める
	for (auto& component : chara->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) { move->SetActive(false); }
		std::shared_ptr<GoalRotateComponent> rota = std::dynamic_pointer_cast<GoalRotateComponent>(component);
		if (rota != nullptr) { rota->SetActive(false); }
		std::shared_ptr<DrawAfterimageComponent> draf = std::dynamic_pointer_cast<DrawAfterimageComponent>(component);
		if (draf != nullptr) { draf->SetActive(true); }
	}
}

void PlayerStateStep::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara->GetAnimController().lock()->Play(ANIM_STEP, false);

	// ステップ中はスローモーションを徐々に解除していく
	stepTimeRate_ = std::lerp(stepTimeRate_, 1.0f, 0.01f);
	SceneManager::GetInstance().SetGameSpeedRate(stepTimeRate_);

	// アニメーション経過時間が閾値を超えたらコンボ受付開始
	if (chara->GetAnimController().lock()->GetStepTime() <= 35.0f)
	{
		StepMove();
	}

	// コンポーネントの更新
	for (auto& component : chara->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara);

	if (player.lock()->GetInputController().lock()->IsNew(InputController::KEY::DASH))
	{
		player.lock()->SetDash(true);
	}

	if (chara->GetAnimController().lock()->IsAnimEnd())
	{
		StepAfter();
		ChangeState(std::make_shared<PlayerStateIdle>(owner_));
	}
	if (chara->GetAnimController().lock()->GetStepTime() >= 35.0f
		&& player.lock()->GetInputController().lock()->IsNew(InputController::KEY::ATTACK))
	{
		StepAfter();
		ChangeState(std::make_shared<PlayerStateAttack>(owner_));
	}
	// スキル入力があった場合はスキル攻撃に移行
	else if (chara->GetAnimController().lock()->GetStepTime() >= 35.0f
		&& player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::SKILL))
	{
		StepAfter();
		ChangeState(std::make_shared<PlayerStateAttack>(owner_, 3));
	}
	else if (chara->GetAnimController().lock()->GetStepTime() >= 45.0f
		&& (chara->GetMovePow().x != 0.0f || chara->GetMovePow().z != 0.0f)
		/*&& chara->IsDash()*/)
	{
		StepAfter();
		ChangeState(std::make_shared<PlayerStateRun>(owner_));
	}
}

void PlayerStateStep::Exit()
{
}

void PlayerStateStep::StepMove(void)
{
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	// 現在の位置を取得
	VECTOR pos = chara->GetTransform().pos;
	float speed = 12.0f * SceneManager::GetInstance().GetGameSpeedRate();
	// 移動量を加算して位置を更新
	pos = VAdd(pos, VScale(stepDirection_, speed));

	// 更新した位置を設定
	chara->SetPosition(pos);
}

void PlayerStateStep::StepAfter(void)
{
	SceneManager::GetInstance().SetGameSpeedRate();

	// PlayerBase → CharaBase
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara->GetAnimController().lock()->Play(ANIM_STEP, false);

	// 移動コンポーネントを再有効化
	for (auto& component : chara->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) move->SetActive(true);
		std::shared_ptr<GoalRotateComponent> rota = std::dynamic_pointer_cast<GoalRotateComponent>(component);
		if (rota != nullptr) { rota->SetActive(true); }
		std::shared_ptr<DrawAfterimageComponent> draf = std::dynamic_pointer_cast<DrawAfterimageComponent>(component);
		if (draf != nullptr) draf->SetActive(false);
	}
}
