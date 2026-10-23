#pragma once
#include <string>
#include "../CharaStatus.h"
#include "../Common/Collider/Collider.h"

enum class AttackType : int
{
    Melee = 0,  // 近接追従
    Bullet = 1, // 遠隔弾
    AOE = 2,    // 範囲設置
};

class AttackData
{
public:

    // コンストラクタ
    AttackData();
    // デストラクタ
    ~AttackData();

    std::wstring animationId;   // 再生するアニメーションID
    int attackPower;            // 攻撃力 (AT)
	int startTime;              // 攻撃の発生フレーム
    float cancelableTime;       // 次のコンボ入力を受け付け開始するアニメーション時間
    std::wstring frameName;     // 当たり判定を付与するボーン(フレーム)名
    float colliderRadius;       // 判定の半径
    float forwardSpeed;         // 攻撃時の前進速度
    int nextAttackId;           // 次の攻撃のIndex

	AttackType attackType; // 攻撃タイプ

    // 攻撃の重さ
    CharaStatus::AttackWeight m_attackWeight = CharaStatus::AttackWeight::Light;

    // アーマー
    CharaStatus::Armor m_armor = CharaStatus::AttackWeight::Light;

    // 攻撃コライダーに付与するタグ（デフォルトは中立や敵対など）
    Collider::TAG colliderTag;
};

