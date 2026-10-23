#pragma once
#include "CameraBase.h"

class ActorBase;

class ParryCamera :
    public CameraBase,
    public std::enable_shared_from_this<ParryCamera>
{
public:
    // 自プレイヤー、ターゲット（攻撃を弾いた敵）、演出終了後に戻るカメラ（PlayerCamera）を渡す
    ParryCamera(std::shared_ptr<ActorBase> player, std::shared_ptr<ActorBase> enemy, std::shared_ptr<CameraBase> returnCamera);
    ~ParryCamera(void) override = default;

    void Init() override;
    void Update() override;
    void Draw() override {}
    void Exit() override {}

private:
    std::weak_ptr<ActorBase> player_;       // プレイヤーの参照
    std::weak_ptr<ActorBase> enemy_;        // 敵の参照
    std::shared_ptr<CameraBase> returnCam_; // 終わった後に戻すカメラ

    int cameraCutIndex_ = 1;                // 1 = 衝撃のクローズアップ, 2 = 反撃への引き
    float cutTimer_ = 0.0f;                 // タイマー

    // ─── 追加：発動瞬間のアングルを固定するための変数 ───
    int lastTime_ = 0;                  // リアルタイム計測用
    VECTOR lockedBasePos_ = { 0,0,0 };    // パリィ発生時の元のカメラ位置
    VECTOR lockedTargetPos_ = { 0,0,0 };  // パリィ発生時の元の注視点
    VECTOR viewForward_ = { 0,0,0 };      // カメラの正面（前後ピストン用）ベクトル
    VECTOR viewRight_ = { 0,0,0 };        // カメラの真横（左右横揺れ用）ベクトル
};