#include <EffekseerForDXLib.h>
#include <algorithm>
#include <cmath>

#include "ParryCamera.h"
#include "../Manager/General/SceneManager.h"
#include "../../../Utility/Utility.h"

// ─── パリィ演出の調整パラメーター ───
namespace ParryConfig
{
    const float EFFECT_DURATION = 0.50f;  // 演出全体の長さ（0.5秒）
    const float PISTON_POWER = 60.0f;  // 前後に近づく・離れる最大幅
    const float SIDE_SHAKE_POWER = 20.0f;  // 左右のガタガタした揺れの最大幅
    const int   LOOP_COUNT = 5;      // 近づいて離れる往復回数（5回）

    // ★追加：揺れ終了時に開始地点よりどれだけ近づくか（好みに合わせて数値を調整してください）
    const float FINAL_CLOSE_DIST = 140.0f;
}

ParryCamera::ParryCamera(std::shared_ptr<ActorBase> player, std::shared_ptr<ActorBase> enemy, std::shared_ptr<CameraBase> returnCamera)
    : CameraBase(),
    player_(player),
    enemy_(enemy),
    returnCam_(returnCamera)
{
}

void ParryCamera::Init()
{
    // 状態維持のため nullptr をセット
    ChangeCamera(nullptr);

    cameraCutIndex_ = 1;
    cutTimer_ = 0.0f;

    // ゲームスピードが0でもカメラを正確に動かすために現在のミリ秒を記録
    lastTime_ = DxLib::GetNowCount();

    // 切り替わる直前の通常カメラの位置と向きを完全にロック
    if (returnCam_)
    {
        lockedBasePos_ = returnCam_->GetPos();
        lockedTargetPos_ = returnCam_->GetTargetPos();
    }
    else
    {
        lockedBasePos_ = DxLib::GetCameraPosition();
        lockedTargetPos_ = DxLib::GetCameraTarget();
    }

    // 固定した視線アングルから、純粋な「カメラの正面方向」と「カメラの右方向」のベクトルを計算
    viewForward_ = VNorm(VSub(lockedTargetPos_, lockedBasePos_));
    viewRight_ = VCross(viewForward_, Utility::DIR_U);

    pos_ = lockedBasePos_;
    targetPos_ = lockedTargetPos_;

    SceneManager::GetInstance().SetGameSpeedRate(0.0f); // ゲームスピードを0にする
}

void ParryCamera::Update()
{
    // どちらかの生存が怪しくなったら安全に復帰
    if (player_.expired() || enemy_.expired())
    {
        SceneManager::GetInstance().SetGameSpeedRate(1.0f);
        ChangeCamera(returnCam_);
        return;
    }

    // ニア・ファークリップを毎フレーム設定
    SetCameraNearFar(cameraNear_, cameraFar_);

    // 現実の経過時間を計算（GameSpeedRateが0でも確実に時間を進める）
    int currentTime = DxLib::GetNowCount();
    float realDeltaTime = static_cast<float>(currentTime - lastTime_) / 1000.0f; // 秒に変換
    lastTime_ = currentTime;

    // 実時間でタイマーを更新
    cutTimer_ += realDeltaTime;

    if (cameraCutIndex_ == 1)
    {
        // ─── 【カット1】固定軸での前後5回ピストン ＆ 左右横揺れ ───
        float duration = ParryConfig::EFFECT_DURATION;
        float t = std::clamp(cutTimer_ / duration, 0.0f, 1.0f);

        // 指定回数（5回）往復させるための角度計算 (1往復 = 2 * PI)
        float const PI = 3.14159265f;
        float angle = t * (static_cast<float>(ParryConfig::LOOP_COUNT) * 2.0f * PI);

        // サイン波・コサイン波を用いて、前後・左右の往復値を計算
        float pulseForward = std::sinf(angle);        // 前後ピストン用の綺麗な波
        float pulseSide = std::cosf(angle * 1.5f); // 左右の揺れ

        // 後半にかけて揺れの振幅自体は徐々に減衰していく (1.0f - t)
        float currentPistonPower = ParryConfig::PISTON_POWER * (1.0f - t);
        float currentSidePower = ParryConfig::SIDE_SHAKE_POWER * (1.0f - t);

        // 1. 前後ピストン移動（視線方向に沿って伸縮）
        VECTOR forwardOffset = VScale(viewForward_, pulseForward * currentPistonPower);

        // 2. 左右の激しい横揺れ（カメラの真横の軸に沿ってブレる）
        VECTOR sideOffset = VScale(viewRight_, pulseSide * currentSidePower);

        // 3. ★追加：終了時に少し近い位置へじんわり移動させるオフセット
        // 進行率 t (0.0 から 1.0) に応じて、徐々に正面方向へ近づいていきます
        VECTOR closeOffset = VScale(viewForward_, t * ParryConfig::FINAL_CLOSE_DIST);

        // すべての揺れ・移動ベクトルを合成
        VECTOR totalOffset = VAdd(VAdd(forwardOffset, sideOffset), closeOffset);

        // ロックした「元の位置」に対してオフセットを加算（回転せず、平行移動のみ）
        pos_ = VAdd(lockedBasePos_, totalOffset);
        targetPos_ = VAdd(lockedTargetPos_, totalOffset); // 注視点も同じだけ動かして画角をキープ

        if (t >= 1.0f)
        {
            cameraCutIndex_ = 2;
            cutTimer_ = 0.0f;
        }
    }
    else if (cameraCutIndex_ == 2)
    {
        // ─── 【カット2】元の位置より少し近い位置でピタッと静止させるフェーズ ───
        // カット1の終了時点で「開始地点より一歩近づいた位置」にいるので、
        // 0.05秒ほどその緊迫した位置をキープして見せます。
        float duration = 0.05f;
        float t = std::clamp(cutTimer_ / duration, 0.0f, 1.0f);

        // 終了時の固定位置（開始地点 + 前進オフセット）
        VECTOR finalPos = VAdd(lockedBasePos_, VScale(viewForward_, ParryConfig::FINAL_CLOSE_DIST));
        VECTOR finalTarget = VAdd(lockedTargetPos_, VScale(viewForward_, ParryConfig::FINAL_CLOSE_DIST));

        pos_ = finalPos;
        targetPos_ = finalTarget;

        if (t >= 1.0f)
        {
            cameraCutIndex_ = 3;
        }
    }
    else
    {
        // ─── 【演出終了】元の通常戦闘カメラ（PlayerCamera）へ戻す ───
        if (returnCam_)
        {
            returnCam_->ResetShake();
            returnCam_->SetNeedInit(false); // 復帰時にカメラがガクッとワープするのを防止
        }

        SceneManager::GetInstance().SetGameSpeedRate(1.0f); // ゲームスピードを通常に戻す
        ChangeCamera(returnCam_);
        return;
    }

    // DXライブラリ & Effekseer への反映
    DxLib::SetCameraPositionAndTarget_UpVecY(pos_, targetPos_);
    Effekseer_Sync3DSetting();
}