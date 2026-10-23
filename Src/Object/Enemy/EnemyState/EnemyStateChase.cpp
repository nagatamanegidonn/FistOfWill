#include "EnemyStateChase.h"

#include "../Object/Common/Transform.h"
#include "../../CharaStatus.h"
#include "../../CharaBase.h"
#include "../../ActorBase.h"
//#include "../../Player/PlayerBase.h"
#include "../EnemyBase.h"

#include "../../Component/ComponentBase.h"
#include "../../State/StateBase.h"
#include "../../Common/AnimationController.h"

#include "EnemyStateIdle.h"
#include "EnemyStateBattleIdle.h"
#include "EnemyStateDamage.h"
#include "EnemyStateAttack.h"

namespace
{
	static const std::wstring ANIM_RUN = L"Run";
}

EnemyStateChase::EnemyStateChase(std::weak_ptr<ActorBase> _actor)
	:
	CharacterStateBase(_actor)
{
	m_State = STATE::IDLE;
}

void EnemyStateChase::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());
	chara.lock()->SetIsReaction(false);
}

void EnemyStateChase::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());
	chara.lock()->GetAnimController().lock()->Play(ANIM_RUN);

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		component->Update();
	}

	// ダメージを受けていたら
	if (enemy.lock()->GetIsReaction())
	{
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		ChangeState(std::make_shared<EnemyStateDamage>(owner_));
		return;
	}
	// 安全確認：ターゲットを見失ったら Idle ステートに戻る
	if (enemy.lock()->GetTarget().expired()) {
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));
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
			ChangeState(std::make_shared<EnemyStateAttack>(owner_));
		}
		else
		{
			ChangeState(std::make_shared<EnemyStateBattleIdle>(owner_));
		}
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		return;
	}
	else if (distance > 1000.0f)// 索敵範囲外にでたら
	{
		enemy.lock()->ResetTarget();
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));
		chara.lock()->SetMovePow(Utility::VECTOR_ZERO);
		return;
	}

	// 移動方向（moveDir_）を設定
	VECTOR moveDir = VNorm(toTarget); // 正規化して純粋な「向き」にする
	chara.lock()->SetMovePow(VScale(moveDir, 5.0f));       // CharaBaseに移動方向を教える


}

void EnemyStateChase::Exit()
{
}