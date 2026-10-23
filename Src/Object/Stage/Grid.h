#pragma once

#include "../Common/Transform.h"
#include "../ActorBase.h"

class SpaceDome;

class Grid :
	public ActorBase
{

public:

#pragma region デバッグ用定数

	// 線の長さ
	static constexpr float LEN = 1200.0f;

	// 線の長さの半分
	static constexpr float HLEN = LEN / 2.0f;

	// 線の間隔
	static constexpr float TERM = 50.0f;

	// 線の数
	static const int NUM = static_cast<int>(LEN / TERM);

	// 線の数の半分
	static const int HNUM = NUM / 2;

#pragma endregion

	//大きさ
	//static constexpr float BACKGROUND_SCALE = 10.0f;
	static constexpr float BACKGROUND_SCALE = 7.5f;

	//Y座標初期値
	//static constexpr float BACKGROUND_POS_Y = -1000.0f;
	static constexpr float BACKGROUND_POS_Y = -100.0f;

	//回転X
	static constexpr float BACKGROUND_ANGLE_X = 180.0f;

	//回転Y
	static constexpr float BACKGROUND_ANGLE_Y = 90.0f;

	//すてーじの半径
	static constexpr float STAGE_RADIUS = 800.0f;

	// コンストラクタ
	Grid(void);

	// デストラクタ
	~Grid(void);

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void DrawDebug(void);
	void Release(void);

	//当たり判定
	void OnHit(const std::weak_ptr<Collider> _hitCol)override;

private:

	SpaceDome* spaceDome_;

};