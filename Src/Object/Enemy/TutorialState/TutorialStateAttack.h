#pragma once
#include <DxLib.h>
#include"../../State/CharacterStateBase.h"
#include"../Object/CharaStatus.h"

class AttackData;

class TutorialStateAttack :
    public CharacterStateBase,
    public std::enable_shared_from_this<TutorialStateAttack>
{
public:

    // コンストラクタ
    TutorialStateAttack(std::weak_ptr<ActorBase> _actor, int attackIndex = 0);
    // デストラクタ
    ~TutorialStateAttack(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;

    // プレイヤーを少し前に動かす
    void AttackMove(void);

private:

    // 状態遷移前の更新
    void ChangeStateUpdate(void);

    VECTOR justPos_;    // 回避の当たり判定位置

    bool isNextAttack_;     // 次の攻撃に移行するかのフラグ
    bool isAttackStarted_;  // 攻撃が開始されたかどうかのフラグ

	CharaStatus::Armor defaultArmor_; // デフォルトのアーマー値を保持する変数

    int attackIndex_;       // 現在のコンボ段数 (0, 1, 2...)
    std::shared_ptr<AttackData> currentData_;// 現在の段数の攻撃データ
};