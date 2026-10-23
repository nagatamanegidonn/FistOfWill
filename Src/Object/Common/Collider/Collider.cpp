//#include"../pch.h"
#include "../../ActorBase.h"
#include "Geometry/Geometry.h"
#include "Collider.h"

Collider::Collider(ActorBase& _owner, const TAG _tag, std::unique_ptr<Geometry> _geometry, const std::set<TAG> _notHitTags) :
	owner_(_owner),
	myTag_(_tag),
	geometry_(std::move(_geometry)),
	notHitTags_(_notHitTags)
{
	isHit_ = false;
	isDead_ = false;
}

Collider::~Collider(void)
{
	notHitTags_.clear();
	hitCollider_.reset(); // 当たった相手へのweak_ptrを明示的に解放
	geometry_.reset();    // ジオメトリも明示的に解放
}

void Collider::Kill(void)
{
	isDead_ = true;
}

void Collider::OnHit(const std::weak_ptr<Collider> _collider)
{
	//死亡済み
	if (isDead_)return;

	//この当たり判定が当たった
	isHit_ = true;

	//親に相手のコライダを渡す
	owner_.OnHit(_collider);

	//当たり判定が終わった
	isHit_ = false;
}
