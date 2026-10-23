//#include"../pch.h"
#include"../../../../Common/Quaternion.h"
#include"../../../../Utility/Utility.h"
#include"Model.h"
#include"Sphere.h"
#include"Capsule.h"
#include"Line.h"
#include"Cube.h"

//***************************************************
//箱
//***************************************************

Cube::Cube(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR _min, const VECTOR _max)
	: Geometry(_pos, _movedPos, _rot)
{
	obb_.vMin = _min;
	obb_.vMax = _max;
	hitResult_ = {};

	UpdateObbAxis();
}

Cube::Cube(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR _halfSize)
	: Geometry(_pos, _movedPos, _rot)
{
	obb_.vMin = VScale(_halfSize, -1.0f);
	obb_.vMax = _halfSize;
	hitResult_ = {};

	UpdateObbAxis();
}

Cube::Cube(const Cube& _copyBase)
	: Geometry(_copyBase.GetColPos(), _copyBase.GetColMovedPos(), _copyBase.GetColRot()),
	obb_(_copyBase.GetObb())
{
	UpdateObbAxis();
}

Cube::~Cube(void)
{

}

void Cube::Draw(const int _color)
{
	
	VECTOR vertices[8];
	CalculateVertices(vertices);

	// 12本のエッジのインデックス
	static const int edges[12][2] = {
		{0,1},{0,2},{0,4}, {1,3},{1,5},
		{2,3},{2,6}, {3,7},
		{4,5},{4,6}, {5,7},{6,7}
	};

	for (int i = 0; i < 12; ++i)
	{
		DrawLine3D(vertices[edges[i][0]], vertices[edges[i][1]], _color);
	}

#ifdef _DEBUG
	//法線の描画
	VECTOR center = VAdd(pos_, VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f));
	for (int i = 0; i < 3; ++i)
	{
		VECTOR endPoint = VAdd(center, VScale(obb_.axis[i], 20.0f)); // 軸の方向に20ユニット伸ばす
		DrawLine3D(center, endPoint, 0xff0000); // 赤色で描画
	}

#endif // _DEBUG

}

const bool Cube::IsHit(Geometry& _geometry)
{
	//回転の更新
	UpdateObbAxis();

	//判定
	bool ret = _geometry.IsHit(*this);

	//結果
	return ret;
}

const bool Cube::IsHit(Model& _model)
{
	return false;
}

