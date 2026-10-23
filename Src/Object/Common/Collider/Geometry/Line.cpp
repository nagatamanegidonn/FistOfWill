//#include"../pch.h"
#include"../Utility/Utility.h"
#include "../../../../Common/Quaternion.h"
#include"Model.h"
#include"Cube.h"
#include"Sphere.h"
#include"Capsule.h"
#include"Line.h"

//***************************************************
//線
//***************************************************

Line::Line(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const VECTOR& _localPosPoint1, const VECTOR& _localPosPoint2)
	: Geometry(_pos, _movedPos, _rot),
	localPosPoint1_(_localPosPoint1),
	localPosPoint2_(_localPosPoint2)
{
	hitInfo_ = {};
	hitResult_ = {};
}

Line::Line(const Line& _copyBase)
	: Geometry(_copyBase.GetColPos(), _copyBase.GetColMovedPos(), _copyBase.GetColRot())
{
	localPosPoint1_ = _copyBase.GetLocalPosPoint1();
	localPosPoint2_ = _copyBase.GetLocalPosPoint2();
	hitInfo_ = {};
	hitResult_ = {};
}

Line::~Line(void)
{
}

void Line::Draw(const int _color)
{
	VECTOR point1 = GetRotPos(localPosPoint1_);
	VECTOR point2 = GetRotPos(localPosPoint2_);

	DrawLine3D(point1, point2, _color);
}

const bool Line::IsHit(Geometry& _geometry)
{
	return _geometry.IsHit(*this);
}

const bool Line::IsHit(Model& _model)
{
	return _model.IsHit(*this);
}

const bool Line::IsHit(Cube& _cube)
{
	return _cube.IsHit(*this);
}

const bool Line::IsHit(Sphere& _sphere)
{
	return _sphere.IsHit(*this);
}

const bool Line::IsHit(Capsule& _capsule)
{
	return _capsule.IsHit(*this);
}

const bool Line::IsHit(Line& _line)
{
    HitResult result1;
    HitResult result2;

    const float EPSILON = 1e-6f;

    VECTOR u = VSub(GetPosPoint2(), GetPosPoint1());
    VECTOR v = VSub(_line.GetPosPoint2(), _line.GetPosPoint1());
    VECTOR w = VSub(GetPosPoint1(), _line.GetPosPoint1());

    float a = VDot(u, u);
    float b = VDot(u, v);
    float c = VDot(v, v);
    float d = VDot(u, w);
    float e = VDot(v, w);

    float denom = a * c - b * b;
    float s = 0.0f, t = 0.0f;

    // --- s を計算 ---
    if (fabs(denom) > EPSILON)
        s = std::clamp((b * e - c * d) / denom, 0.0f, 1.0f);

    // --- t を計算 ---
    t = (b * s + e) / c;
    t = std::clamp(t, 0.0f, 1.0f);

    // --- s の再計算 ---
    s = (b * t - d) / a;
    s = std::clamp(s, 0.0f, 1.0f);

    // --- 最近接点 ---
    VECTOR p1 = VAdd(GetPosPoint1(), VScale(u, s));
    VECTOR p2 = VAdd(_line.GetPosPoint1(), VScale(v, t));

    VECTOR diff = VSub(p1, p2);
    float distance = sqrtf(VDot(diff, diff));

    // ヒット判定
    if (distance > EPSILON)
        return false; //当たっていない

    // --- ここから衝突情報作成 ---
    result1.point = p1;      // 線分同士は接触点 p1 ≒ p2
    result2.point = p2;

    result1.t = t;
    result2.t = t;

    // normal
    if (distance > EPSILON)
        result1.normal = VScale(diff, 1.0f / distance);
    else
        result1.normal = { 1,0,0 }; // ほぼ重なり時

    result2.normal = VScale(result1.normal, -1.0f);

    // depth（線分同士なので基本 0）
    result1.depth = distance; // EPSILON 以内なら 0 に等しい
    result2.depth = distance;

    //当たった
    return true;
}

void Line::SetHitInfo(const MV1_COLL_RESULT_POLY _hitInfo)
{
    hitInfo_ = _hitInfo;

    if (hitInfo_.HitFlag)
    {
        // DxLibの衝突点
        hitResult_.point = hitInfo_.HitPosition;

        // DxLibの衝突ポリゴンの法線
        hitResult_.normal = hitInfo_.Normal;

        // 【重要】めり込み深さ(depth)を計算する
        // 判定用ラインの始点(Point1)から衝突点までのベクトルを計算
        VECTOR startToHit = VSub(hitInfo_.HitPosition, GetPosPoint1());

        // ラインの方向ベクトル（始点から終点）
        VECTOR lineDir = VSub(GetPosPoint2(), GetPosPoint1());
        float lineLength = VSize(lineDir);

        if (lineLength > 1e-5f)
        {
            // 始点から衝突点までの距離
            float distToHit = VSize(startToHit);
            // めり込み深さ = ラインの総延長 - 始点から衝突点までの距離
            hitResult_.depth = lineLength - distToHit;
        }
        else
        {
            hitResult_.depth = 0.0f;
        }
    }
}
