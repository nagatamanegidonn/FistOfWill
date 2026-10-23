#pragma once
#include"../../State/CharacterStateBase.h"

class EnemyStateIdle :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateIdle>
{
public:
    
    // コンストラクタ
    EnemyStateIdle(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~EnemyStateIdle() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};

