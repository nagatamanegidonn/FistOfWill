#pragma once
#include"../../State/CharacterStateBase.h"

class TutorialStateIdle :
    public CharacterStateBase,
    public std::enable_shared_from_this<TutorialStateIdle>
{
public:
    
    // コンストラクタ
    TutorialStateIdle(std::weak_ptr<ActorBase> _actor);
    // デスストラクタ
    ~TutorialStateIdle(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};

