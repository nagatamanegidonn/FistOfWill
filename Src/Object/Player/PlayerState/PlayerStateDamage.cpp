#include "PlayerStateDamage.h"
#include "PlayerStateRun.h"
#include "PlayerStateDead.h"

#include "../PlayerBase.h"
#include "../../CharaStatus.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"
// コンポーネント
#include "../../Component/MoveApplyComponent.h"
#include "../../Component/GoalRotateComponent.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"


namespace
{
	static const std::wstring ANIM_DMG = L"Damage";
}

PlayerStateDamage::PlayerStateDamage(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void PlayerStateDamage::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->SetIsReaction(false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) { move->SetActive(false); }
		std::shared_ptr<GoalRotateComponent> rota = std::dynamic_pointer_cast<GoalRotateComponent>(component);
		if (rota != nullptr) { rota->SetActive(false); }
	}
}

void PlayerStateDamage::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_DMG, false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara.lock());

	if (player.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		AfterDamage();
		ChangeState(std::make_shared<PlayerStateRun>(owner_));
	}
	// ダメージ中にリアクション
	else if (player.lock()->GetIsReaction())
	{
		chara.lock()->SetIsReaction(false);
		chara.lock()->GetAnimController().lock()->Play(ANIM_DMG, false, false, true);
		ChangeState(std::make_shared<PlayerStateDamage>(owner_));
	}
	else if (chara.lock()->GetCharaStatus()->GetNowHP() <= 0)
	{
		AfterDamage();
		ChangeState(std::make_shared<PlayerStateDead>(owner_));
	}
}

void PlayerStateDamage::Exit()
{
}

void PlayerStateDamage::AfterDamage(void)
{
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	for (auto& component : chara.lock()->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) { move->SetActive(true); }
		std::shared_ptr<GoalRotateComponent> rota = std::dynamic_pointer_cast<GoalRotateComponent>(component);
		if (rota != nullptr) { rota->SetActive(true); }
	}
}