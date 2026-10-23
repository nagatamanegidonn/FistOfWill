#include "EnemyStateBattleIdle.h"


#include "../EnemyBase.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"

#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "EnemyStateDamage.h"
#include "EnemyStateAttack.h"
#include "EnemyStateChase.h"
#include "EnemyStateIdle.h"

namespace
{
	static const std::wstring ANIM_IDLE = L"BattleIdle";
}

EnemyStateBattleIdle::EnemyStateBattleIdle(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void EnemyStateBattleIdle::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);
}

void EnemyStateBattleIdle::Update()
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

	// ターゲットが消えたらIdleに
	if (enemy.lock()->GetTarget().expired()) {
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));
		return;
	}
	if (enemy.lock()->GetIsReaction())
	{
		ChangeState(std::make_shared<EnemyStateDamage>(owner_));
		return;
	}

	// ターゲット（Player）への方向ベクトルを計算する
	const VECTOR& myPos = enemy.lock()->GetTransform().pos;
	const VECTOR& targetPos = enemy.lock()->GetTarget().lock()->GetTransform().pos;

	VECTOR toTarget = VSub(targetPos, myPos);
	toTarget.y = 0.0f; // 高低差で傾かないように水平ベクトルにする

	// 距離を計算（近すぎたら追跡を止めて攻撃ステートに遷移させるため）
	float distance = VSize(toTarget);

	if (distance < enemy.lock()->GetAttackRange())// 攻撃射程内に入ったら
	{
		// 敵がその場で立ち止まるように移動をクリア
		if (enemy.lock()->IsEnableAttack())
		{
			ChangeState(std::make_shared<EnemyStateChase>(owner_));
		}
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		return;
	}
	else if (distance >= enemy.lock()->GetAttackRange())// 攻撃範囲外にでたら
	{
		ChangeState(std::make_shared<EnemyStateChase>(owner_));
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		return;
	}
	else if (distance > 1000.0f)// 索敵範囲外にでたら
	{
		enemy.lock()->GetTarget();
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		return;
	}
}

void EnemyStateBattleIdle::Exit()
{
}