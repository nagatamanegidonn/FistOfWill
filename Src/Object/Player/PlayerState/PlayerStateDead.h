
#pragma once
#include"../../State/CharacterStateBase.h"

class PlayerStateDead :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateDead>
{
public:
    
    // コンストラクタ
    PlayerStateDead(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateDead() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};