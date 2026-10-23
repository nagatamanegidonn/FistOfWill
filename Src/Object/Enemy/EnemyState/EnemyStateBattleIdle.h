#pragma once
#include"../../State/CharacterStateBase.h"

class EnemyStateBattleIdle :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateBattleIdle>
{
public:

    // コンストラクタ
    EnemyStateBattleIdle(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~EnemyStateBattleIdle() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};

