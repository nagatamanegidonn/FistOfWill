#include "PlayerOnHit.h"

#include "../Player/PlayerBase.h"
#include "../Common/Collider/Collider.h"
#include "../Common/Collider/Geometry/Line.h"
#include "../Common/Collider/Geometry/Capsule.h"
#include "../Common/Collider/Geometry/Model.h"

#include "../Attack/AttackBase.h"
#include "../Enemy/EnemyBase.h"
#include "../CharaStatus.h"

#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Camera/CameraBase.h"

PlayerOnHit::PlayerOnHit(PlayerBase& owner)
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
            return OnHitGround(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::MACHINE, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitCharacter(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::EVASION, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitJustEvasion(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::ATTACK, [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitAttack(hitColWeak);
        });
    onHitMap_.emplace(Collider::TAG::HOSTILITY , [this](const std::weak_ptr<Collider>& hitColWeak)
        {
            return OnHitAttack(hitColWeak);
        });
}

PlayerOnHit::~PlayerOnHit()
{
}

// タグがGROUNDのときの衝突後処理
void PlayerOnHit::OnHitGround(const std::weak_ptr<Collider>& hitColWeak)
{
    // コライダの持ち主が衝突時に行う処理でありコライダ自体の判定をしてない
	// つまり、Cubeのコライダが衝突したときに、CubeのコライダからPlayerのコライダを見ている

    // 衝突した相手コライダの衝突情報から自分のコライダ情報を受け取る
    // コライダ
	const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();

	//　HitMyCollider のような関数があれば、相手のコライダから自分のコライダを見つけることもできる

	// 衝突した相手コライダのタグがGROUNDでなければ処理しない
    const auto& hitCol = hitColWeak.lock();
    if (!hitCol || hitCol->GetTag() != Collider::TAG::GROUND) return;

    // コライダーの継承先に変更
    // Line へのキャストを試みる
    Line* line = dynamic_cast<Line*>(&groundPreCol.lock()->GetGeometry());
    if (line)
    {
		// 衝突したコリジョンがモデルかどうかを確認
        Model* model = dynamic_cast<Model*>(&hitColWeak.lock()->GetGeometry());
        if (model)
        {
            // 内部の hitInfo_.HitFlag を確認
            auto& hit = line->GetHitInfo();
            if (hit.HitFlag > 0)
            {

                const Geometry::HitResult& res = line->GetHitResult();
                VECTOR pushNormal = res.normal;

                // 法線が下を向いていた場合はひっくり返す（地面判定なので基本は上向き）
                if (pushNormal.y < 0) pushNormal = VNorm(VScale(pushNormal, -1.0f));

                // 傾斜制限：ポリゴンの向きが一定以上の「床」であれば押し戻す（壁や急斜面で上るのを防ぐ）
                if (pushNormal.y > 0.5f)
                {
                    // めり込んだ深さ（depth）に少しだけ余裕（0.005f~0.05f程度）を足して押し戻す
                    owner_.AddMove(VScale(pushNormal, res.depth + 0.005f));
                }
                // ※もしProcessLineHit内でSetIsGround(true)をしていないならここで呼ぶ
                owner_.SetIsGround(true);
            }
        }
        else
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
    }
    // Capsule へのキャストを試みる
    Capsule* capsule = dynamic_cast<Capsule*>(&groundPreCol.lock()->GetGeometry());
    if (capsule)
    {
        // Capsule 用の処理（後述）ここで横入りの設置判定をできる限りなくす
        ProcessCapsuleHit(*capsule, hitCol->GetGeometry());
        return;
    }

}
void PlayerOnHit::OnHitWall(const std::weak_ptr<Collider>& hitColWeak)
{
    // 衝突相手の古来だ情報を取り出す
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();

    // 衝突した相手コライダの処理
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

    Line* line = dynamic_cast<Line*>(&groundPreCol.lock()->GetGeometry());
    if (line)
    {
        // 衝突したコリジョンがモデルかどうかを確認
        Model* model = dynamic_cast<Model*>(&hitColWeak.lock()->GetGeometry());
        if (model)
        {
            // 内部の hitInfo_.HitFlag を確認
            auto& hit = line->GetHitInfo();
            if (hit.HitFlag > 0)
            {
                const Geometry::HitResult& res = line->GetHitResult();
                VECTOR pushNormal = res.normal;

                // 法線が下を向いていた場合はひっくり返す（地面判定なので基本は上向き）
                if (pushNormal.y < 0) pushNormal = VNorm(VScale(pushNormal, -1.0f));

                // 傾斜制限：ポリゴンの向きが一定以上の「床」であれば押し戻す（壁や急斜面で上るのを防ぐ）
                if (pushNormal.y > 0.5f)
                {
                    // めり込んだ深さ（depth）に少しだけ余裕（0.005f~0.05f程度）を足して押し戻す
                    owner_.AddMove(VScale(pushNormal, res.depth + 0.005f));
                }
                // ※もしProcessLineHit内でSetIsGround(true)をしていないならここで呼ぶ
                owner_.SetIsGround(true);
            }
        }
        else
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
    }
}
void PlayerOnHit::OnHitCharacter(const std::weak_ptr<Collider>& hitColWeak)
{
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();
    const auto& hitCol = hitColWeak.lock();

    // Capsule へのキャストを試みる
    Capsule* capsule = dynamic_cast<Capsule*>(&groundPreCol.lock()->GetGeometry());
    if (capsule)
    {
        // Capsule 用の処理（後述）
        ProcessCapsuleHit(*capsule, hitCol->GetGeometry());
        return;
    }
}

void PlayerOnHit::OnHitJustEvasion(const std::weak_ptr<Collider>& hitColWeak)
{
    // 判定にはいいている間はジャスト回避が発動するようにする
    owner_.SetIsJust(true);
}

void PlayerOnHit::OnHitAttack(const std::weak_ptr<Collider>& hitColWeak)
{
    const auto& groundPreCol = hitColWeak.lock()->GetHitCollider();
    const auto& hitCol = hitColWeak.lock();

    // 一度 Attackの持ち主をconst ActorBase* のポインタにする
    const ActorBase* ownerPtr = &(hitColWeak.lock()->GetOwner());

    // dynamic_cast で目的のポインタ型にキャストする
    const CharaBase* charaPtr = dynamic_cast<const CharaBase*>(ownerPtr);

    // キャストが成功したかチェック
    if (charaPtr != nullptr) {
        // 参照として使いたいなら、ここで参照を作る
        const CharaBase& chara = *charaPtr;

        // 処理を書く
        int hp = owner_.GetCharaStatus()->GetNowHP();
        if (hp <= 0)
        {
            //chara.cha
			//owner_.Delete(); // HPが0以下なら削除する
            return;
        }
        hp -= chara.GetCharaStatus()->GetAT();
        owner_.GetCharaStatus()->SetNowHP(hp);
    
        hitCol->Kill(); // 攻撃のコライダを消す（例：攻撃が一度当たったら消える場合） 
    }
    // 当たった判定がAttackBaseの場合の処理
    else if (auto attackBasePtr = dynamic_cast<AttackBase*>(const_cast<ActorBase*>(ownerPtr)))
    {
        // 攻撃判定の生成者の確認
        auto ownerShared = attackBasePtr->GetOwner().lock();
        auto playerShared = std::dynamic_pointer_cast<PlayerBase>(ownerShared);

        //　生成者が自分なら無視する（自分の攻撃に当たった場合は無視）
        if (playerShared && playerShared.get() == &owner_)
        {
            return; // 自分の攻撃なので無視
        }

        // 当たった攻撃の持ち主がプレイヤーかつ、当たったところを見られている場合カメラを揺らす
        if (auto enemyPtr = std::dynamic_pointer_cast<EnemyBase>(ownerShared))
        {
            // 攻撃の重さとアーマーを比較して、パリィ可能かどうかを判定する
            // Ondamage内にてHPの減少リアクションを行うかの判断を行っている（リアクションの判断は存在するだけ）
            if (owner_.GetCharaStatus()->OnDamage(attackBasePtr->GetAttackDamage(), attackBasePtr->GetAttackWeight()))
            {
                // 外部からの呼び出し例：30フレームの間、強さ 10.0f で左右に滑らかに揺らす
                SceneManager::GetInstance().GetCameraBase().lock()->CameraShake(30, 10.0f);

                owner_.SetIsReaction(true); // 反応状態にする
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
void PlayerOnHit::ProcessLineHit(Line& line, Geometry& opponentGeo)
{
    const Geometry::HitResult& res = line.GetHitResult();
    VECTOR pushNormal = res.normal;

    if (pushNormal.y < 0) pushNormal = VNorm(VScale(pushNormal, -1.0f));

    if (pushNormal.y > 0.5f)
    {
        owner_.AddMove(VScale(pushNormal, res.depth + 0.05f));
        //owner_.JumpReset();
    }
}

// 形状がCapsuleのときの処理
void PlayerOnHit::ProcessCapsuleHit(Capsule& capsule, Geometry& opponentGeo)
{
	Model* model = dynamic_cast<Model*>(&opponentGeo);

	// modelにキャストできた場合は、モデルのポリゴンに対してカプセルの衝突判定を行う
	if (model != nullptr)
    {
        //auto hits = capsule.GetHitInfo();

        //for (int i = 0; i < hits.HitNum; i++)
        //{
        //    auto hit = hits.Dim[i];
        //    for (int tryCnt = 0; tryCnt < 10; tryCnt++)
        //    {
        //        int pHit = HitCheck_Capsule_Triangle(
        //            capsule.GetPosTop(), capsule.GetPosDown(), capsule.GetRadius(),
        //            hit.Position[0], hit.Position[1], hit.Position[2]);
        //        // 衝突したコリジョンのタイプで処理分け
        //        if (pHit /*&& c.lock()->type_ == Collider::TYPE::STAGE*/)
        //        {
        //            // カプセルを移動させる
        //            owner_.AddMove(VScale(hit.Normal, 1.0f));
        //            continue;
        //        }
        //        break;
        //    }
        //}

        // Model 以外のジオメトリに対する処理（必要に応じて追加）
        return;
    }
    // カプセル用の衝突判定
    else if (capsule.IsHit(opponentGeo))
    {
        const Geometry::HitResult& res = capsule.GetHitResult();

        // カプセル特有の押し戻し（例：横からぶつかった時の処理など）
        // res.normal や res.depth を使って owner_.Move() する
        owner_.AddMove(VScale(res.normal, res.depth));
    }
}
