
#pragma once
#include <DxLib.h>
#include"../../State/CharacterStateBase.h"

class PlayerStateStep :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateStep>
{
public:
    
    // コンストラクタ
    PlayerStateStep(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~PlayerStateStep(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

private:

    void StepMove(void);
	void StepAfter(void);

	float stepTimeRate_;   // ステップの進行度（0.0～1.0）
	VECTOR stepDirection_; // ステップの方向ベクトル
};

