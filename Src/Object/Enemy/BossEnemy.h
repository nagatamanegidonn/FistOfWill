#pragma once
#include "EnemyBase.h"

class BossEnemy :
    public EnemyBase
{
public:

	const VECTOR LINE_UP = { 0.0f, 50.0f, 0.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN = { 0.0f, 0.0f, 0.0f };//下方向のローカル座標

	const VECTOR LINE_UP2 = { 0.0f, 50.0f, 25.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN2 = { 0.0f, 0.0f, 25.0f };	//下方向のローカル座標

	const VECTOR LINE_UP3 = { 0.0f, 50.0f, -25.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN3 = { 0.0f, 0.0f, -25.0f };	//下方向のローカル座標

	const VECTOR CUPSULE_UP = { 0.0f, 60.0f + 100.0f, 0.0f };
	const VECTOR CUPSULE_DOWN = { 0.0f, 60.0f + 10.0f, 0.0f };
	const float CUPSULE_RAD = 60.0f;

	// コンストラクタ
	BossEnemy(int _key);
	// デストラクタ
	virtual ~BossEnemy(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	void OnHit(const std::weak_ptr<Collider> _hitCol)override;

private:

	VECTOR hipsPos_;	//ヒップの位置
};

