#pragma once
#include"../../State/CharacterStateBase.h"

class PlayerBase;

class PlayerStateJump :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateJump>
{
public:
    
    // コンストラクタ
    PlayerStateJump(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateJump(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

private:

    float stepJump_; // ジャンプ入力受付時間
};

