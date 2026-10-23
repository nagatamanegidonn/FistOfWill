//#include"../pch.h"
#include "../../../../Common/Quaternion.h"
#include "Geometry.h"

//***************************************************
//Šî–{
//***************************************************

Geometry::~Geometry(void)
{
}

Geometry::Geometry(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot)
	: pos_(_pos), 
	movedPos_(_movedPos),
	quaRot_(_rot)
{
	hitResult_ = {};
}

const VECTOR Geometry::GetRotPos(const VECTOR& _localPos) const
{
	VECTOR localRotPos = quaRot_.PosAxis(_localPos);
	return VAdd(pos_, localRotPos);
}

const VECTOR Geometry::GetRotMovedPos(const VECTOR& _localPos) const
{
	VECTOR localRotPos = quaRot_.PosAxis(_localPos);
	return VAdd(movedPos_, localRotPos);
}

const VECTOR Geometry::GetInvRotPos(const VECTOR& _localPos) const
{
	VECTOR localRotPos = quaRot_.Inverse().PosAxis(_localPos);
	return VAdd(pos_, localRotPos);
}

void Geometry::HitAfter(void)
{
}
