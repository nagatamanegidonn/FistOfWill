#pragma once
#include"../../State/CharacterStateBase.h"

class EnemyStateDamage :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateDamage>
{
public:
    
    // コンストラクタ
    EnemyStateDamage(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~EnemyStateDamage() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;

private:

	void AfterDamage(void);
};