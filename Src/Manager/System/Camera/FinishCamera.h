#pragma once
#include "CameraBase.h"

class ActorBase;

class FinishCamera :
    public CameraBase,
    public std::enable_shared_from_this<FinishCamera>
{
public:

    // ターゲット（倒した敵）と、演出終了後に戻るカメラ（PlayerCameraなど）を渡す
    // コンストラクタ
    FinishCamera(std::shared_ptr<ActorBase> target, std::shared_ptr<CameraBase> returnCamera);
    // デストラクタ
    ~FinishCamera(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override {}
    void Exit(void) override {}

private:

    std::weak_ptr<ActorBase> target_;       // 演出の対象（敵）
    std::shared_ptr<CameraBase> returnCam_; // 終わった後に戻すカメラ
    
    int cameraCutIndex_ = 1;                // 現在「何カメ」か (1 = 1カメ, 2 = 2カメ...)
    float cutTimer_ = 0.0f;                 // 各カットの経過時間タイマー
};