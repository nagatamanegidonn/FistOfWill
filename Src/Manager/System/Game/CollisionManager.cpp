//#include"../pch.h"
#include"../Object/Common/Transform.h"
#include"../../../Object/Common/Collider/Geometry/Geometry.h"
#include"../../../Object/Common/Collider/Collider.h"
#include"../Object/ActorBase.h"
#include"../Utility/Utility.h"
#include "CollisionManager.h"

#include <algorithm> // std::erase_if を使うなら（C++20）

//静的インスタンスの初期化
CollisionManager* CollisionManager::instance_ = nullptr;

void CollisionManager::CreateInstance(void)
{
	//インスタンスの初期生成
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
}

void CollisionManager::AddCollider(const std::shared_ptr<Collider> _collider)
{
	//コライダの追加
	colliders_.push_back(_collider);
}

void CollisionManager::Sweep(void)
{
	//終了したコライダを削除する
	std::erase_if(colliders_, [](const std::shared_ptr<Collider> _col) {return _col->IsDead(); });
}

void CollisionManager::Update(void)
{
	//コライダが一つもないなら処理を飛ばす
	if (colliders_.empty())return;

	//コライダの数
	const auto& colSize = colliders_.size();

	//当たり判定フレーム
	if (updateFrame_ < COL_UPDATE_FRAME)
	{
		//カウンタ
		updateFrame_++;
		return;
	}

	for (int i = 0; i < colSize - 1; i++)
	{
		//コライダが死亡済みなら
		if (colliders_[i]->IsDead())
		{
			//飛ばす
			continue;
		}

		for (int j = i + 1; j < colSize; j++)
		{
			//コライダが死亡済みなら
			if (colliders_[i]->IsDead() || colliders_[j]->IsDead())
			{
				//飛ばす
				continue;
			}

			//同一所持者をスキップ
			if (&colliders_[i]->GetOwner() == &colliders_[j]->GetOwner())
			{
				//飛ばす
				continue;
			}

			//事前当たり判定
			if (!IsBroudCollision(*colliders_[i], *colliders_[j]))
			{
				//飛ばす
				continue;
			}

			//当たり判定をするタグか
			if (!IsCollisionTag(*colliders_[i], *colliders_[j]))
			{
				//飛ばす
				continue;
			}

			//当たり判定
			if (IsCollision(*colliders_[i], *colliders_[j]))
			{
				//それぞれの当たった処理
				colliders_[i]->OnHit(colliders_[j]);
				colliders_[j]->OnHit(colliders_[i]);

				//当たった後の処理
				if (!colliders_[i]->IsDead())
				{
					colliders_[i]->GetGeometry().HitAfter();
				}
				if (!colliders_[j]->IsDead())
				{
					colliders_[j]->GetGeometry().HitAfter();
				}
			}
		}
	}
	//カウンタの初期化
	updateFrame_ = 0;
}

void CollisionManager::Draw(void)
{

	// 現在の全コライダー数を表示
	DrawFormatString(10, 130, 0xffffff, L"TotalColliders: %d", colliders_.size());

	int i = 0;
	for (auto& col : colliders_) {
		col->GetGeometry().Draw();
		if (col->GetTag() == Collider::TAG::ATTACK) i++;
		else if (col->GetTag() == Collider::TAG::NEUTRAL) i++;
		else if (col->GetTag() == Collider::TAG::HOSTILITY) i++;
	}
	DrawFormatString(10, 150, 0xffffff, L"AttackColl:%d", i);
}

void CollisionManager::Clear(void)
{
	//コライダの全削除
	colliders_.clear();
}

void CollisionManager::Destroy(void)
{
	//コライダの全削除
	colliders_.clear();

	//自身のインスタンス削除
	delete instance_;
	instance_ = nullptr;
}

bool CollisionManager::CheckCollision(const std::shared_ptr<Collider>& _col, const Collider::TAG _targetTag, const std::set<Collider::TAG> _notHitTags)
{
	for (const auto& col : colliders_)
	{
		if (col->GetTag() == _targetTag && !col->IsDead() && IsCollisionTag(*_col, *col) && IsCollision(*_col, *col))
		{
			return true;
		}
	}
	return false;
}

CollisionManager::CollisionManager(void)
	:
	updateFrame_(0)
{
	colliders_.clear();
}

// 当たり判定前の距離による判定
const bool CollisionManager::IsBroudCollision(const Collider& _col1, const Collider& _col2)
{
	//原点
	const VECTOR& origin = _col1.GetOwner().GetTransform().pos;

	//座標
	const VECTOR& pos1 = Utility::VECTOR_ZERO;
	const VECTOR& pos2 = VSub(_col2.GetOwner().GetTransform().pos,origin);
	//距離
	float range = _col1.GetGeometry().GetHitRadius() + _col2.GetGeometry().GetHitRadius();
	float sqrDistance = Utility::SqrMagnitudeF(VAdd(pos1, pos2));

	//判定
	return sqrDistance < range * range;
}

const bool CollisionManager::IsCollisionTag(const Collider& _col1, const Collider& _col2) const
{
	//タグ
	const auto& tag1 = _col1.GetTag();
	const auto& tag2 = _col2.GetTag();

	//当たり判定しないタグ
	const auto& notColTags1 = _col1.GetNotHitTags();
	const auto& notColTags2 = _col2.GetNotHitTags();

	//2人目の当たり判定しないタグ
	if (notColTags2.contains(tag1))
	{
		//1人目のタグが2人目の当たり判定しないタグと同一だった
		return false;
	}

	//1人目の当たり判定しないタグ
	if (notColTags1.contains(tag2))
	{
		//2人目のタグが1人目の当たり判定しないタグと同一だった
		return false;
	}

	//当たる
	return true;
}

const bool CollisionManager::IsCollision(const Collider& _col1, const Collider& _col2)const
{
	//1つ目の当たり判定形状
	auto& geo1 = _col1.GetGeometry();
	
	//2つ目の当たり判定形状
	auto& geo2 = _col2.GetGeometry();

	//当たり判定
	bool ret = geo1.IsHit(geo2);
	
	// 当たった後の処理
	if (ret)
	{
		// コライダのshared_ptrを取得
		std::weak_ptr<Collider> col1Ptr, col2Ptr;
		for (const auto& c : colliders_) {
			if (c.get() == &_col1) col1Ptr = c;
			if (c.get() == &_col2) col2Ptr = c;
		}
		// 当たった情報の設定
		if (!col1Ptr.expired()) {
			const_cast<Collider&>(_col1).SetHitCollider(col2Ptr);
		}
		if (!col2Ptr.expired()) {
			const_cast<Collider&>(_col2).SetHitCollider(col1Ptr);
		}
	}

	//当たり判定
	return ret;
}