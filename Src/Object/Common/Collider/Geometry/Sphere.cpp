//#include"../pch.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Utility/Utility.h"
#include"Model.h"
#include"Cube.h"
#include"Capsule.h"
#include"Line.h"
#include"Sphere.h"
#include <algorithm>

//***************************************************
//球
//***************************************************

Sphere::Sphere(const VECTOR& _pos, const VECTOR& _movedPos, const float _radius)
	: Geometry(_pos, _movedPos, Quaternion()),
	radius_(_radius)
{
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Sphere::Sphere(const Sphere& _copyBase)
	: Geometry(_copyBase.GetColPos(),_copyBase.GetColMovedPos(), Quaternion())
{
	radius_ = _copyBase.GetRadius();
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Sphere::~Sphere(void)
{
}

void Sphere::Draw(const int _color)
{
	DrawSphere3D(pos_, radius_, 10, _color, _color, false);
}

const bool Sphere::IsHit(Geometry& _geometry)
{
	return _geometry.IsHit(*this);
}

const bool Sphere::IsHit(Model& _model)
{
	return _model.IsHit(*this);
}

const bool Sphere::IsHit(Cube& _cube)
{
	// --- OBB情報 ---
	const auto& obb = _cube.GetObb();
	VECTOR center = VAdd(_cube.GetColPos(),
		VScale(VAdd(obb.vMin, obb.vMax), 0.5f)); // OBB中心
	VECTOR half = VScale(VSub(obb.vMax, obb.vMin), 0.5f); // 半サイズ

	// --- ローカル空間へ変換 ---
	VECTOR relPos = VSub(pos_, center);

	// Sphere の移動
	VECTOR sphereMoveVec = VSub(movedPos_, pos_);

	// OBB の移動
	VECTOR cubeMoveVec = VSub(_cube.GetColMovedPos(), _cube.GetColPos());

	// 相対速度にする
	VECTOR velocity = VSub(sphereMoveVec, cubeMoveVec);	

	//ローカル変換
	VECTOR localP0 = VGet(VDot(relPos, obb.axis[0]),
		VDot(relPos, obb.axis[1]),
		VDot(relPos, obb.axis[2]));
	VECTOR localV = VGet(VDot(velocity, obb.axis[0]),
		VDot(velocity, obb.axis[1]),
		VDot(velocity, obb.axis[2]));

	// --- AABBスイープ ---
	VECTOR boxMin = VScale(half, -1.0f);
	VECTOR boxMax = half;
	VECTOR bmin = VSub(boxMin, VGet(radius_, radius_, radius_));
	VECTOR bmax = VAdd(boxMax, VGet(radius_, radius_, radius_));

	//スラブ法で衝突しているかを判定
	float tEnter = 0.0f;
	float tExit = 1.0f;

	for (int i = 0; i < 3; i++)
	{
		float start = (&localP0.x)[i];
		float dir = (&localV.x)[i];
		float minB = (&bmin.x)[i];
		float maxB = (&bmax.x)[i];

		if (fabs(dir) < 1e-6f)
		{
			if (start < minB || start > maxB)
				return false; // 外れている → 衝突なし
		}
		else
		{
			float t1 = (minB - start) / dir;
			float t2 = (maxB - start) / dir;
			if (t1 > t2) std::swap(t1, t2);

			tEnter = std::max(tEnter, t1);
			tExit = std::min(tExit, t2);

			if (tEnter > tExit)
				return false;
		}
	}

	// --- 衝突あり ---

	//衝突した瞬間を格納
	hitResult_.t = std::max(0.0f, tEnter);
	
	//Cube側も格納しておく
	HitResult cubeResult;
	cubeResult.t = hitResult_.t;

	VECTOR localHitPos = VAdd(localP0, VScale(localV, hitResult_.t));

	// --- 衝突法線 ---
	VECTOR localNormal = VGet(0, 0, 0);
	float eps = 1e-4f;
	for (int i = 0; i < 3; i++)
	{
		if (fabs((&localHitPos.x)[i] - (&bmin.x)[i]) < eps)
			(&localNormal.x)[i] = -1.0f;
		else if (fabs((&localHitPos.x)[i] - (&bmax.x)[i]) < eps)
			(&localNormal.x)[i] = 1.0f;
	}
	
	float dxMin = fabs(localHitPos.x - bmin.x);
	float dxMax = fabs(localHitPos.x - bmax.x);
	float dyMin = fabs(localHitPos.y - bmin.y);
	float dyMax = fabs(localHitPos.y - bmax.y);
	float dzMin = fabs(localHitPos.z - bmin.z);
	float dzMax = fabs(localHitPos.z - bmax.z);

	// 最小距離の軸が衝突面
	float minDist = dxMin;
	localNormal = VGet(-1, 0, 0);

	if (dxMax < minDist) { minDist = dxMax; localNormal = VGet(1, 0, 0); }
	if (dyMin < minDist) { minDist = dyMin; localNormal = VGet(0, -1, 0); }
	if (dyMax < minDist) { minDist = dyMax; localNormal = VGet(0, 1, 0); }
	if (dzMin < minDist) { minDist = dzMin; localNormal = VGet(0, 0, -1); }
	if (dzMax < minDist) { minDist = dzMax; localNormal = VGet(0, 0, 1); }

	//深度
	float depth = 0.0f;

	if (localNormal.x > 0) depth = bmax.x - localHitPos.x;
	else if (localNormal.x < 0) depth = localHitPos.x - bmin.x;
	else if (localNormal.y > 0) depth = bmax.y - localHitPos.y;
	else if (localNormal.y < 0) depth = localHitPos.y - bmin.y;
	else if (localNormal.z > 0) depth = bmax.z - localHitPos.z;
	else if (localNormal.z < 0) depth = localHitPos.z - bmin.z;

	// --- ワールド座標に戻す ---
	VECTOR hitWorld = center;
	hitWorld = VAdd(hitWorld,
		VAdd(VScale(obb.axis[0], localHitPos.x),
			VAdd(VScale(obb.axis[1], localHitPos.y),
				VScale(obb.axis[2], localHitPos.z))));

	VECTOR normalWorld = VGet(
		localNormal.x * obb.axis[0].x + localNormal.y * obb.axis[1].x + localNormal.z * obb.axis[2].x,
		localNormal.x * obb.axis[0].y + localNormal.y * obb.axis[1].y + localNormal.z * obb.axis[2].y,
		localNormal.x * obb.axis[0].z + localNormal.y * obb.axis[1].z + localNormal.z * obb.axis[2].z
	);
	normalWorld = VNorm(normalWorld);

	//情報の格納
	hitResult_.depth = depth;
	hitResult_.point = hitWorld;
	hitResult_.normal = normalWorld;

	//Cube側も渡す
	cubeResult.depth = depth;
	cubeResult.point = hitWorld;
	cubeResult.normal = VScale(normalWorld, -1.0f);

	return true;
}

const bool Sphere::IsHit(Sphere& _sphere)
{
	//結果
	HitResult result;

	VECTOR mySpherePos = pos_;
	VECTOR partnerSpherePos = _sphere.GetColPos();

	float mySphereRadius = GetRadius();
	float partnerSphereRadius = _sphere.GetRadius();

	VECTOR diff = VSub(partnerSpherePos, mySpherePos);
	float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
	float sumRadius = mySphereRadius + partnerSphereRadius;

	// 衝突していない
	if (distSq >= sumRadius * sumRadius)
	{
		return false;
	}

	result.t = 0.0f; // 静止物体の場合は必ず 0

	float dist = sqrtf(distSq);

	if (dist == 0.0f)
	{
		// 完全に同位置 → 適当な法線を与える
		result.normal = { 1,0,0 };
		result.depth = sumRadius;
		result.point = mySpherePos;
	}
	else
	{
		// 法線
		result.normal = VScale(diff, 1.0f / dist);

		// 深度
		result.depth = sumRadius - dist;

		// 接触点
		result.point = VAdd(mySpherePos, VScale(result.normal, mySphereRadius));
	}

	//保存
	hitResult_ = result;

	//相手側も保存
	result.normal = VScale(result.normal, -1.0f);
	_sphere.SetHitResult(result);

	return true;
}

const bool Sphere::IsHit(Capsule& _capsule)
{
	//結果
	HitResult result;

	//各要素
	VECTOR spherePos = pos_;
	VECTOR sphereMovedPos = movedPos_;
	VECTOR capTopPos = _capsule.GetPosTop();
	VECTOR capTopMovedPos = _capsule.GetMovedPosTop();
	VECTOR capDownPos = _capsule.GetPosDown();
	VECTOR capDownMovedPos = _capsule.GetMovedPosDown();

	// 相対運動
	VECTOR moveVecSphere = VSub(movedPos_, pos_);
	VECTOR moveVecCapsule = VSub(_capsule.GetColMovedPos(), _capsule.GetColPos()); // カプセル全体の移動量
	VECTOR vec = VSub(moveVecSphere, moveVecCapsule);  // Sphereだけが動くように見える相対運動

	// 衝突半径
	float sumRadius = radius_ + _capsule.GetRadius();

	// ----
	// 時間 t を0〜1の間で二次方程式として解く
	// 距離^2(t) = rSum^2 になる t を求める
	// ----
	float bestT = 1.0f;
	bool found = false;

	// t を細かくサーチして初期解を取る（安定化のため）
	const int STEPS = 64;
	for (int i = 0; i <= STEPS; i++)
	{
		float t = (float)i / STEPS;

		// Capsule positions at time t
		VECTOR A = {
			capTopPos.x + (capTopMovedPos.x - capTopPos.x) * t,
			capTopPos.y + (capTopMovedPos.y - capTopPos.y) * t,
			capTopPos.z + (capTopMovedPos.z - capTopPos.z) * t
		};
		VECTOR B = {
			capDownPos.x + (capDownMovedPos.x - capDownPos.x) * t,
			capDownPos.y + (capDownMovedPos.y - capDownPos.y) * t,
			capDownPos.z + (capDownMovedPos.z - capDownPos.z) * t
		};

		// Sphere center at time t
		VECTOR p = {
			spherePos.x + vec.x * t,
			spherePos.y + vec.y * t,
			spherePos.z + vec.z * t
		};

		// Find closest point on segment AB to p
		VECTOR AB = VSub(B, A);
		VECTOR AP = VSub(p, A);
		float abLenSq = VDot(AB, AB);

		float u = 0.0f;
		if (abLenSq > 1e-6f)
			u = VDot(AP, AB) / abLenSq;

		u = std::clamp(u, 0.0f, 1.0f);

		VECTOR closest = {
			A.x + AB.x * u,
			A.y + AB.y * u,
			A.z + AB.z * u
		};

		VECTOR d = VSub(p, closest);
		float distSq = VDot(d, d);

		if (distSq <= sumRadius * sumRadius)
		{
			bestT = t;
			found = true;
			break;
		}
	}

	if (!found)
		return false; // No hit

	// -----------------------------
	// 確定した衝突時刻 t = bestT での情報を計算する
	// -----------------------------
	result.t = bestT;

	// Positions at collision time
	float t = bestT;

	VECTOR A = {
		capTopPos.x + (capTopMovedPos.x - capTopPos.x) * t,
		capTopPos.y + (capTopMovedPos.y - capTopPos.y) * t,
		capTopPos.z + (capTopMovedPos.z - capTopPos.z) * t
	};
	VECTOR B = {
		capDownPos.x + (capDownMovedPos.x - capDownPos.x) * t,
		capDownPos.y + (capDownMovedPos.y - capDownPos.y) * t,
		capDownPos.z + (capDownMovedPos.z - capDownPos.z) * t
	};
	VECTOR p = {
		spherePos.x + vec.x * t,
		spherePos.y + vec.y * t,
		spherePos.z + vec.z * t
	};

	// Closest point again
	VECTOR AB = VSub(B, A);
	VECTOR AP = VSub(p, A);
	float abLenSq = VDot(AB, AB);
	float u = (abLenSq > 1e-6f) ? VDot(AP, AB) / abLenSq : 0.0f;
	u = std::clamp(u, 0.0f, 1.0f);

	VECTOR closest = {
		A.x + AB.x * u,
		A.y + AB.y * u,
		A.z + AB.z * u
	};

	// Normal
	VECTOR diff = VSub(p, closest);
	float dist = sqrtf(VDot(diff, diff));

	if (dist > 1e-6f)
		result.normal = VScale(diff, 1.0f / dist);
	else
		result.normal = { 1,0,0 }; // Degenerate fallback

	// Depth (penetration amount AFTER collision)
	result.depth = sumRadius - dist;

	// Contact point (Sphere side)
	result.point = VSub(p, VScale(result.normal, radius_));

	//結果の保存
	hitResult_ = result;

	//相手側も保存
	result.normal = VScale(result.normal, -1.0f);
	_capsule.SetHitResult(result);

	return true;
}

const bool Sphere::IsHit(Line& _line)
{
	HitResult result;

	//両者の要素
	VECTOR spherePos = pos_;
	VECTOR sphereMovedPos = movedPos_;
	VECTOR linePosPoint1 = _line.GetPosPoint1();
	VECTOR lineMovedPosPoint1 = _line.GetMovedPosPoint1();
	VECTOR linePosPoint2 = _line.GetPosPoint2();
	VECTOR lineMovedPosPoint2 = _line.GetMovedPosPoint2();

	//--------------------------------
	// Step 1: 相対運動へ変換
	//--------------------------------
	VECTOR sphereMoveVec = VSub(sphereMovedPos, spherePos);   // sphere 移動
	VECTOR lineMoveVec = VSub(lineMovedPosPoint1, linePosPoint1); // line 移動量（線分全体）
	VECTOR vec = VSub(sphereMoveVec, lineMoveVec);   // 相対速度

	//--------------------------------
	// Step 2: t を 0〜1 で探す
	// 球中心が線分に近づき r 以下になる瞬間
	//--------------------------------
	float bestT = 1.0f;
	bool found = false;

	const int STEPS = 64; // 安定化用サンプリング
	for (int i = 0; i <= STEPS; i++)
	{
		float t = (float)i / STEPS;

		VECTOR L1 = {
			linePosPoint1.x + (lineMovedPosPoint1.x - linePosPoint1.x) * t,
			linePosPoint1.y + (lineMovedPosPoint1.y - linePosPoint1.y) * t,
			linePosPoint1.z + (lineMovedPosPoint1.z - linePosPoint1.z) * t
		};

		VECTOR L2 = {
			linePosPoint2.x + (lineMovedPosPoint2.x - linePosPoint2.x) * t,
			linePosPoint2.y + (lineMovedPosPoint2.y - linePosPoint2.y) * t,
			linePosPoint2.z + (lineMovedPosPoint2.z - linePosPoint2.z) * t
		};

		VECTOR p = {
			spherePos.x + vec.x * t,
			spherePos.y + vec.y * t,
			spherePos.z + vec.z * t
		};

		VECTOR d = VSub(L2, L1);
		VECTOR m = VSub(p, L1);

		float dd = VSquareSize(d);
		float u = (dd > 1e-6f) ? VDot(m, d) / dd : 0.0f;
		u = std::clamp(u, 0.0f, 1.0f);

		VECTOR closest = VAdd(L1, VScale(d, u));
		VECTOR diff = VSub(closest, p);

		float distSq = VSquareSize(diff);

		if (distSq <= radius_ * radius_)
		{
			bestT = t;
			found = true;
			break;
		}
	}

	if (!found)
		return false; // 衝突なし

	//--------------------------------
	// Step 3: 衝突時点の情報を計算
	//--------------------------------
	result.t = bestT;

	float t = bestT;

	// 衝突時の線分位置
	VECTOR L1 = {
		linePosPoint1.x + (lineMovedPosPoint1.x - linePosPoint1.x) * t,
		linePosPoint1.y + (lineMovedPosPoint1.y - linePosPoint1.y) * t,
		linePosPoint1.z + (lineMovedPosPoint1.z - linePosPoint1.z) * t
	};

	VECTOR L2 = {
		linePosPoint2.x + (lineMovedPosPoint2.x - linePosPoint2.x) * t,
		linePosPoint2.y + (lineMovedPosPoint2.y - linePosPoint2.y) * t,
		linePosPoint2.z + (lineMovedPosPoint2.z - linePosPoint2.z) * t
	};

	// 衝突時の球中心（相対）
	VECTOR p = {
		spherePos.x + vec.x * t,
		spherePos.y + vec.y * t,
		spherePos.z + vec.z * t
	};

	VECTOR d = VSub(L2, L1);
	VECTOR m = VSub(p, L1);

	float dd = VSquareSize(d);
	float u = (dd > 1e-6f) ? VDot(m, d) / dd : 0.0f;
	u = std::clamp(u, 0.0f, 1.0f);

	VECTOR closest = VAdd(L1, VScale(d, u));
	VECTOR diff = VSub(closest, p);
	float dist = sqrtf(VDot(diff, diff));

	// normal
	if (dist > 1e-6f)
		result.normal = VScale(diff, 1.0f / dist);
	else
		result.normal = VGet(1, 0, 0);

	// 衝突時点の depth は 0（すれ違い）
	result.depth = radius_ - dist;

	// 線分側の接触点
	result.point = closest;

	//結果の保存
	hitResult_ = result;

	//相手側も保存
	result.normal = VScale(result.normal, -1.0f);
	_line.SetHitResult(result);

	return true;
}

void Sphere::HitAfter(void)
{
	if (hitInfo_.HitNum > 0 && hitInfo_.Dim != nullptr)
	{
		//当たり判定情報の解放
		MV1CollResultPolyDimTerminate(hitInfo_);

		//再初期化
		std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	}
}
