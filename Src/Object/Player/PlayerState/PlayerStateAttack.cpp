
#include "../PlayerBase.h"
#include "../../CharaBase.h"
#include "../../CharaStatus.h"
#include "../../ActorBase.h"

#include "../../Common/AnimationController.h"
#include "../../Common/InputController.h"

#include "../../Component/ComponentBase.h"
#include "../../Component/MoveApplyComponent.h"
#include "../../Component/ControlMoveComponent.h"
#include "../../Component/DrawTrailComponent.h"

#include "../../Common/Collider/Collider.h"
#include "../../Common/Collider/Geometry/Geometry.h"
#include "../../Common/Collider/Geometry/Sphere.h"

#include "../../Attack/AttackData.h"

#include "../../State/StateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDead.h"
#include "PlayerStateStep.h"
#include "PlayerStateRun.h"
#include "PlayerStateDamage.h"

#include "../../../Utility/Utility.h"
#include "../../../Utility//ModelFrameUtility.h"

#include "../../Attack/AttackBase.h"
#include "../../Attack/MeleeAttack.h"
#include "../../Attack/ForwardAttack.h"

#include "../../../Manager/General/ActorManager.h"
#include "../../../Manager/General/SceneManager.h"
#include "../../../Manager/Resource/SoundManager.h"

namespace
{
	static const std::wstring FRAME_LATTACK = L"mixamorig:LeftHand";
	static const std::wstring FRAME_RATTACK = L"mixamorig:RightHand";

	// 攻撃IDに応じた音声識別子の配列
	static const std::wstring ATTACK_SE[] = { L"ATTACK1", L"ATTACK2", L"ATTACK3" , L"ATTACK3" };
}

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<ActorBase> _actor, int attackIndex)
	:
	CharacterStateBase(_actor),
	attackIndex_(attackIndex)
{
	m_State = STATE::ATTACK;
	isNextAttack_ = false;
	isAttackStarted_ = false;
	isSkill_ = false;

	// 外部（この場合は定数配列）から現在の段数のデータを取得
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	currentData_ = chara->GetAttackData(attackIndex_);
}

void PlayerStateAttack::Init()
{
	// 次の状態を自分の状態を入れる
	ChangeState(nullptr);

	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	// データに基づいた初期化
	chara->GetCharaStatus()->SetAT(currentData_->attackPower);
	chara->GetAnimController().lock()->Play(currentData_->animationId, false);
	
	// 攻撃SEの再生
	SoundManager::GetInstance().Play(ATTACK_SE[attackIndex_], Sound::TIMES::ONCE, true);

	// 移動コンポーネントを止める
	for (auto& component : chara->GetComponents())
	{
		std::shared_ptr<ControlMoveComponent> move = std::dynamic_pointer_cast<ControlMoveComponent>(component);
		if (move != nullptr) move->SetActive(false);
		std::shared_ptr<DrawTrailComponent> trail = std::dynamic_pointer_cast<DrawTrailComponent>(component);
		if (trail != nullptr)
		{
			trail->SetActive(true); 
			trail->Init();
			trail->SetAttackData(currentData_);
		}
	}
}

