#pragma once
#include "StateBase.h"

class ActorBase;

class CharacterStateBase abstract :
    public StateBase
{
public:

	// コンストラクタ
	CharacterStateBase(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~CharacterStateBase() = default;

private:
protected:
};

