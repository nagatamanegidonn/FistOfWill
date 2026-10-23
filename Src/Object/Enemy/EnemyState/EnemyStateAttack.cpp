
#include "../EnemyBase.h"
#include "../../CharaBase.h"
#include "../../CharaStatus.h"
#include "../../ActorBase.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/Resource/SoundManager.h"
#include "../Manager/General/ActorManager.h"
#include "../Manager/General/Effect/EffectManager.h"
#include "../Manager/System/Camera/CameraBase.h"
#include "../../Common/AnimationController.h"

#include "../../Component/ComponentBase.h"
#include "../../Component/MoveApplyComponent.h"

#include "../../Common/Collider/Collider.h"
#include "../../Common/Collider/Geometry/Geometry.h"
#include "../../Common/Collider/Geometry/Sphere.h"

#include "../../../Utility/Utility.h"
#include "../../../Utility//ModelFrameUtility.h"

#include "../../State/StateBase.h"
#include "EnemyStateIdle.h"
#include "EnemyStateAttack.h"
#include "EnemyStateDead.h"
#include "EnemyStateDamage.h"

#include "../../Attack/AttackData.h"
#include "../../Attack/AttackBase.h"
#include "../../Attack/BulletAttack.h"
#include "../../Attack/ForwardAttack.h"

namespace
{
	static const std::wstring FRAME_LATTACK = L"mixamorig:LeftHand";
	static const std::wstring FRAME_RATTACK = L"mixamorig:RightHand";
}

EnemyStateAttack::EnemyStateAttack(std::weak_ptr<ActorBase> _actor, int attackIndex)
	:
	CharacterStateBase(_actor),
	attackIndex_(attackIndex)
{
	m_State = STATE::ATTACK;
	isNextAttack_ = false;
	isAttackStarted_ = false;

	// 外部（この場合は定数配列）から現在の段数のデータを取得
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	currentData_ = chara->GetAttackData(attackIndex_);

	// カウンターのタイミングを知らせる音再生
	SoundManager::GetInstance().Play(L"Counter", Sound::TIMES::ONCE, true);

	// エフェクト再生
	justPos_ = Utility::MV1GetFreamPos(owner_.lock()->GetTransform().modelId, L"mixamorig:Hips");
	EffectManager::GetInstance().CreateEffect(L"FlashEffect", justPos_);
}

void EnemyStateAttack::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	// データに基づいた初期化
	chara.lock()->GetCharaStatus()->SetAT(currentData_->attackPower);
	chara.lock()->GetCharaStatus()->SetArmor(CharaStatus::Armor::Middle);
	chara.lock()->GetAnimController().lock()->Play(currentData_->animationId, false);

	justPos_ = Utility::MV1GetFreamPos(owner_.lock()->GetTransform().modelId, L"mixamorig:Hips");

	// 回避用判定
	std::unique_ptr<Geometry> geo = std::make_unique<Sphere>(justPos_, justPos_, 200.0f);
	owner_.lock()->MakeCollider(Collider::TAG::EVASION, std::move(geo), {});

	for (auto& component : chara.lock()->GetComponents())
	{
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr)
		{
			move->SetActive(false);
		}
	}
}

void EnemyStateAttack::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());

	// 攻撃アニメーションの再生
	chara.lock()->GetAnimController().lock()->Play(currentData_->animationId, false);

	// もし次の攻撃可能でなければ
	if (!isNextAttack_)
	{
		if (!isAttackStarted_ &&
			chara.lock()->GetAnimController().lock()->GetStepTime() >= currentData_->startTime)
		{
			isAttackStarted_ = true;
			// ジャスト判定にならないようにする
			owner_.lock()->DeleteColliderAtTag(Collider::TAG::EVASION);
			chara.lock()->GetCharaStatus()->SetArmor(CharaStatus::Armor::Heaviest);

			std::shared_ptr<AttackBase> newAttack = nullptr;
			// 攻撃種類が変わったの使い分け
			switch (currentData_->attackType)
			{
			case AttackType::Melee:
				newAttack = std::make_shared<ForwardAttack>(owner_.lock(), currentData_);
				break;
			case AttackType::Bullet:
				newAttack = std::make_shared<BulletAttack>(owner_.lock(), currentData_);
				break;
			default:
				break;
			}

			// 生成したオブジェクトをゲーム全体を管理するマネージャー（Sceneなど）に登録する
			if (newAttack) { ActorManager::Get().Entry(newAttack); }
		}
		if (chara.lock()->GetAnimController().lock()->GetStepTime() >= currentData_->cancelableTime)
		{
			isNextAttack_ = true;
			// 自身のオーナー（ActorBase）を取得
			if (auto actor = owner_.lock())
			{
				// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
				actor->DeleteColliderAtTag(Collider::TAG::ATTACK);
			}
		}
		// 攻撃移動
		AttackMove();
	}

	// コンポーネントの更新
	for (auto& component : chara.lock()->GetComponents())
	{
		// dynamic_pointer_cast の戻り値は std::shared_ptr です
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) { continue; }
		component->Update();
	}

	// 回避判定用の位置を調整
	justPos_ = Utility::MV1GetFreamPos(owner_.lock()->GetTransform().modelId, L"mixamorig:Hips");

	if (chara.lock()->GetAnimController().lock()->IsAnimEnd())
	{
		ChangeStateUpdate();
		// Idle状態に変更
		ChangeState(std::make_shared<EnemyStateIdle>(owner_));		
	}
	else if (enemy.lock()->GetIsReaction())
	{
		ChangeStateUpdate();
		ChangeState(std::make_shared<EnemyStateDamage>(owner_));
	}
	else if (chara.lock()->GetCharaStatus()->GetNowHP() <= 0)
	{
		ChangeStateUpdate();
		ChangeState(std::make_shared<EnemyStateDead>(owner_));
	}
}

void EnemyStateAttack::Exit()
{
	// 自身のオーナー（ActorBase）を取得
	if (auto actor = owner_.lock())
	{
		// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
		actor->DeleteColliderAtTag(Collider::TAG::ATTACK);
		actor->DeleteColliderAtTag(Collider::TAG::EVASION);
	}
}

void EnemyStateAttack::AttackMove(void)
{
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	// 現在の位置を取得
	VECTOR pos = chara->GetTransform().pos;

	VECTOR moveVec = chara->GetTransform().GetForward();
	float moveSpeed = currentData_->forwardSpeed;

	auto target = chara->GetTarget();
	if (target.lock())
	{
		moveVec = VNorm(VSub(target.lock()->GetTransform().pos, pos));
	}

	// 移動量を加算して位置を更新
	pos = VAdd(pos, VScale(moveVec, moveSpeed));

	// 更新した位置を設定
	chara->SetMovePow(VScale(moveVec, moveSpeed));
}

void EnemyStateAttack::ChangeStateUpdate()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::weak_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	std::weak_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(chara.lock());

	// 攻撃力とアーマーを元に戻す
	chara.lock()->GetCharaStatus()->SetAT(10);
	chara.lock()->GetCharaStatus()->SetArmor(CharaStatus::Armor::Light);
	chara.lock()->SetIsReaction(false);
	enemy.lock()->SetAttackCoolTime(5.0f);
	for (auto& component : chara.lock()->GetComponents())
	{
		// 移動量適用コンポーネントを有効にする
		std::shared_ptr<MoveApplyComponent> move = std::dynamic_pointer_cast<MoveApplyComponent>(component);
		if (move != nullptr) move->SetActive(true);
	}
}
