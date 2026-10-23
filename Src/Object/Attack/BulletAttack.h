#pragma once
#include "AttackBase.h"

class BulletAttack : 
	public AttackBase
{
public:

	// コンストラクタ
	BulletAttack(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data);
	// デストラクタ
	virtual ~BulletAttack() {}

	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
};