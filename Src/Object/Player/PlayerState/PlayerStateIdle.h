#pragma once
#include"../../State/CharacterStateBase.h"

class PlayerBase;

class PlayerStateIdle :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateIdle>
{
public:
    
    // コンストラクタ
    PlayerStateIdle(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateIdle(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};

