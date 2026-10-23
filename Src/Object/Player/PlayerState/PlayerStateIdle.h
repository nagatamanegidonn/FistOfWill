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
    ~PlayerStateIdle() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};

