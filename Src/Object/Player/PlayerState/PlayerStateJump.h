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
    ~PlayerStateJump() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;

private:

    float stepJump_; // ジャンプ入力受付時間
};

