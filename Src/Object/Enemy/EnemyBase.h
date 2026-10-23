#pragma once
#include <string>
#include "../CharaBase.h"

class StateBase;
class PlayerBase;

class EnemyBase :
    public CharaBase
{
public:

	// コンストラクタ
	EnemyBase(int key);
	// デストラクタ
	virtual ~EnemyBase(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	// 攻撃関係
	//攻撃クールタイム取得
	float GetAttackCoolTime() const { return attackCoolTime_; }
	//攻撃クールタイム設定
	void SetAttackCoolTime(float coolTime) { attackCoolTime_ = coolTime; }

	//攻撃可能
	bool IsEnableAttack() const;

	// 周囲のプレイヤーから最適なターゲットを探して更新する関数
	void UpdateTarget() override;

	// 現在の状態
	std::weak_ptr<StateBase> GetState(void) const { return state_; }

	// 攻撃可能距離の取得
	const float GetAttackRange(void) const { return attackRange_; }

protected:

	// 攻撃関係
	float attackCoolTime_;	// 攻撃クールタイム
	bool canAttack_;		// 攻撃権

	// ターゲット回転
	float rotateTimer_ = 0.0f;          // 回転間隔のためのタイマー
	const float rotateInterval_ = 1.6f; // 例：0.2秒ごとに向き直す

	// 攻撃距離
	float attackRange_; // 攻撃可能距離

	// 視野判定 関係
	bool IsTargetInFOV(const VECTOR& followPos, float fovDeg);
	void DrawFOV(float fovDeg, float radius, int rayCount, unsigned int color);

};

