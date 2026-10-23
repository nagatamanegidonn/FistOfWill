#pragma once
#include "AttackBase.h"

class MeleeAttack :
    public AttackBase
{
public:

	// コンストラクタ
	MeleeAttack(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data);
	// デストラクタ
	virtual ~MeleeAttack(void) {}

	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
};

