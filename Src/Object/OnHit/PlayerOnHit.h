#pragma once
#include "OnHitBase.h"

class PlayerBase;

class PlayerOnHit : public OnHitBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="owner">所有者</param>
	PlayerOnHit(PlayerBase& owner);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerOnHit() override;

private:

	// 所有者
	PlayerBase& owner_;

	// タグ別衝突後処理
	void OnHitGround(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitWall(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitCharacter(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitJustEvasion(const std::weak_ptr<Collider>& hitColWeak);
	void OnHitAttack(const std::weak_ptr<Collider>& hitColWeak);

	// 既存の OnHitStage を整理するための関数
	void ProcessLineHit(Line& line, Geometry& opponentGeo);
	void ProcessCapsuleHit(Capsule& capsule, Geometry& opponentGeo);

};

