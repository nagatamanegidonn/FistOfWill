
#pragma once
#include <deque>
#include <memory>

#include "ComponentBase.h"

class AttackData;
class SwordTrailData;
class SwordTrailMaterial;
class SwordTrailRenderer;

// 軌跡描画コンポーネント
class DrawTrailComponent :
	public ComponentBase
{
public:

	// コンストラクタ
	DrawTrailComponent(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~DrawTrailComponent(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void PostUpdate(void) override;
	virtual void Draw(void) override;

	void SetAttackData(std::shared_ptr<AttackData> attackData) { attackData_ = attackData; }

protected:

	std::weak_ptr<AttackData> attackData_;   // CSVから読み込んだこの攻撃のパラメータ

	// --- 軌跡用に追加 ---
	std::unique_ptr<SwordTrailData> swordTrail_;
	std::unique_ptr<SwordTrailMaterial> trailMaterial_;
	std::unique_ptr<SwordTrailRenderer> trailRenderer_;
};
