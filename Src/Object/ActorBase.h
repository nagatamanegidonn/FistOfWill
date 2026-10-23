#pragma once
#include <memory>
#include <vector>
#include "Common/Collider/Collider.h"
#include "Common/Transform.h"
#include "../Utility/Utility.h"

class ResourceManager;
class SceneManager;
class Geometry;
class OnHitBase;

class ComponentBase;

static struct InitParameter
{
	VECTOR scl = Utility::VECTOR_ONE;
	VECTOR pos = Utility::VECTOR_ZERO;
	VECTOR rot = Utility::VECTOR_ZERO;
	VECTOR lrot = Utility::VECTOR_ZERO;
	int modelHandle = -1;
};

class ActorBase  
{
public:

	// コンストラクタ
	ActorBase(void);

	// デストラクタ
	virtual ~ActorBase(void);

	// 初期化処理
	virtual void Init(void) = 0;
	void InitParam(const InitParameter& param);

	// 更新処理
	virtual void Update(void) = 0;
	
	// 描画処理
	virtual void Draw(void) = 0;
	virtual void DrawShadow(void);

	// 削除フラグ
	bool IsDelete() const { return isDelete_; };

	// 削除予約
	void Delete() { isDelete_ = true; };

	//当たり判定の削除
	void Sweep(void);

	/// @brief それぞれの当たり判定後の処理
	/// @param _hitCol 相手側の当たり判定タグ
	virtual void OnHit(const std::weak_ptr<Collider> _hitCol) {};

	const Transform& GetTransform(void) const;
	void SetPosition(const VECTOR& pos) { transform_.pos = pos; }

	//当たり判定前半径の取得
	const float GetBroudRadius(void) const { return broudRadius_; }

	//コライダの取得
	const std::vector<std::shared_ptr<Collider>>& GetColliders(void) { return collider_; };

	// 移動系の処理
	void AddMove(const VECTOR& moveVec) {
		transform_.pos = VAdd(transform_.pos, moveVec);
		transform_.Update();
	}

	// コンポーネントの取得
	std::vector<std::shared_ptr<ComponentBase>>& GetComponents(void) { return component_; }

	/// @brief 当たり判定作成(形状情報作成後)
	/// @param _tag 自身の当たり判定タグ
	/// @param _geometry 自身の形状情報
	/// @param _notHitTags 衝突させないタグ
	void MakeCollider(const Collider::TAG _tag, std::unique_ptr<Geometry> _geometry, const std::set<Collider::TAG> _notHitTags = {});

	//該当タグで当たり判定削除
	void DeleteColliderAtTag(Collider::TAG _tag);

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

	//当たり判定関係
	std::vector<std::shared_ptr<Collider>> collider_;	//全体の当たり判定情報
	std::shared_ptr<OnHitBase> onHit_;					//当たり判定後の処理

	// コンポーネント群
	std::vector<std::shared_ptr<ComponentBase>> component_;
	
	//消滅
	bool isDelete_;

	//当たり判定前用半径
	float broudRadius_;

	//全当たり判定の消去
	void DeleteAllCollider(void);
};
