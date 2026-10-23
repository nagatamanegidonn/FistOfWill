#include "ForwardAttack.h"
#include "../Attack/AttackData.h"
#include "../CharaBase.h"

#include "../Common/Collider/Geometry/Sphere.h"
#include "../Common/AnimationController.h"

#include "../Manager/General/Effect/EffectManager.h"

#include "../../Utility/Utility.h"

ForwardAttack::ForwardAttack(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data)
	: AttackBase(owner, data)
{
}

void ForwardAttack::Init()
{
	auto ownerPtr = owner_.lock();
	if (!ownerPtr) return;

	// 親の特定のボーン位置を取得して自身の初期位置にする
	transform_.pos = Utility::MV1GetFreamPos(ownerPtr->GetTransform().modelId, attackData_->frameName);
	transform_.Update();

	// 当たり判定（Sphere）を自身に作成
	std::unique_ptr<Geometry> geo = std::make_unique<Sphere>(transform_.pos, transform_.pos, attackData_->colliderRadius);
	MakeCollider(attackData_->colliderTag, std::move(geo), {});

	// エフェクト再生
	EffectManager::GetInstance().CreateEffect(L"HitEffect", transform_.pos);
}

void ForwardAttack::Update()
{
	auto ownerPtr = owner_.lock();
	if (!ownerPtr)
	{
		Delete(); // 親がいなくなったら自身も消滅
		return;
	}

	// 毎フレーム、親のアニメーションに合わせて位置を追従（ここが近接の肝）
	transform_.pos = VAdd(owner_.lock()->GetTransform().pos, VScale(owner_.lock()->GetTransform().GetForward(), attackData_->colliderRadius / 2.0f));
	transform_.pos = VAdd(transform_.pos, VScale(owner_.lock()->GetTransform().GetUp(), 100.0f));
	transform_.Update();

	// アニメーションの再生時間がキャンセル可能時間を超えたら、攻撃コライダーを削除して自身も削除する
	auto chara = std::dynamic_pointer_cast<CharaBase>(ownerPtr);
	if (chara->GetAnimController().lock()->GetStepTime() >= attackData_->cancelableTime)
	{
		// 自身のオーナー（ActorBase）を取得
		if (auto actor = owner_.lock())
		{
			// 自身の生成した、または残っている攻撃コライダーをタグ指定で削除（Kill）する
			actor->DeleteColliderAtTag(attackData_->colliderTag);
		}
		// 削除予約
		Delete();
	}

}