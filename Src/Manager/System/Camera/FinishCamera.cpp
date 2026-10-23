
#include <EffekseerForDXLib.h>

#include "FinishCamera.h"
#include "../../../Utility/Utility.h"

FinishCamera::FinishCamera(std::shared_ptr<ActorBase> target, std::shared_ptr<CameraBase> returnCamera)
    : CameraBase(),
    target_(target),
    returnCam_(returnCamera)
{
}

void FinishCamera::Init()
{
    // 自分自身を現在のカメラ状態として維持
    ChangeCamera(nullptr);

    cameraCutIndex_ = 1; // 1カメからスタート
    cutTimer_ = 0.0f;

    pos_ = Utility::VECTOR_ZERO;
    targetPos_ = Utility::VECTOR_ZERO;
}

void FinishCamera::Update()
{
    // ターゲットが途中で消滅したら安全のために演出を終わらせる
    if (target_.expired())
    {
        ChangeCamera(returnCam_);
        return;
    }

    // ニア・ファークリップを毎フレーム設定
    SetCameraNearFar(cameraNear_, cameraFar_);

    // 敵の現在位置を取得
    VECTOR enemyPos = target_.lock()->GetTransform().pos;

    // ─── 緩急の肝①：ゲーム全体の時間を操作して「ドン！」を強調する ───
    // ※もしゲーム全体のスローモーション機能（TimeScaleなど）がない場合、
    // ここでDxLibのウエイトや擬似スローを挟むだけでも劇的に変わります。
    // カメが切り替わった直後の0.1秒間だけ超スローにする
    float timeScale = 1.0f;
    if (cutTimer_ < 0.1f)
    {
        timeScale = 0.1f; // 10倍のスロー（ほぼ静止）
    }
    else
    {
        timeScale = 1.0f; // 通常速度に戻る
    }

    // タイマー更新 (60fps想定で1フレームあたり約0.016秒進む)
    float deltaTime = 1.0f / 60.0f;
    cutTimer_ += deltaTime / 1.2f;// 1.2fは１つの時間を少し長くする

    // ─── 緩急の肝②：イージング（進捗率のカーブ化） ───
    // 普通のタイマーだと 0.0 → 1.0 まで等速で進みますが、
    // 「最初はものすごい勢いで動き、後半にピタッと止まる」カーブ（EaseOutCubic）を作ります。
    
    // 各カメの目標座標（開始点と終了点）
    VECTOR startPos = Utility::VECTOR_ZERO;
    VECTOR endPos = Utility::VECTOR_ZERO;
    float currentCutDuration = 0.4f; // このカットのトータル時間


    // ─── 1カメ、2カメ、3カメの切り替えロジック ───
    if (cameraCutIndex_ == 1)
    {
        currentCutDuration = 0.35f; // 1カメは短く鋭く！
        targetPos_ = VAdd(enemyPos, VGet(0.0f, 110.0f, 0.0f)); // 敵の顔

        // 【演出】遠くから敵の顔面へ向かって「ドン！」と超高速ズームイン
        startPos = VAdd(targetPos_, VGet(0.0f, 40.0f, 350.0f)); // 開始：ちょっと遠目
        endPos = VAdd(targetPos_, VGet(0.0f, 10.0f, 100.0f)); // 終了：ドアップ面撮り

        // 進捗率（0.0 ~ 1.0）
        float t = std::clamp(cutTimer_ / currentCutDuration, 0.0f, 1.0f);
        // イージング公式（Ease Out Cubic）: 1 - (1 - t)^3
        float ease = 1.0f - std::powf(1.0f - t, 3.0f);

        // カメラ位置をブレンド（線形補間）
        pos_ = VAdd(startPos, VScale(VSub(endPos, startPos), ease));

        if (t >= 1.0f)
        {
            cameraCutIndex_ = 2;
            cutTimer_ = 0.0f;
        }
    }
    else if (cameraCutIndex_ == 2)
    {
        currentCutDuration = 0.45f;
        targetPos_ = VAdd(enemyPos, VGet(0.0f, 80.0f, 0.0f)); // 敵の腰

        // 【演出】カメラが切り替わった瞬間、敵の背後にワープし、そこからさらに後ろに「ドン！」とのけぞる（突き放す引き）
        startPos = VAdd(targetPos_, VGet(-150.0f, 50.0f, -150.0f)); // 開始：近めの斜め後ろ
        endPos = VAdd(targetPos_, VGet(-350.0f, 120.0f, -350.0f));  // 終了：大きく引く

        float t = std::clamp(cutTimer_ / currentCutDuration, 0.0f, 1.0f);
        float ease = 1.0f - std::powf(1.0f - t, 3.0f); // 同じく後半にかけて減速

        pos_ = VAdd(startPos, VScale(VSub(endPos, startPos), ease));

        if (t >= 1.0f)
        {
            cameraCutIndex_ = 3;
            cutTimer_ = 0.0f;
        }
    }
    else if (cameraCutIndex_ == 3)
    {
        currentCutDuration = 0.5f;
        targetPos_ = VAdd(enemyPos, VGet(0.0f, 40.0f, 0.0f)); // 敵の足元

        // 【演出】最後は真上から地面に向けて「ガツン！」と見下ろす（叩きつけるような俯瞰）
        startPos = VAdd(targetPos_, VGet(10.0f, 600.0f, -10.0f)); // 開始：超高所
        endPos = VAdd(targetPos_, VGet(20.0f, 350.0f, -50.0f)); // 終了：少し近づいて凝視

        float t = std::clamp(cutTimer_ / currentCutDuration, 0.0f, 1.0f);
        float ease = 1.0f - std::powf(1.0f - t, 3.0f);

        pos_ = VAdd(startPos, VScale(VSub(endPos, startPos), ease));

        if (t >= 1.0f)
        {
            cameraCutIndex_ = 4;
        }
    }
    else
    {
        // 【演出終了】元のカメラ（PlayerCamera）に戻す
        if (returnCam_)
        {
            returnCam_->ResetShake();       // 戻る前にPlayerCameraの古いシェイク残りを完全に消去
			returnCam_->SetNeedInit(false); // 戻るカメラのInitをスキップ（すでに初期化済みなので）
        }

        ChangeCamera(returnCam_);
        return;
    }

    // ─── 確定したカメラ座標をDxLibに適用 ───
    DxLib::SetCameraPositionAndTarget_UpVecY(pos_, targetPos_);
    Effekseer_Sync3DSetting();
}