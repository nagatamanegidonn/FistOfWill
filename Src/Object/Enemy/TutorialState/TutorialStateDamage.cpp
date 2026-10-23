
#include "../EnemyBase.h"
#include "../../CharaStatus.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "TutorialStateDamage.h"
#include "TutorialStateIdle.h"

namespace
{
	static const std::wstring ANIM_DMG = L"Damage";
}

TutorialStateDamage::TutorialStateDamage(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void TutorialStateDamage::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->SetIsReaction(false);
}

void TutorialStateDamage::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());

	chara.lock()->GetAnimController().lock()->Play(ANIM_DMG, false);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());

	if (enemy.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		enemy.lock()->SetIsReaction(false);
		ChangeState(std::make_shared<TutorialStateIdle>(owner_));
	}
	// ダメージ中にリアクション
	else if (enemy.lock()->GetIsReaction())
	{
		chara.lock()->SetIsReaction(false);
		chara.lock()->GetAnimController().lock()->Play(ANIM_DMG, false, false, true);
		ChangeState(std::make_shared<TutorialStateDamage>(owner_));
	}
}

void TutorialStateDamage::Exit()
{
}