const bool Cube::IsHit(Cube& _cube)
{
	//結果
	HitResult cubeResultA;
	HitResult cubeResultB;

	float minOverlap = FLT_MAX;
	VECTOR bestAxis = { 0, 0, 0 }; // 最小オーバーラップ軸（法線になる）
	
	// 各OBBの中心座標（ワールド空間）
	VECTOR centerA = VAdd(pos_, VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f));
	VECTOR centerB = VAdd(_cube.GetColPos(), VScale(VAdd(_cube.GetObb().vMin, _cube.GetObb().vMax), 0.5f));

	// 2つの中心の差
	VECTOR t = VSub(centerB, centerA);

	// OBBの軸長（半サイズ）
	VECTOR halfA = VScale(VSub(obb_.vMax, obb_.vMin), 0.5f);
	VECTOR halfB = VScale(VSub(_cube.GetObb().vMax, _cube.GetObb().vMin), 0.5f);

	// 各軸を順にチェック（15軸）
	for (int i = 0; i < 3; ++i) {

		const VECTOR& axisA = obb_.axis[i];

		// 軸Aの投影量
		float ra = halfA.x * fabs(VDot(axisA, obb_.axis[0])) +
			halfA.y * fabs(VDot(axisA, obb_.axis[1])) +
			halfA.z * fabs(VDot(axisA, obb_.axis[2]));

		float rb = halfB.x * fabs(VDot(axisA, _cube.GetObb().axis[0])) +
			halfB.y * fabs(VDot(axisA, _cube.GetObb().axis[1])) +
			halfB.z * fabs(VDot(axisA, _cube.GetObb().axis[2]));

		if (fabs(VDot(t, axisA)) > ra + rb) return false;
		
		float dist = fabs(VDot(t, axisA));
		float overlap = ra + rb - dist;
		if (overlap < 0.0f) return false; // 分離軸あり → 衝突なし

		if (overlap < minOverlap) {
			minOverlap = overlap;
			bestAxis = axisA;
			// tとの向きが逆なら反転（OBB A → B に押し戻す方向にする）
			if (VDot(t, axisA) < 0) {
				bestAxis = VScale(axisA, -1.0f);
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		const VECTOR& axisB = _cube.GetObb().axis[i];

		float ra = halfA.x * fabs(VDot(axisB, obb_.axis[0])) +
			halfA.y * fabs(VDot(axisB, obb_.axis[1])) +
			halfA.z * fabs(VDot(axisB, obb_.axis[2]));

		float rb = halfB.x * fabs(VDot(axisB, _cube.GetObb().axis[0])) +
			halfB.y * fabs(VDot(axisB, _cube.GetObb().axis[1])) +
			halfB.z * fabs(VDot(axisB, _cube.GetObb().axis[2]));

		if (fabs(VDot(t, axisB)) > ra + rb) return false;

		float dist = fabs(VDot(t, axisB));
		float overlap = ra + rb - dist;
		if (overlap < 0.0f) return false;

		if (overlap < minOverlap) {
			minOverlap = overlap;
			bestAxis = axisB;
			if (VDot(t, axisB) < 0) {
				bestAxis = VScale(axisB, -1.0f);
			}
		}
	}

	// 外積軸
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			VECTOR axis = VCross(obb_.axis[i], _cube.GetObb().axis[j]);

			// 軸が0に近い（平行またはゼロベクトル） → 無視
			if (VSize(axis) < 0.0001f) continue;
			axis = VNorm(axis);

			float ra = halfA.x * fabs(VDot(axis, obb_.axis[0])) +
				halfA.y * fabs(VDot(axis, obb_.axis[1])) +
				halfA.z * fabs(VDot(axis, obb_.axis[2]));

			float rb = halfB.x * fabs(VDot(axis, _cube.GetObb().axis[0])) +
				halfB.y * fabs(VDot(axis, _cube.GetObb().axis[1])) +
				halfB.z * fabs(VDot(axis, _cube.GetObb().axis[2]));

			if (fabs(VDot(t, axis)) > ra + rb) return false;

			if (VSize(axis) < 0.0001f) continue;
			axis = VNorm(axis);

			float dist = fabs(VDot(t, axis));
			float overlap = ra + rb - dist;
			if (overlap < 0.0f) return false;

			if (overlap < minOverlap) {
				minOverlap = overlap;
				bestAxis = axis;
				if (VDot(t, axis) < 0) {
					bestAxis = VScale(axis, -1.0f);
				}
			}
		}
	}

	//法線方向
	cubeResultA.normal = bestAxis;
	cubeResultB.normal = VScale(bestAxis, -1.0f);

	//深度
	cubeResultA.depth = minOverlap;
	cubeResultB.depth = minOverlap;

	// CubeA側の衝突点の計算
	float ra =
		halfA.x * fabs(VDot(bestAxis, obb_.axis[0])) +
		halfA.y * fabs(VDot(bestAxis, obb_.axis[1])) +
		halfA.z * fabs(VDot(bestAxis, obb_.axis[2]));

	// 衝突点 (A の表面)
	cubeResultA.point = VAdd(centerA, VScale(bestAxis, ra));

	// B側の衝突点
	float rb =
		halfB.x * fabs(VDot(bestAxis, _cube.GetObb().axis[0])) +
		halfB.y * fabs(VDot(bestAxis, _cube.GetObb().axis[1])) +
		halfB.z * fabs(VDot(bestAxis, _cube.GetObb().axis[2]));
	VECTOR pointB = VSub(centerB, VScale(bestAxis, rb));
	cubeResultB.point = pointB;

	//保存
	hitResult_ = cubeResultA;
	_cube.SetHitResult(cubeResultB);

	// すべての軸で重なっている → 衝突
	return true;
}

const bool Cube::IsHit(Sphere& _sphere)
{
	return _sphere.IsHit(*this);
}

