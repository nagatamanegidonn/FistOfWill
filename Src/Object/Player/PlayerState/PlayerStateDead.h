
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
    ~PlayerStateDead(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};