#pragma once

#include<DxLib.h>
#include"../../../../Common/Quaternion.h"

class Model;
class Cube;
class Sphere;
class Capsule;
class Line;

class Geometry
{
public:

	//ヒット情報
	struct HitResult
	{
		float t = 1.0f;        // 0～1：衝突した割合（移動途中の位置）
		VECTOR point = { 0,0,0 };
		VECTOR normal = { 0,0,0 };
		float depth = 0.0f;   
	};

	//デストラクタ
	virtual~Geometry(void) = 0;

	//描画
	virtual void Draw(const int _color = NORMAL_COLOR) = 0;

	//各種当たり判定
	virtual const bool IsHit(Geometry& _geometry) = 0;
	virtual const bool IsHit(Model& _model) = 0;
	virtual const bool IsHit(Cube& _cube) = 0;
	virtual const bool IsHit(Sphere& _sphere) = 0;
	virtual const bool IsHit(Capsule& _capsule) = 0;
	virtual const bool IsHit(Line& _line) = 0;
	
	//ヒット後の処理
	virtual void HitAfter(void);

	//親の座標を返す
	inline const VECTOR& GetColPos(void)const { return pos_; }

	//親の移動後座標を返す
	inline const VECTOR& GetColMovedPos(void)const { return movedPos_; }
	
	//親の回転を返す
	inline const Quaternion& GetColRot(void)const { return quaRot_; }

	//当たった法線方向の取得
	inline const VECTOR GetHitNormal(void)const { return hitResult_.normal; }

	//当たった法線方向の設定
	inline void SetHitNormal(const VECTOR _hitNormal) { hitResult_.normal = _hitNormal; }

	//当たった情報の取得
	const HitResult& GetHitResult(void)const { return hitResult_; }

	//当たった情報の設定
	void SetHitResult(const HitResult _hitResult) { hitResult_ = _hitResult; }

	// 相対座標を回転させてワールド座標で取得する
	const VECTOR GetRotPos(const VECTOR& _localPos) const;
	const VECTOR GetRotMovedPos(const VECTOR& _localPos) const;
	
	// 相対座標を逆回転させてワールド座標で取得する
	const VECTOR GetInvRotPos(const VECTOR& _localPos) const;

	// 半径の取得
	virtual const float GetHitRadius(void) const { return hitRadius_; }
	// 半径の設定
	virtual void SetHitRadius(const float _hitRadius) { hitRadius_ = _hitRadius; }

protected:

	//通常色
	static constexpr int NORMAL_COLOR = 0xffffff;

	//親情報
	const VECTOR& pos_;			//親の座標
	const VECTOR& movedPos_;	//親の移動後座標
	const Quaternion& quaRot_;	//親の回転

	//当たった情報
	HitResult hitResult_;

	//判定を行う距離半径
	float hitRadius_ = 10000.0f;

	/// @brief コンストラクタ(外部で作る必要のない基底なのでprotected)
	/// @param _pos 追従する親の座標
	/// @param _rot 追従する親の回転
	Geometry(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot);
};