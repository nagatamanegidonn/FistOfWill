
#pragma once
#include"../../State/CharacterStateBase.h"

class PlayerStateDamage :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateDamage>
{
public:
    
    // コンストラクタ
    PlayerStateDamage(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateDamage(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

private:

	void AfterDamage(void);

};