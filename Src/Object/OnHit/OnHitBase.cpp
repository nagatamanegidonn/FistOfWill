#include "../Common/Collider/Collider.h"
#include "OnHitBase.h"

OnHitBase::OnHitBase(ActorBase& owner)
{
    // 衝突物別関数の登録
    onHitMap_.emplace(Collider::TAG::NONE, [this](const std::weak_ptr<Collider>& opponentCollider)
        {
            return OnHitNone(opponentCollider);
        });
}

OnHitBase::~OnHitBase()
{
}

void OnHitBase::Update(const std::weak_ptr<Collider>& opponentCollider)
{
    auto collider = opponentCollider.lock();

    // 中身の確認
    if (!collider)
    {
        // 無効なポインタの場合は何もしない
        return;
    }

    // 登録されていない場合
    if (onHitMap_.count(collider->GetTag()) == 0)
    {
        // 処理を行わずに終了
        return;
    }

    // 衝突物別の処理へ
    onHitMap_[collider->GetTag()](opponentCollider);
}
