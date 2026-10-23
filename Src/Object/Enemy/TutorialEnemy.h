#pragma once
#include "EnemyBase.h"

class Destination;

class TutorialEnemy :
	public EnemyBase
{
public:

	const VECTOR LINE_UP = { 0.0f, 50.0f, 0.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN = { 0.0f, 0.0f, 0.0f };	//下方向のローカル座標

	// コンストラクタ
	TutorialEnemy(int _key);
	// デストラクタ
	virtual ~TutorialEnemy(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	void OnHit(const std::weak_ptr<Collider> _hitCol)override;

private:

	std::shared_ptr<Destination> destination_;	//ゴール地点

};

