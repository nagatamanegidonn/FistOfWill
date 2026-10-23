#include "AttackData.h"

AttackData::AttackData()
	:
	animationId(L""),
	attackPower(0),
	startTime(0),
	cancelableTime(0.0f),
	frameName(L"mixamorig:RightHand"),
	colliderRadius(20.0f),
	forwardSpeed(1.0f),
	nextAttackId(-1),
	attackType(AttackType::Melee),
	m_attackWeight(CharaStatus::AttackWeight::Light),
	m_armor(CharaStatus::AttackWeight::Light),
	colliderTag(Collider::TAG::HOSTILITY)
{
}

AttackData::~AttackData()
{
}
