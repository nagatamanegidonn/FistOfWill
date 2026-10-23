
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
    ~PlayerStateDamage() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;

private:

	void AfterDamage(void);

};