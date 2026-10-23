
#pragma once
#include "OnHitBase.h"

class EnemyBase;

class TutorialEnemyOnHit :
	public OnHitBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="owner">所有者</param>
	TutorialEnemyOnHit(EnemyBase& owner);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TutorialEnemyOnHit() override;

private:

	// 所有者
	EnemyBase& owner_;

	// タグ別衝突後処理
	void OnHitStage(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitWall(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitAttack(const std::weak_ptr<Collider>& hitColWeak);

	// 既存の OnHitStage を整理するための関数
	void ProcessLineHit(Line& line, Geometry& opponentGeo);
	void ProcessCapsuleHit(Capsule& capsule, Geometry& opponentGeo);

	bool isAgoReaction_ = false; // 反応済みかどうかのフラグ
};