const bool Cube::IsHit(Capsule& _capsule)
{
	HitResult result;

	// --- カプセルのワールド頂点 ---
	VECTOR topW = _capsule.GetPosTop();   // ワールド座標
	VECTOR downW = _capsule.GetPosDown();  // ワールド座標
	float radius = _capsule.GetRadius();

	// --- OBB のワールド中心 ---
	VECTOR obbCenter = VAdd(
		pos_,
		VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f)
	);

	// --- カプセル端点を OBB ローカル空間に ---
	VECTOR rel1 = VSub(topW, obbCenter);
	VECTOR rel2 = VSub(downW, obbCenter);

	VECTOR local1 = {
		VDot(rel1, obb_.axis[0]),
		VDot(rel1, obb_.axis[1]),
		VDot(rel1, obb_.axis[2])
	};

	VECTOR local2 = {
		VDot(rel2, obb_.axis[0]),
		VDot(rel2, obb_.axis[1]),
		VDot(rel2, obb_.axis[2])
	};

	// --- OBB を AABB として扱い最近接点を求める ---
	VECTOR segDir = VSub(local1, local2);

	// セグメント → AABB 最近接点（AABB 側）
	// AABBに最も近い点
	VECTOR pOnAABB = ClosestPointAABB(local2, obb_.vMin, obb_.vMax);

	// セグメント側の最近接点の t
	float t = ClosestPointT(pOnAABB, local2, local1);
	VECTOR pOnSeg = VAdd(local2, VScale(segDir, t));

	VECTOR diff = VSub(pOnAABB, pOnSeg);
	float dist2 = VSquareSize(diff);

	if (dist2 > radius * radius)
		return false; // 衝突していない

	float dist = sqrtf(dist2);
	float depth = radius - dist;

	// --- ローカル法線 ---
	VECTOR normalLocal;
	if (dist < 1e-6f) {
		// めり込みすぎ → どこかの軸を法線に採用
		normalLocal = { 1, 0, 0 };
	}
	else {
		normalLocal = VScale(diff, 1.0f / dist);
	}

	// --- ワールド法線に変換 ---
	VECTOR normalW = VAdd(
		VAdd(
			VScale(obb_.axis[0], normalLocal.x),
			VScale(obb_.axis[1], normalLocal.y)
		),
		VScale(obb_.axis[2], normalLocal.z)
	);
	normalW = VNorm(normalW);

	// --- ワールド座標で衝突点を復元 ---
	VECTOR pointW = VAdd(
		VAdd(
			VScale(obb_.axis[0], pOnAABB.x),
			VScale(obb_.axis[1], pOnAABB.y)
		),
		VScale(obb_.axis[2], pOnAABB.z)
	);
	pointW = VAdd(pointW, obbCenter);

	// --- 結果 ---
	result.normal = normalW;
	result.point = pointW;
	result.depth = depth;
	result.t = t;

	//結果の保存
	hitResult_ = result;

	//相手側も
	result.normal = VScale(result.normal, -1.0f);
	_capsule.SetHitResult(result);

	return true;
}

