#pragma once
#include <string>
#include <memory>

class ActorBase;

struct AttackData
{
    int attackId;             // 攻撃ID（コンボの識別に使う）
    std::wstring animPath;    // 再生するアニメーションファイルのパス

    // タイミング管理（フレーム数、または秒数）
    float startHitTime;       // 攻撃判定が「出る」時間
    float endHitTime;         // 攻撃判定が「消える」時間
    float nextInputStartTime; // 次のコンボ入力を「受付開始する」時間
    float nextInputEndTime;   // 次のコンボ入力を「締め切る」時間

    int nextAttackId;         // コンボ入力が成功したときの次の攻撃ID

    // 判定・演出情報
    float rangeRadius;        // 攻撃判定の大きさ（球体やカプセルの半径）
    std::string boneName;     // 武器を持つ手や、攻撃を発生させるボーン名（"mixamorig:RightHand" など）
    int damage;               // 攻撃力
};

class AttackManager
{
private:
    std::weak_ptr<ActorBase> owner_;      // 攻撃の持ち主（PlayerやEnemy）
    const AttackData* currentAttack_;     // 現在実行中の攻撃データ
    float attackTimer_;                   // 攻撃開始からの経過時間
    bool isHitActive_;                    // 現在当たり判定が出ているか

public:

    AttackManager(std::shared_ptr<ActorBase> owner);

    ~AttackManager(void);

    // 攻撃の開始
    void StartAttack(const AttackData& data);

    // 毎フレームの更新（CharacterのUpdateから呼ぶ）
    void Update(float deltaTime);

private:
    void CreateAttackCollider()
    {
        auto actor = owner_.lock();
        if (!actor) return;

        // ActorBaseの MakeCollider を使って、手のボーン座標などに球体やカプセルの当たり判定を作る
        // タグは Collider::TAG::ATTACK などを新設すると良いです
        // VECTOR handPos = Utility::MV1GetFreamPos(actor->GetTransform().modelId, currentAttack_->boneName);
        // MakeCollider(...);
    }

    void DestroyAttackCollider()
    {
        if (auto actor = owner_.lock()) {
            // DeleteColliderAtTag(Collider::TAG::ATTACK); などを呼んで消す
        }
    }
};