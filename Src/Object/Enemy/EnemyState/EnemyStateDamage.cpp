
#include "../EnemyBase.h"
#include "../../CharaStatus.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "EnemyStateIdle.h"
#include "EnemyStateDead.h"
#include "EnemyStateDamage.h"
#include "EnemyStateAttack.h"

namespace
{
	static const std::wstring ANIM_DMG = L"Damage";
}

EnemyStateDamage::EnemyStateDamage(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void EnemyStateDamage::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	chara.lock()->SetIsReaction(false);
}

void EnemyStateDamage::Update()
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

	if(enemy.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		enemy.lock()->SetIsReaction(false);
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));
	}
	// ダメージ中にリアクション
	else if (enemy.lock()->GetIsReaction())
	{
		chara.lock()->SetIsReaction(false);
		chara.lock()->GetAnimController().lock()->Play(ANIM_DMG, false, false, true);
		ChangeState(std::make_shared<EnemyStateDamage>(owner_));
	}
	else if (chara.lock()->GetCharaStatus()->GetNowHP() <= 0)
	{
		ChangeState(std::make_shared<EnemyStateDead>(owner_));
	}
	
	else if (enemy.lock()->GetTarget().lock())
	{
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
				ChangeState(std::make_shared<EnemyStateAttack>(owner_));
			}
			chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
			return;
		}
	}
}

void EnemyStateDamage::Exit()
{
}

void EnemyStateDamage::AfterDamage(void)
{

}
