#pragma once
#include"../../State/CharacterStateBase.h"

class EnemyStateDead :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateDead>
{
public:
    
    // コンストラクタ
    EnemyStateDead(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~EnemyStateDead() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};