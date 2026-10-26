#pragma once
#include "AttackBase.h"

class ForwardAttack :
    public AttackBase
{
public:

	// コンストラクタ
	ForwardAttack(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data);
	// デストラクタ
	virtual ~ForwardAttack(void) {}

	// 初期化処理
	void Init() override;

	// 更新処理
	void Update() override;

};

