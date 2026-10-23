
#pragma once
#include"../../State/CharacterStateBase.h"

class AttackData;

class EnemyStateAttack :
    public CharacterStateBase,
    public std::enable_shared_from_this<EnemyStateAttack>
{
public:
    
    // コンストラクタ
    EnemyStateAttack(std::weak_ptr<ActorBase> _actor, int attackIndex = 0);
    // デストラクタ
    ~EnemyStateAttack() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;

    // プレイヤーを少し前に動かす
    void AttackMove(void);

private:

    // 状態遷移前の更新
    void ChangeStateUpdate();

    VECTOR justPos_;    // 回避の当たり判定位置

    bool isNextAttack_; // 次の攻撃に移行するかのフラグ
    bool isAttackStarted_; // 攻撃が開始されたかどうかのフラグ

    int attackIndex_;       // 現在のコンボ段数 (0, 1, 2...)
    std::shared_ptr<AttackData> currentData_;// 現在の段数の攻撃データ
};