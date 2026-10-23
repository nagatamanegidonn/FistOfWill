
#include "TutorialEnemyOnHit.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Camera/CameraBase.h"
#include "../Manager/System/Camera/ParryCamera.h"
#include "../Manager/System/Game/TutorialManager.h"
#include "../Manager/Resource/SoundManager.h"

#include "../Player/PlayerBase.h"
#include "../Enemy/EnemyBase.h"
#include "../CharaStatus.h"
#include "../Attack/AttackBase.h"

#include "../Common/Collider/Collider.h"
#include "../Common/Collider/Geometry/Line.h"
#include "../Common/Collider/Geometry/Capsule.h"

#include "../Enemy/TutorialState/TutorialStateAttack.h"

TutorialEnemyOnHit::TutorialEnemyOnHit(EnemyBase& owner)
    :
    OnHitBase(owner),
    owner_(owner)
{
    onHitMap_.emplace(Collider::TAG::NORMAL_OBJECT, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitWall(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::GROUND, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitStage(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::NEUTRAL, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitAttack(hitColWeak);
        });
}

TutorialEnemyOnHit::~TutorialEnemyOnHit()
{
}

// タグがGROUNDのときの衝突後処理
void TutorialEnemyOnHit::OnHitStage(const std::weak_ptr<Collider>& hitColWeak)
{
    // コライダの持ち主が衝突時に行う処理でありコライダ自体の判定をしてない
    // つまり、Cubeのコライダが衝突したときに、CubeのコライダからPlayerのコライダを見ている

    // 衝突した相手コライダの衝突情報から自分のコライダ情報を受け取る
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();

    //　HitMyCollider のような関数があれば、相手のコライダから自分のコライダを見つけることもできる
    const auto& hitCol = hitColWeak.lock();
    if (!hitCol || hitCol->GetTag() != Collider::TAG::GROUND) return;

    // コライダーの継承先に変更
    // Line へのキャストを試みる
    Line* line = dynamic_cast<Line*>(&groundPreCol.lock()->GetGeometry());
    if (line)
    {
        // タグがFOOTかを確認
        if (groundPreCol.lock()->GetTag() == Collider::TAG::FOOT)
        {
            // Line 用の処理
            ProcessLineHit(*line, hitCol->GetGeometry());
            owner_.SetIsGround(true);
        }
        return;
    }
    // Capsule へのキャストを試みる
    Capsule* capsule = dynamic_cast<Capsule*>(&groundPreCol.lock()->GetGeometry());
    if (capsule)
    {
        // Capsule 用の処理（後述）
        ProcessCapsuleHit(*capsule, hitCol->GetGeometry());
        return;
    }
}

// タグがNORMAL_OBJECTのときの衝突後処理
void TutorialEnemyOnHit::OnHitWall(const std::weak_ptr<Collider>& hitColWeak)
{
    // 壁に当たったときの処理
    // 例えば、押し戻される、跳ね返るなど
    // hitColWeak を使って相手のコライダやジオメトリを取得し、必要な処理を行う
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();
    const auto& hitCol = hitColWeak.lock();

    // Capsule へのキャストを試みる
    Capsule* capsule = dynamic_cast<Capsule*>(&groundPreCol.lock()->GetGeometry());
    if (capsule)
    {
        auto& hits = capsule->GetHitInfo();

        for (int i = 0; i < hits.HitNum; i++)
        {
            auto hit = hits.Dim[i];
            for (int tryCnt = 0; tryCnt < 10; tryCnt++)
            {

                int pHit = HitCheck_Capsule_Triangle(
                    capsule->GetPosTop(), capsule->GetPosDown(), capsule->GetRadius(),
                    hit.Position[0], hit.Position[1], hit.Position[2]);

                // 衝突したコリジョンのタイプで処理分け
                if (pHit /*&& c.lock()->type_ == Collider::TYPE::STAGE*/)
                {
                    // カプセルを移動させる
                    owner_.AddMove(VScale(hit.Normal, 1.0f));
                    continue;
                }
                break;
            }
        }
    }
}

void TutorialEnemyOnHit::OnHitAttack(const std::weak_ptr<Collider>& hitColWeak)
{
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();
    const auto& hitCol = hitColWeak.lock();

    // 攻撃の持ち主を取得する
    // 一度 const ActorBase* のポインタにする
    const ActorBase* ownerPtr = &(hitColWeak.lock()->GetOwner());
    // dynamic_cast で目的のポインタ型にキャストする
    const CharaBase* charaPtr = dynamic_cast<const CharaBase*>(ownerPtr);

    // キャストが成功したかチェック
    if (charaPtr != nullptr) {
        // 参照として使いたいなら、ここで参照を作る
        const CharaBase& chara = *charaPtr;

        owner_.SetIsReaction(true); // 反応状態にする

        hitCol->Kill(); // 攻撃のコライダを消す（例：攻撃が一度当たったら消える場合） 
    }
    // 当たった判定の持ち主がAttackBaseの場合の処理
    else if (auto attackBasePtr = dynamic_cast<AttackBase*>(const_cast<ActorBase*>(ownerPtr)))
    {
        // 攻撃判定の生成者の確認
        auto ownerShared = attackBasePtr->GetOwner().lock();
        auto enemyShared = std::dynamic_pointer_cast<EnemyBase>(ownerShared);

        //　生成者が自分なら無視する（自分の攻撃に当たった場合は無視）
        if (enemyShared && enemyShared.get() == &owner_)
        {
            return; // 自分の攻撃なので無視
        }

        // owner_ は EnemyBase の参照なので、shared_ptr に変換する必要がある
        enemyShared = std::dynamic_pointer_cast<EnemyBase>(owner_.shared_from_this());

        // 当たった攻撃の持ち主がプレイヤーかつ、当たったところを見られている場合カメラを揺らす
        if (auto playerPtr = std::dynamic_pointer_cast<PlayerBase>(ownerShared))
        {
            // 外部からの呼び出し例：30フレームの間、強さ 10.0f で左右に滑らかに揺らす
            SceneManager::GetInstance().GetCameraBase().lock()->CameraShake(30, 10.0f);

            // 攻撃の重さとアーマーを比較して、パリィ可能かどうかを判定する
            // Ondamage内にてHPの減少リアクションを行うかの判断を行っている（リアクションの判断は存在するだけ）
            if (owner_.GetCharaStatus()->OnDamage(attackBasePtr->GetAttackDamage(), attackBasePtr->GetAttackWeight()))
            {            
                // ここから処理はリアクションが確定した場合の処理
                enemyShared->SetTarget(playerPtr);

                // 〇攻撃中ならパリィカメラの生成
                // state_ を EnemyStateAttack へキャストしてみる
                // まず lock() して参照を確保（生存確認）
                std::shared_ptr<StateBase> currentState = enemyShared->GetState().lock();

                // nullptr チェックを行いつつキャスト
                if (auto attackState = std::dynamic_pointer_cast<TutorialStateAttack>(enemyShared->GetState().lock()))
                {
                    // チュートリアル進行フラグを立てる
                    TutorialManager::GetInstance().StartNextTutorial(true, 2.0f);

                    // カウンターのサウンド再生
                    SoundManager::GetInstance().Play(L"HitCounter", Sound::TIMES::ONCE, true);

                    // パリィカメラを生成（追跡対象の敵と、終わったら戻るカメラを渡す）
                    auto currentCam = SceneManager::GetInstance().GetCameraBase().lock();
                    auto parryCam = std::make_shared<ParryCamera>(playerPtr, enemyShared, currentCam);
                    SceneManager::GetInstance().GetCameraBase().lock()->ChangeCamera(parryCam);
                }
                else
                {
                    // 通常の被ダメージ処理（リアクションなし）を行う場合の処理
                    SoundManager::GetInstance().Play(L"HitDamageLow", Sound::TIMES::ONCE, true);
                }
                owner_.SetIsReaction(true); // 反応状態にする
            }
            else
            {
                // 通常の被ダメージ処理（リアクションなし）を行う場合の処理
                SoundManager::GetInstance().Play(L"HitDamageLow", Sound::TIMES::ONCE, true);
            }
        }

        hitCol->Kill(); // 攻撃のコライダを消す（例：攻撃が一度当たったら消える場合） 
    }
    else
    {
        // CharaBaseではなかった場合の処理
    }
}

// 形状がLineのときの処理
void TutorialEnemyOnHit::ProcessLineHit(Line& line, Geometry& opponentGeo)
{
    if (line.IsHit(opponentGeo))
    {
        const Geometry::HitResult& res = line.GetHitResult();
        VECTOR pushNormal = res.normal;

        if (pushNormal.y < 0) pushNormal = VScale(pushNormal, -1.0f);

        if (pushNormal.y > 0.5f)
        {
            owner_.AddMove(VScale(pushNormal, res.depth + 0.05f));
        }
    }
}

// 形状がCapsuleのときの処理
void TutorialEnemyOnHit::ProcessCapsuleHit(Capsule& capsule, Geometry& opponentGeo)
{
    // カプセル用の衝突判定
    if (capsule.IsHit(opponentGeo))
    {
        const Geometry::HitResult& res = capsule.GetHitResult();

        // カプセル特有の押し戻し（例：横からぶつかった時の処理など）
        // res.normal や res.depth を使って owner_.Move() する
        owner_.AddMove(VScale(res.normal, res.depth));
    }
}
