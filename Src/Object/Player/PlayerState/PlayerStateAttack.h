#pragma once
#include <memory>
#include <DxLib.h>
#include"../../State/CharacterStateBase.h"

class AttackData;

class PlayerStateAttack :
    public CharacterStateBase,
    public std::enable_shared_from_this<PlayerStateAttack>
{
public:

    // コンストラクタ
    PlayerStateAttack(std::weak_ptr<ActorBase> _actor, int attackIndex = 0);
    // デストラクタ
    ~PlayerStateAttack(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

    // プレイヤーを少し前に動かす
	void AttackMove(void);

private:

	bool isNextAttack_; // 次の攻撃に移行するかのフラグ
	bool isAttackStarted_; // 攻撃が開始されたかどうかのフラグ
	bool isSkill_;         // スキル攻撃かどうかのフラグ
    int attackIndex_;      // 現在のコンボ段数 (0, 1, 2...)
    
    std::shared_ptr<AttackData> currentData_;// 現在の段数の攻撃データ

    // 状態変更時の後処理
    void StateAfter(void);

};

