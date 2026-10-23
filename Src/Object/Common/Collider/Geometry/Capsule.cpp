//#include"../../pch.h"
#include"../Utility/Utility.h"
#include "../Common/Quaternion.h"
#include"Model.h"
#include"Cube.h"
#include"Sphere.h"
#include"Line.h"
#include"Capsule.h"
#include <algorithm>

//***************************************************
//カプセル
//***************************************************

Capsule::Capsule(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR _localPosTop, const VECTOR _localPosDown, const float _radius)
	: Geometry(_pos, _movedPos, _rot),
	localPosTop_(_localPosTop),
	localPosDown_(_localPosDown),
	radius_(_radius)
{
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	hitResult_ = {};
}

Capsule::Capsule(const Capsule& _copyBase)
	: Geometry(_copyBase.GetColPos(), _copyBase.GetColMovedPos(), _copyBase.GetColRot())
{
	radius_ = _copyBase.GetRadius();
	localPosTop_ = _copyBase.GetLocalPosTop();
	localPosDown_ = _copyBase.GetLocalPosDown();
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	hitResult_ = {};
}

Capsule::~Capsule(void)
{
}

void Capsule::Draw(const int _color)
{
	// 上の球体
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, 5, _color, _color, false);

	// 下の球体
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, 5, _color, _color, false);

	VECTOR dir;
	VECTOR s;
	VECTOR e;

	// 球体を繋ぐ線(X+)
	dir = quaRot_.PosAxis(Utility::DIR_R);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, _color);

	// 球体を繋ぐ線(X-)
	dir = quaRot_.PosAxis(Utility::DIR_L);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, _color);

	// 球体を繋ぐ線(Z+)
	dir = quaRot_.PosAxis(Utility::DIR_F);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, _color);

	// 球体を繋ぐ線(Z-)
	dir = quaRot_.PosAxis(Utility::DIR_B);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, _color);

	// カプセルの中心
	DrawSphere3D(GetCenter(), 5.0f, 10, _color, _color, true);
}

const bool Capsule::IsHit(Geometry& _geometry)
{
	bool ret = _geometry.IsHit(*this);

	return ret;
}

const bool Capsule::IsHit(Model& _model)
{
	bool ret = _model.IsHit(*this);

	return ret;
}

const bool Capsule::IsHit(Cube& _cube)
{
	return _cube.IsHit(*this);
}

const bool Capsule::IsHit(Sphere& _sphere)
{
	bool ret = _sphere.IsHit(*this);

	return ret;
}

const bool Capsule::IsHit(Capsule& _capsule)
{
	HitResult hitResult;

	VECTOR d1 = VSub(GetPosDown(), GetPosTop());					// 線分1の方向ベクトル
	VECTOR d2 = VSub(_capsule.GetPosDown(), _capsule.GetPosTop());	// 線分2の方向ベクトル
	VECTOR r = VSub(GetPosTop(), _capsule.GetPosTop());

	float a = VDot(d1, d1); // d1・d1
	float e = VDot(d2, d2); // d2・d2
	float f = VDot(d2, r);

	float s, t;

	float c = VDot(d1, r);
	float b = VDot(d1, d2);
	float denom = a * e - b * b;

	if (denom != 0.0f)
	{
		s = (b * f - c * e) / denom;
		s = std::clamp(s, 0.0f, 1.0f);
	}
	else
	{
		s = 0.0f;
	}

	t = (b * s + f) / e;
	if (t < 0.0f)
	{
		t = 0.0f;
		s = std::clamp(-c / a, 0.0f, 1.0f);
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
		s = std::clamp((b - c) / a, 0.0f, 1.0f);
	}

	//衝突したか
	VECTOR c1 = VAdd(GetPosTop(), VScale(d1, s));
	VECTOR c2 = VAdd(_capsule.GetPosTop(), VScale(d2, t));
	VECTOR diff = VSub(c1, c2);
	float distance = VSize(diff);

	// 判定
	bool isHit = distance <= (GetRadius() + _capsule.GetRadius());

	if (isHit)
	{
		// normal（衝突法線：カプセル2 → 1）
		VECTOR normal;
		if (distance > 1e-6f)
			normal = VScale(diff, 1.0f / distance);
		else
			normal = { 0.0f, 1.0f, 0.0f }; // 完全一致したら適当な軸を返す

		// depth（めり込み量）
		float depth = GetRadius() + _capsule.GetRadius() - distance;
		depth = std::max(depth, 0.0f);

		// 衝突点（point）
		float r1 = GetRadius();
		float r2 = _capsule.GetRadius();

		// どちらの中心にも寄りすぎない中間点
		VECTOR point = VAdd(
			c2,
			VScale(normal, r2 - depth * 0.5f)
		);

		//結果
		hitResult.normal = normal;
		hitResult.depth = depth;
		hitResult.point = point;
		hitResult.t = t;

		//保存
		hitResult_ = hitResult;

		//相手側も
		hitResult.normal = VScale(hitResult.normal, -1.0f);
		_capsule.SetHitResult(hitResult);
	}

	return isHit;
}

const bool Capsule::IsHit(Line& _line)
{
	HitResult result;

	// --- ベクトル準備 ---
	VECTOR u = VSub(_line.GetPosPoint2(), _line.GetPosPoint1());      // 線分1
	VECTOR v = VSub(GetPosDown(), GetPosTop());						  // カプセル側の線分
	VECTOR w = VSub(_line.GetPosPoint1(), GetPosTop());

	float a = VDot(u, u);
	float b = VDot(u, v);
	float c = VDot(v, v);
	float d = VDot(u, w);
	float e = VDot(v, w);

	float denom = a * c - b * b;
	float s = 0.0f, t = 0.0f;

	// --- s の計算 ---
	if (denom != 0.0f) {
		s = std::clamp((b * e - c * d) / denom, 0.0f, 1.0f);
	}

	// --- t の計算 ---
	t = (b * s + e) / c;
	t = std::clamp(t, 0.0f, 1.0f);

	// --- s の再計算（相互依存のため） ---
	s = (b * t - d) / a;
	s = std::clamp(s, 0.0f, 1.0f);

	// --- 最近接点を求める ---
	VECTOR closest1 = VAdd(_line.GetPosPoint1(), VScale(u, s));     // 直線側
	VECTOR closest2 = VAdd(GetPosTop(), VScale(v, t));				// カプセル軸側

	VECTOR diff = VSub(closest1, closest2);
	float distance = sqrtf(VDot(diff, diff));

	float radius = radius_;

	// --- 判定 ---
	if (distance > radius) {
		return false;   // 当たっていない
	}

	// --- 以下：衝突情報の計算 ---

	// 法線
	VECTOR normal;
	if (distance > 1e-6f)
		normal = VScale(diff, 1.0f / distance);
	else
		normal = { 1,0,0 }; // 重なりすぎて方向が無いので暫定

	// 深度
	float depth = radius - distance;

	// 衝突点（line と capsule の間の接触点）
	VECTOR point = VAdd(closest2, VScale(normal, radius - depth * 0.5f));

	// 結果
	result.normal = normal;
	result.point = point;
	result.depth = depth;
	result.t = t;

	//保存
	hitResult_ = result;

	//相手側も
	result.normal = VScale(result.normal, -1.0f);
	_line.SetHitResult(result);

	return true;
}

void Capsule::HitAfter(void)
{
	if (hitInfo_.HitNum > 0 && hitInfo_.Dim != nullptr)
	{
		//当たり判定情報の解放
		MV1CollResultPolyDimTerminate(hitInfo_);

		//再初期化
		std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	}
}

const VECTOR Capsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();

	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, 0.5f));
}