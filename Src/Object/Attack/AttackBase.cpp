#include "AttackBase.h"
#include "AttackData.h"

AttackBase::AttackBase(std::shared_ptr<ActorBase> owner, std::shared_ptr<AttackData> data)
	: 
	ActorBase(),
	attackDamage_(0),
	owner_(owner), 
	attackData_(data), 
	lifeTimer_(0.0f)
{
	attackDamage_ = attackData_->attackPower;		// CSV‚©‚çUŒ‚—Í‚ğæ“¾
	m_attackWeight = attackData_->m_attackWeight;	// CSV‚©‚çUŒ‚‚Ìd‚³‚ğæ“¾
}

AttackBase::~AttackBase(void)
{
}

void AttackBase::Init(void)
{
}

void AttackBase::Update(void)
{
}

void AttackBase::Draw(void)
{
}
