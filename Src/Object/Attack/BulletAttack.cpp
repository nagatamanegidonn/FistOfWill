#include "BulletAttack.h"
#include "../Attack/AttackData.h"

#include "../Common/Collider/Geometry/Sphere.h"

#include "../../Manager/General/SceneManager.h"

namespace
{
	const float BULLET_LIFETIME = 30.0f; // 弾の生存時間（秒）
}

BulletAttack::BulletAttack(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data)
	: 
	AttackBase(owner, data)
{
}

void BulletAttack::Init()
{
	auto ownerPtr = owner_.lock();
	if (!ownerPtr) return;

	// 生成時の位置と向きを親（プレイヤー）に合わせる
	transform_.pos = ownerPtr->GetTransform().pos;
	transform_.quaRot = ownerPtr->GetTransform().quaRot;
	transform_.Update();

	// 当たり判定を生成
	std::unique_ptr<Geometry> geo = std::make_unique<Sphere>(transform_.pos, transform_.pos, attackData_->colliderRadius);
	MakeCollider(attackData_->colliderTag, std::move(geo), {});

	lifeTimer_ = BULLET_LIFETIME; // 生存時間タイマーをリセット
}

void BulletAttack::Update()
{
	// 親から独立しているため、ownerPtr がいなくても前進し続ける
	VECTOR moveVec = transform_.GetForward();
	AddMove(VScale(moveVec, attackData_->forwardSpeed)); // CSVの前進速度で進む

	// 一定時間経過したら自動消滅
	lifeTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	if (lifeTimer_ < 0.0f)
	{
		Delete(); // ActorBaseの削除フラグを立てる
	}
}

void BulletAttack::Draw()
{
#ifdef _DEBUG
	DrawSphere3D(transform_.pos, attackData_->colliderRadius, 16, GetColor(255, 0, 0), GetColor(255, 255, 255), true);
#endif // _DEBUG
}
