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
    ~PlayerStateRun() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};