const bool Cube::IsHit(Line& _line)
{
	// ワールド空間の線分座標
	VECTOR p1 = _line.GetPosPoint1();
	VECTOR p2 = _line.GetPosPoint2();

	// OBB のローカル中心（Min/Maxの中点）
	VECTOR localCenter = VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f);

	// OBB のワールド中心を計算：axisで回転して pos_ を加算
	VECTOR worldCenter = VAdd(
		VAdd(
			VAdd(
				VScale(obb_.axis[0], localCenter.x),
				VScale(obb_.axis[1], localCenter.y)
			),
			VScale(obb_.axis[2], localCenter.z)
		),
		pos_
	);

	// 線分をOBB空間に変換（ワールド中心 → ローカル）
	VECTOR rel1 = VSub(p1, worldCenter);
	VECTOR rel2 = VSub(p2, worldCenter);

	VECTOR local1 = {
		VDot(rel1, obb_.axis[0]),
		VDot(rel1, obb_.axis[1]),
		VDot(rel1, obb_.axis[2])
	};

	VECTOR local2 = {
		VDot(rel2, obb_.axis[0]),
		VDot(rel2, obb_.axis[1]),
		VDot(rel2, obb_.axis[2])
	};

	// スラブ法：線分が AABB（Min/Max）と交差するか
	VECTOR dir = VSub(local2, local1);
	float tmin = -FLT_MAX, tmax = FLT_MAX; // tminを負の最大値で初期化

	int hitAxis = -1;
	float hitSide = 1.0f;

	for (int axis = 0; axis < 3; ++axis)
	{
		float start, delta, minB, maxB;
		if (axis == 0) { start = local1.x; delta = dir.x; minB = obb_.vMin.x; maxB = obb_.vMax.x; }
		else if (axis == 1) { start = local1.y; delta = dir.y; minB = obb_.vMin.y; maxB = obb_.vMax.y; }
		else { start = local1.z; delta = dir.z; minB = obb_.vMin.z; maxB = obb_.vMax.z; }

		if (fabs(delta) < 1e-6) {
			// 線分がその軸に平行な場合、範囲外なら即終了
			if (start < minB || start > maxB) return false;
		}
		else {
			float ood = 1.0f / delta;
			float t1 = (minB - start) * ood;
			float t2 = (maxB - start) * ood;
			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tmin) {
				tmin = t1;
				hitAxis = axis;
				hitSide = (delta > 0.0f) ? -1.0f : 1.0f;
			}
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
		}
	}

	// 線分の範囲(0～1)の外で交差しているなら当たっていない
	if (tmin > 1.0f || tmax < 0.0f) return false;

	// --- 結果の構築 ---
	VECTOR normalLocal = { 0, 0, 0 };
	float depth = 0.0f;

	// 線分の先端(local2)がCubeの中にある場合（めり込み）
	bool isInside = (local2.x >= obb_.vMin.x && local2.x <= obb_.vMax.x &&
		local2.y >= obb_.vMin.y && local2.y <= obb_.vMax.y &&
		local2.z >= obb_.vMin.z && local2.z <= obb_.vMax.z);

	if (isInside) {
		// 最も近い面を探して押し戻しベクトルを作る
		float minDist = FLT_MAX;
		float dists[6] = {
			local2.x - obb_.vMin.x, obb_.vMax.x - local2.x,
			local2.y - obb_.vMin.y, obb_.vMax.y - local2.y,
			local2.z - obb_.vMin.z, obb_.vMax.z - local2.z
		};
		VECTOR normals[6] = {
			{-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1}
		};
		for (int i = 0; i < 6; ++i) {
			if (dists[i] < minDist) {
				minDist = dists[i];
				normalLocal = normals[i];
				depth = dists[i];
			}
		}
	}
	else if (tmin > 0.0f && tmin <= 1.0f) { // ★修正：判定範囲を厳密に
		// 線分が外から刺さった場合
		if (hitAxis == 0) normalLocal.x = hitSide;
		else if (hitAxis == 1) normalLocal.y = hitSide;
		else if (hitAxis == 2) normalLocal.z = hitSide;

		// ★ここが重要：坂道での沈み込みを防ぐ正確な深度計算
		// 単なる線分の余りではなく、法線方向に対してどれだけ深く刺さっているかを計算する
		VECTOR intersectVec = VScale(dir, (1.0f - tmin));
		depth = fabs(VDot(intersectVec, normalLocal));
	}
	else {
		return false;
	}

	// ワールド空間への変換 (ここからは元の通り)
	VECTOR normalWorld = VNorm(VTransformSR(normalLocal, quaRot_.ToMatrix()));

	// ヒット情報の保存
	hitResult_.normal = normalWorld;
	hitResult_.depth = depth;
	// 衝突点は、線分の交点をワールド空間に変換して求める
	hitResult_.point = VAdd(VTransformSR(VAdd(local1, VScale(dir, std::max(0.0f, tmin))), quaRot_.ToMatrix()), worldCenter);
	hitResult_.t = std::max(0.0f, tmin);

	// 相手側（Line）
	HitResult lineResult = hitResult_;
	lineResult.normal = VScale(normalWorld, -1.0f);
	_line.SetHitResult(lineResult);

	return true;
}

void Cube::HitAfter(void)
{
	//当たった情報の初期化
	hitResult_.normal = Utility::VECTOR_ZERO;
}

void Cube::SetHalfSize(const VECTOR& _halfSize)
{
	obb_.vMin = VScale(_halfSize, -1.0f);
	obb_.vMax = _halfSize;
}

void Cube::UpdateObbAxis(void) 
{
	MATRIX rotMat;
	rotMat = quaRot_.ToMatrix();

	obb_.axis[0] = VTransform(VGet(1, 0, 0), rotMat); // Right
	obb_.axis[1] = VTransform(VGet(0, 1, 0), rotMat); // Up
	obb_.axis[2] = VTransform(VGet(0, 0, 1), rotMat); // Forward
}

