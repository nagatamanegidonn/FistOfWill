#pragma once

#include"Geometry.h"

class Sphere : public Geometry
{
public:

	/// @brief コンストラクタ
	/// @param _pos 追従する親の座標
	/// @param _radius 半径
	Sphere(const VECTOR& _pos, const VECTOR& _movedPos, const  float _radius);

	/// @brief コピーコンストラクタ
	/// @param _copyBase コピー元
	Sphere(const Sphere& _copyBase);

	//デストラクタ
	~Sphere(void)override;

	//描画
	void Draw(const int _color = NORMAL_COLOR)override;

	//各種当たり判定
	const bool IsHit(Geometry& _geometry)override;
	const bool IsHit(Model& _model)override;
	const bool IsHit(Cube& _cube)override;
	const bool IsHit(Sphere& _sphere)override;
	const bool IsHit(Capsule& _capsule)override;
	const bool IsHit(Line& _line)override;

	//ヒット後の処理
	void HitAfter(void)override;

	//半径の取得
	inline const float GetRadius(void)const { return radius_; }

	//半径の設定
	inline void SetRadius(const float _radius) { radius_ = _radius; }

	//当たった時の情報取得
	inline const MV1_COLL_RESULT_POLY_DIM& GetHitInfo(void)const { return hitInfo_; }

	//当たった時の情報設定
	inline void SetHitInfo(MV1_COLL_RESULT_POLY_DIM _hitInfo) { std::swap(hitInfo_, _hitInfo); }

private:

	float radius_;						//半径
	MV1_COLL_RESULT_POLY_DIM hitInfo_;	//当たった時の情報(モデル)
};