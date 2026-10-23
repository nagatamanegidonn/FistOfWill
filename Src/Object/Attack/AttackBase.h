#pragma once
#include "../ActorBase.h"
#include "../CharaStatus.h"
#include <memory>

class AttackData;

class AttackBase :
	public ActorBase
{
public:

	// コンストラクタ
	AttackBase(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data);
	// デストラクタ
	virtual ~AttackBase(void);

	// 初期化処理
	virtual void Init(void) override;
	// 更新処理
	virtual void Update(void) override;
	// 描画処理
	virtual void Draw(void) override;

	// 攻撃力
	int GetAttackDamage(void) const { return attackDamage_; }

	// 攻撃の重さ
	CharaStatus::AttackWeight GetAttackWeight(void) const { return m_attackWeight; }

	// 持ち主の取得
	std::weak_ptr<ActorBase> GetOwner(void) { return owner_; }

protected:

	int attackDamage_;                      // 攻撃力

	//攻撃の重さ
	CharaStatus::AttackWeight m_attackWeight;

	std::weak_ptr<ActorBase> owner_;        // 攻撃の発生源（プレイヤーや敵など）
	std::shared_ptr<AttackData> attackData_;// CSVから読み込んだこの攻撃のパラメータ

	float lifeTimer_;                       // 遠隔弾などの持続時間管理用
};

