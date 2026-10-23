
#include "../Manager/Resource/ResourceManager.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Game/CollisionManager.h"

#include "ActorBase.h"

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

ActorBase::ActorBase(void)
	: 
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	isDelete_(false)
{
	// 半径
	broudRadius_ = 0.0f;
}

ActorBase::~ActorBase(void)
{
	for (auto& collider : collider_)
	{
		// 所持している全コライダの削除
		collider->Kill();
	}

	collider_.clear();
}

void ActorBase::InitParam(const InitParameter& parameter)
{
	// モデルの基本設定
	//transform_.SetModel(-1);
	transform_.scl = parameter.scl;
	transform_.pos = parameter.pos;
	transform_.quaRot = Quaternion::Euler({
			Utility::Deg2RadF(parameter.rot.x),
			Utility::Deg2RadF(parameter.rot.y),
			Utility::Deg2RadF(parameter.rot.z) });
	transform_.quaRotLocal = Quaternion::Euler({
			Utility::Deg2RadF(parameter.lrot.x),
			Utility::Deg2RadF(parameter.lrot.y),
			Utility::Deg2RadF(parameter.lrot.z) });
	transform_.Update();
}

void ActorBase::DrawShadow(void)
{
	if (transform_.modelId != -1)MV1DrawModel(transform_.modelId);
}

void ActorBase::Sweep(void)
{
	// 削除された判定を配列から破棄
	std::erase_if(collider_, [](std::shared_ptr<Collider>& _collider) {return _collider->IsDead(); });
}

void ActorBase::MakeCollider(const Collider::TAG _tag, std::unique_ptr<Geometry> _geometry, const std::set<Collider::TAG> _notHitTags)
{
	// 情報を使ってコライダの作成
	std::shared_ptr<Collider> collider = std::make_shared<Collider>(*this, _tag, std::move(_geometry), _notHitTags);

	// コライダを管理マネージャーに追加
	CollisionManager::GetInstance().AddCollider(collider);

	// 配列に格納
	collider_.push_back(collider);
}

void ActorBase::DeleteColliderAtTag(Collider::TAG _tag)
{
	for (auto& collider : collider_)
	{
		// そのコライダが存在しない　又は　指定タグと違う
		if (collider == nullptr || collider->GetTag() != _tag)continue;
		collider->Kill();
	}

	// 削除
	std::erase_if(collider_, [](std::shared_ptr<Collider>& _collider) {return _collider->IsDead(); });
}

void ActorBase::DeleteAllCollider(void)
{
	for (auto& collider : collider_)
	{
		// そのコライダが存在しない
		if (collider == nullptr)continue;
		collider->Kill();
	}
	collider_.clear();
}
