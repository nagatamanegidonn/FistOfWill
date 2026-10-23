
#include "../EnemyBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "TutorialStateDamage.h"
#include "TutorialStateIdle.h"
#include "TutorialStateAttack.h"

namespace
{
	static const std::wstring ANIM_IDLE = L"BattleIdle";
}

TutorialStateIdle::TutorialStateIdle(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void TutorialStateIdle::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_IDLE);
}

void TutorialStateIdle::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_IDLE);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());

	if (enemy.lock()->GetIsReaction())
	{
		ChangeState(std::make_shared<TutorialStateDamage>(owner_));
	}
	else if (enemy.lock()->IsEnableAttack())
	{
		ChangeState(std::make_shared<TutorialStateAttack>(owner_));
	}
}

void TutorialStateIdle::Exit()
{
}