#include "AttackManager.h"

AttackManager::AttackManager(std::shared_ptr<ActorBase> owner)
    : 
    owner_(owner),
    currentAttack_(nullptr),
    attackTimer_(0.0f),
    isHitActive_(false)
{
    auto actor = owner_.lock();
    if (!actor) return;
    // ActorBaseの MakeCollider を使って、手のボーン座標などに球体やカプセルの当たり判定を作る
    // タグは Collider::TAG::ATTACK などを新設すると良いです
    // VECTOR handPos = Utility::MV1GetFreamPos(actor->GetTransform().modelId, currentAttack_->boneName);
    // MakeCollider(...);
}

AttackManager::~AttackManager()
{
    if (auto actor = owner_.lock()) {
        // DeleteColliderAtTag(Collider::TAG::ATTACK); などを呼んで消す
    }
}

void AttackManager::StartAttack(const AttackData& data)
{
    currentAttack_ = &data;
    attackTimer_ = 0.0f;
    isHitActive_ = false;

    // 持ち主のアニメーションを再生
    if (auto actor = owner_.lock()) {
        // animationController_->Play() などを呼ぶ
    }
}

void AttackManager::Update(float deltaTime)
{
    if (!currentAttack_) return;

    attackTimer_ += deltaTime;

    // 1. 攻撃判定の発生タイミング判定
    if (!isHitActive_ && attackTimer_ >= currentAttack_->startHitTime && attackTimer_ <= currentAttack_->endHitTime)
    {
        isHitActive_ = true;
        CreateAttackCollider();
    }

    // 2. 攻撃判定の終了タイミング判定
    if (isHitActive_ && attackTimer_ > currentAttack_->endHitTime)
    {
        isHitActive_ = false;
        DestroyAttackCollider();
    }

    // 3. 攻撃全体の終了判定
    // アニメーションが終わるか、攻撃時間が終わったらリセット
    if (attackTimer_ >= currentAttack_->endHitTime /* もしくはアニメ終了時間 */)
    {
        currentAttack_ = nullptr;
    }
}