void Cube::CalculateVertices(VECTOR outVertices[8]) const
{
	MATRIX rotMat;
	rotMat = quaRot_.ToMatrix();

	int idx = 0;
	for (int x = 0; x <= 1; ++x)
	{
		for (int y = 0; y <= 1; ++y)
		{
			for (int z = 0; z <= 1; ++z)
			{
				VECTOR local;
				local.x = (x == 0) ? obb_.vMin.x : obb_.vMax.x;
				local.y = (y == 0) ? obb_.vMin.y : obb_.vMax.y;
				local.z = (z == 0) ? obb_.vMin.z : obb_.vMax.z;

				VECTOR world = VTransform(local, rotMat);
				world = VAdd(world, pos_);

				outVertices[idx++] = world;
			}
		}
	}
}

VECTOR Cube::ClosestPointOnAABB(const VECTOR& point, const VECTOR& aabbMin, const VECTOR& aabbMax) {
	return {
		std::clamp(point.x, aabbMin.x, aabbMax.x),
		std::clamp(point.y, aabbMin.y, aabbMax.y),
		std::clamp(point.z, aabbMin.z, aabbMax.z)
	};
}

VECTOR Cube::ClosestPointOnSegmentToAABB(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& aabbMin, const VECTOR& aabbMax) {
	VECTOR ab = VSub(segEnd, segStart);
	float tMin = 0.0f;
	float tMax = 1.0f;

	// スラブごとの処理をしてもよいが、最小化問題としてtをスキャンでも可
	const int STEPS = 10;
	float minDistSq = FLT_MAX;
	VECTOR closest = segStart;

	for (int i = 0; i <= STEPS; ++i) {
		float t = static_cast<float>(i) / STEPS;
		VECTOR point = VAdd(segStart, VScale(ab, t));
		VECTOR clamped = ClosestPointOnAABB(point, aabbMin, aabbMax);
		VECTOR diff = VSub(clamped, point);
		float distSq = VDot(diff, diff);

		if (distSq < minDistSq) {
			minDistSq = distSq;
			closest = point;
		}
	}

	return closest;
}

// AABB上の最近接点を返す（線分に対して）
VECTOR Cube::GetClosestPointOnAABBToSegment(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& aabbMin, const VECTOR& aabbMax) {
	// 線分上の最近接点をAABB内にクランプ
	VECTOR ab = VSub(segEnd, segStart);
	float t = 0.0f;
	float tMin = 0.0f;
	float tMax = 1.0f;

	const int STEPS = 10; // 精度を上げたいなら増やす
	float minDistSq = FLT_MAX;
	VECTOR bestPoint = segStart;

	for (int i = 0; i <= STEPS; ++i) {
		t = static_cast<float>(i) / STEPS;
		VECTOR point = VAdd(segStart, VScale(ab, t)); // 線分上の点
		VECTOR clamped = {
			std::clamp(point.x, aabbMin.x, aabbMax.x),
			std::clamp(point.y, aabbMin.y, aabbMax.y),
			std::clamp(point.z, aabbMin.z, aabbMax.z)
		};

		float distSq = VDot(VSub(clamped, point), VSub(clamped, point));
		if (distSq < minDistSq) {
			minDistSq = distSq;
			bestPoint = clamped;
		}
	}

	return bestPoint;
}

// カプセル線分上の最近接点（射影）を返す
VECTOR Cube::CapsuleSegmentClosestPoint(const VECTOR& segStart, const VECTOR& segEnd, const VECTOR& targetPoint) {
	VECTOR ab = VSub(segEnd, segStart);
	VECTOR ap = VSub(targetPoint, segStart);

	float abLenSq = VDot(ab, ab);
	if (abLenSq == 0.0f) return segStart; // 線分が点の場合

	float t = VDot(ap, ab) / abLenSq;
	t = std::clamp(t, 0.0f, 1.0f); // 線分内にクランプ

	return VAdd(segStart, VScale(ab, t));
}


// 線分上で最近接点の t を求める
float Cube::ClosestPointT(const VECTOR& p, const VECTOR& a, const VECTOR& b)
{
	VECTOR ab = VSub(b, a);
	float ab2 = VSquareSize(ab);
	if (ab2 < 1e-6f) return 0.0f;
	return std::clamp(VDot(VSub(p, a), ab) / ab2, 0.0f, 1.0f);
}

// AABB 最近接点
VECTOR Cube::ClosestPointAABB(const VECTOR& p, const VECTOR& vMin, const VECTOR& vMax)
{
	VECTOR r;
	r.x = std::clamp(p.x, vMin.x, vMax.x);
	r.y = std::clamp(p.y, vMin.y, vMax.y);
	r.z = std::clamp(p.z, vMin.z, vMax.z);
	return r;
}