#pragma once
#include <DxLib.h>
#include"../../State/CharacterStateBase.h"

class PlayerStateFall :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateFall>
{
public:

    // コンストラクタ
    PlayerStateFall(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateFall(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

private:

    float stepJump_;    // ジャンプ入力受付時間
    VECTOR gravityPow_; // ジャンプ力
};

