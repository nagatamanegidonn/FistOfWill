#pragma once
#include "../ActorBase.h"

class Destination :
    public ActorBase
{
public:

	// コンストラクタ
	Destination(std::shared_ptr<ActorBase> owner);

	// デストラクタ
	virtual ~Destination(void);

	// 初期化処理
	virtual void Init(void) override;

	// 更新処理
	virtual void Update(void) override;

	// 描画処理
	virtual void Draw(void) override;

	// 持ち主の取得
	std::weak_ptr<ActorBase> GetOwner(void) { return owner_; }

	// ゴールしたかどうかの取得
	bool IsGoal(void) const { return isGoal_; }
	void SetGoal(bool goal) { isGoal_ = goal; }

protected:

	std::weak_ptr<ActorBase> owner_;           // 攻撃の発生源（プレイヤーや敵など）
	bool isGoal_;                              // ゴールしたかどうかのフラグ

	virtual void OnHit(const std::weak_ptr<Collider> _hitCol) override;

};

