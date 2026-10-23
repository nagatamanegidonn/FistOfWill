
#pragma once
#include"../../State/CharacterStateBase.h"

class EnemyStateChase :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateChase>
{
public:
    
    // コンストラクタ
    EnemyStateChase(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~EnemyStateChase() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};