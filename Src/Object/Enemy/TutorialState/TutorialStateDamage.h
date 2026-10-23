#pragma once
#include"../../State/CharacterStateBase.h"

class TutorialStateDamage :
    public CharacterStateBase,
    public std::enable_shared_from_this<TutorialStateDamage>
{
public:

    // コンストラクタ
    TutorialStateDamage(std::weak_ptr<ActorBase> _actor);
    // デスストラクタ
    ~TutorialStateDamage(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};