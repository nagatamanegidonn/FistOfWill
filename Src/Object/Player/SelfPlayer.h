#pragma once
#include "PlayerBase.h"

class Cube;
class Collider;

class SelfPlayer :
    public PlayerBase
{
public:

	const VECTOR LINE_UP = { 0.0f, 50.0f, 0.0f };		//上方向のローカル座標
	const VECTOR LINE_DOWN = { 0.0f, 0.0f, 0.0f };		//下方向のローカル座標

	const VECTOR LINE_UP2 = { 0.0f, 50.0f, 20.0f };		//上方向のローカル座標
	const VECTOR LINE_DOWN2 = { 0.0f, 0.0f, 20.0f };	//下方向のローカル座標

	const VECTOR LINE_UP3 = { 0.0f, 50.0f, -20.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN3 = { 0.0f, 0.0f, -20.0f };	//下方向のローカル座標

	const VECTOR LINE_UP4= { 20.0f, 50.0f, 0.0f };		//上方向のローカル座標
	const VECTOR LINE_DOWN4 = { 20.0f, 0.0f, 0.0f };	//下方向のローカル座標

	const VECTOR LINE_UP5 = { -20.0f, 50.0f, 0.0f };	//上方向のローカル座標
	const VECTOR LINE_DOWN5 = { -20.0f, 0.0f, 0.0f };	//下方向のローカル座標

	// コンストラクタ
	SelfPlayer(int _key);
	// デストラクタ
	virtual ~SelfPlayer(void);

	void Init(void) override;
	void Update(void) override;
	void UpdateDebugImGui(void);
	void Draw(void) override;

	//当たり判定
	void OnHit(const std::weak_ptr<Collider> _hitCol) override;

private:

	VECTOR hipsPos_;	//ヒップの位置

};

