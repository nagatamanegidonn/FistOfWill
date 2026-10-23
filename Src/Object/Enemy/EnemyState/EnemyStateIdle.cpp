
#include "../EnemyBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "EnemyStateDamage.h"
#include "EnemyStateChase.h"
#include "EnemyStateIdle.h"

namespace
{
	static const std::wstring ANIM_IDLE = L"Idle";
}

EnemyStateIdle::EnemyStateIdle(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void EnemyStateIdle::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
}

void EnemyStateIdle::Update()
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

	if (!enemy.lock()->GetTarget().expired()) {
		ChangeState(std::make_shared<EnemyStateChase>(owner_));
		return;
	}
	if (enemy.lock()->GetIsReaction())
	{
		ChangeState(std::make_shared<EnemyStateDamage>(owner_));
	}
}

void EnemyStateIdle::Exit()
{
}