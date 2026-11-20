#pragma once
#include"../../State/CharacterStateBase.h"

class PlayerStateRun :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateRun>
{
public:
    
    // コンストラクタ
    PlayerStateRun(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateRun(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};

