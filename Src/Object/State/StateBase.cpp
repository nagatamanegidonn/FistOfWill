#include "StateBase.h"

StateBase::StateBase(std::weak_ptr<ActorBase> _actor)
	:
	owner_(_actor),
	m_State(STATE::IDLE)
{
}

void StateBase::Init()
{
}

void StateBase::Update()
{
}

void StateBase::Exit()
{
}

void StateBase::ChangeState(std::shared_ptr<StateBase> nextState)
{
	// ‰ğ•úˆ—
	Exit();

	//ó‘Ô•Ï‰»
	m_nextState = move(nextState);
}