void PlayerStateAttack::Update()
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	// 攻撃アニメーションの再生
	chara->GetAnimController().lock()->Play(currentData_->animationId, false);

	// もし次の攻撃可能でなければ
	if (!isNextAttack_)
	{
		// アニメーション経過時間が閾値を超えたらコンボ受付開始
		if (!isAttackStarted_ &&
			chara->GetAnimController().lock()->GetStepTime() >= currentData_->startTime)
		{
			isAttackStarted_ = true;

			std::shared_ptr<AttackBase> newAttack = nullptr;

			// ここが「攻撃種類が変わった時」の使い分け処理です！
			switch (currentData_->attackType)
			{
			case AttackType::Melee:
				newAttack = std::make_shared<ForwardAttack>(owner_.lock(), currentData_);
				break;

			case AttackType::Bullet:
				//newAttack = std::make_shared<BulletAttack>(owner_.lock(), currentData_);
				newAttack = std::make_shared<MeleeAttack>(owner_.lock(), currentData_);

				break;

			default:
				newAttack = std::make_shared<MeleeAttack>(owner_.lock(), currentData_);
				break;
			}

			// 生成したオブジェクトをゲーム全体を管理するマネージャー（Sceneなど）に登録する
			if (newAttack)
			{
				// 【重要】ゲームフレームワークの仕様に合わせて、管理リストへ追加してください
				// これにより、毎フレーム自動で attack->Update() や Draw() が呼ばれるようになります
				ActorManager::Get().Entry(newAttack);
			}
		}
		if (chara->GetAnimController().lock()->GetStepTime() >= currentData_->cancelableTime)
		{
			isNextAttack_ = true;
			// 自身のオーナー（ActorBase）を取得
			if (auto actor = owner_.lock())
			{
				// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
				actor->DeleteColliderAtTag(Collider::TAG::ATTACK);
			}
		}
		// 攻撃中の移動
		AttackMove();
	}
	else
	{
		chara->SetMovePow(Utility::VECTOR_ZERO);
	}

	// コンポーネントの更新
	for (auto& component : chara->GetComponents())
	{
		std::shared_ptr<ControlMoveComponent> move = std::dynamic_pointer_cast<ControlMoveComponent>(component);
		if (move != nullptr) continue;

		component->Update();
	}

	std::weak_ptr<PlayerBase> player = std::dynamic_pointer_cast<PlayerBase>(chara);

	// Skillのみ先行入力を受け付ける
	if (!isSkill_&&player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::SKILL))
	{
		isSkill_ = true;
	}

	if (chara->GetIsReaction())
	{
		StateAfter();
		ChangeState(std::make_shared<PlayerStateDamage>(owner_));
		return;
	}

	if (chara->GetAnimController().lock()->IsAnimEnd())
	{
		StateAfter();
		ChangeState(std::make_shared<PlayerStateIdle>(owner_));
		return;
	}
	// パリィ攻撃
	else if (isSkill_ && isNextAttack_ && attackIndex_ != 3)
	{
		// ここで次の段数のステートを新しく生成して遷移！
		int nextIndex = 3;
		// 次の攻撃がないいならアイドルに戻る
		if (nextIndex <= -1)
		{
			StateAfter();
			ChangeState(std::make_shared<PlayerStateIdle>(owner_));
			return;
		}
		ChangeState(std::make_shared<PlayerStateAttack>(owner_, nextIndex));
	}
	else if (player.lock()->GetInputController().lock()->IsNew(InputController::KEY::ATTACK)
		&& isNextAttack_)
	{
		// ここで次の段数のステートを新しく生成して遷移！
		int nextIndex = currentData_->nextAttackId;

		// 次の攻撃がないいならアイドルに戻る
		if (nextIndex <= -1)
		{
			StateAfter();
			ChangeState(std::make_shared<PlayerStateIdle>(owner_));
			return;
		}

		// 次の攻撃段数を指定して自分（PlayerStateAttack）を再生成
		ChangeState(std::make_shared<PlayerStateAttack>(owner_, nextIndex));
	}
	else if (player.lock()->GetInputController().lock()->IsTriggered(InputController::KEY::STEP)
		&& isNextAttack_)
	{
		StateAfter();
		// 回避行動を設定
		ChangeState(std::make_shared<PlayerStateStep>(owner_));
	}
	else if (player.lock()->GetInputController().lock()->IsNew(InputController::KEY::MOVE)
		&& chara->GetAnimController().lock()->GetStepTime() >= currentData_->cancelableTime)
	{
		StateAfter();
		//ChangeState(std::make_shared<PlayerStateWalk>(owner_));
		ChangeState(std::make_shared<PlayerStateRun>(owner_));
	}
	else if (chara->GetCharaStatus()->GetNowHP() <= 0)
	{
		StateAfter();
		ChangeState(std::make_shared<PlayerStateDead>(owner_));
	}
}

void PlayerStateAttack::Exit()
{
	// 自身のオーナー（ActorBase）を取得
	if (auto actor = owner_.lock())
	{
		// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
		actor->DeleteColliderAtTag(Collider::TAG::ATTACK);
	}
}

void PlayerStateAttack::AttackMove(void)
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

void PlayerStateAttack::StateAfter(void)
{
	// PlayerBase → CharaBase でもOK（継承関係があるので）
	std::shared_ptr<CharaBase> chara = std::dynamic_pointer_cast<CharaBase>(owner_.lock());
	if (!chara) return;

	SceneManager::GetInstance().SetGameSpeedRate();

	// アニメーションが最後まで終わったらアイドルへ戻る
	chara->GetCharaStatus()->SetAT(10);

	// 移動コンポーネントを再有効化
	for (auto& component : chara->GetComponents())
	{
		std::shared_ptr<ControlMoveComponent> move = std::dynamic_pointer_cast<ControlMoveComponent>(component);
		if (move != nullptr) move->SetActive(true);
		std::shared_ptr<DrawTrailComponent> trail = std::dynamic_pointer_cast<DrawTrailComponent>(component);
		if (trail != nullptr) trail->SetActive(false);
	}
}
