#pragma once
#include "PlayerBase.h"

class SelectPlayer :
    public PlayerBase
{
public:

	// コンストラクタ
	SelectPlayer(void);
	// デストラクタ
	virtual ~SelectPlayer(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//当たり判定
	void OnHit(const std::weak_ptr<Collider> _hitCol) override;

private:

	// 回転の反映
	void UpdateRotate(void) override;

};

