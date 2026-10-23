#pragma once

#include"Geometry.h"

class Line : public Geometry
{
public:

	/// @brief コンストラクタ
	/// @param _pos 追従する親の座標
	/// @param _rot 追従する親の回転
	/// @param _localPosPoint1 1つ目の点の座標
	/// @param _localPosPoint2 2つ目の点の座標
	Line(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR& _localPosPoint1, const VECTOR& _localPosPoint2);

	/// @brief コピーコンストラクタ
	/// @param _copyBase コピー元
	Line(const Line& _copyBase);

	// デストラクタ
	~Line(void)override;

	// 描画
	void Draw(const int _color = NORMAL_COLOR)override;

	//各種当たり判定
	const bool IsHit(Geometry& _geometry)override;
	const bool IsHit(Model& _model)override;
	const bool IsHit(Cube& _cube)override;
	const bool IsHit(Sphere& _sphere)override;
	const bool IsHit(Capsule& _capsule) override;
	const bool IsHit(Line& _line) override;

	//1つ目の点の座標を取得
	inline const VECTOR GetLocalPosPoint1(void)const { return localPosPoint1_; }

	//2つ目の点の座標を取得
	inline const VECTOR GetLocalPosPoint2(void)const { return localPosPoint2_; }

	// 回転済みの1つ目の点の座標を取得
	inline const VECTOR GetPosPoint1(void) const { return GetRotPos(localPosPoint1_); }

	// 回転済みの2つ目の点の座標を取得
	inline const VECTOR GetPosPoint2(void) const { return GetRotPos(localPosPoint2_); }
	
	// 回転済みの1つ目の点の移動後座標を取得
	inline const VECTOR GetMovedPosPoint1(void) const { return GetRotMovedPos(localPosPoint1_); }

	// 回転済みの2つ目の点の移動後座標を取得
	inline const VECTOR GetMovedPosPoint2(void) const { return GetRotMovedPos(localPosPoint2_); }

	//1つ目の点の座標の設定
	inline void SetLocalPosPoint1(const VECTOR _pos) { localPosPoint1_ = _pos; }

	//2つ目の点の座標の設定
	inline void SetLocalPosPoint2(const VECTOR _pos) { localPosPoint2_ = _pos; }

	//当たった時の情報取得
	inline const MV1_COLL_RESULT_POLY& GetHitInfo(void)const { return hitInfo_; }

	//当たった時の情報設定
	void SetHitInfo(const MV1_COLL_RESULT_POLY _hitInfo);

	// LocalPosPointの再設定
	void SetLocalPosPoints(const VECTOR _localPosPoint1, const VECTOR _localPosPoint2)
	{
		localPosPoint1_ = _localPosPoint1;
		localPosPoint2_ = _localPosPoint2;
	}

private:

	VECTOR localPosPoint1_;	//1つ目の点の座標
	VECTOR localPosPoint2_;	//2つ目の点の座標

	MV1_COLL_RESULT_POLY hitInfo_;	//当たった時の情報(モデル)
};