#pragma once

#include"Geometry.h"

class Cube : public Geometry
{
public:

	//バウンディングボックス
	struct OBB
	{
		VECTOR vMin;
		VECTOR vMax;
		VECTOR axis[3];
	};

	/// @brief コンストラクタ
	/// @param _pos 追従する親の座標
	/// @param _rot 追従する親の回転
	/// @param _min 親から見た、箱の最小地点
	/// @param _max 親から見た、箱の最大地点
	Cube(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR _min, const VECTOR _max);
	
	/// @brief コンストラクタ
	/// @param _pos 追従する親の座標
	/// @param _rot 追従する親の回転
	/// @param _halfSize 箱の半分サイズ
	Cube(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR _halfSize);

	/// @brief コピーコンストラクタ
	/// @param _copyBase コピー元
	Cube(const Cube& _copyBase);

	//デストラクタ
	~Cube(void)override;

	//描画
	void Draw(const int _color = NORMAL_COLOR)override;

	//各種当たり判定
	const bool IsHit(Geometry& _geometry)override;
	const bool IsHit(Model& _model)override;
	const bool IsHit(Cube& _cube)override;
	const bool IsHit(Sphere& _sphere)override;
	const bool IsHit(Capsule& _capsule)override;
	const bool IsHit(Line& _line)override;

	//ヒット後処理
	void HitAfter(void)override;

	//回転バウンティボックスの取得
	inline const OBB& GetObb(void)const { return obb_; }

	//箱の最小地点の取得
	inline const VECTOR GetVecMin(void)const { return obb_.vMin; }

	//箱の最大地点の取得
	inline const VECTOR GetVecMax(void)const { return obb_.vMax; }

	//回転バウンティボックスの設定
	inline void SetObb(const OBB& _obb) { obb_ = _obb; }

	//箱の最小地点の設定
	inline void SetVecMin(const VECTOR& _min) { obb_.vMin = _min; }

	//箱の最大地点の設定
	inline void SetVecMax(const VECTOR& _max){ obb_.vMax = _max; }

	//サイズの半分の設定
	inline void SetHalfSize(const VECTOR& _halfSize);

private:

	//回転バウンディングボックス
	OBB obb_;

	//箱の回転情報の取得
	inline const VECTOR GetAxis(const int _num)const { return obb_.axis[_num]; }

	// クォータニオンから回転軸を計算
	void UpdateObbAxis(void);

	// 各頂点の計算（ワールド座標）
	void CalculateVertices(VECTOR outVertices[8]) const;

	// AABB 上で最近接点を求める（線分の点との最短点）
	VECTOR ClosestPointOnAABB(const VECTOR& point, const VECTOR& aabbMin, const VECTOR& aabbMax);

	// 線分上で、AABBにもっとも近い点を求める（補助関数）
	VECTOR ClosestPointOnSegmentToAABB(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& aabbMin, const VECTOR& aabbMax);
		
	// AABB上の最近接点を返す（線分に対して）
	VECTOR GetClosestPointOnAABBToSegment(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& aabbMin, const VECTOR& aabbMax);
	
	// カプセル線分上の最近接点（射影）を返す
	VECTOR CapsuleSegmentClosestPoint(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& targetPoint);

	// 線分上で最近接点の t を求める
	float ClosestPointT(const VECTOR& p, const VECTOR& a, const VECTOR& b);
	
	// AABB 最近接点
	VECTOR ClosestPointAABB(const VECTOR& p, const VECTOR& vMin, const VECTOR& vMax);
};