#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <algorithm>
#include "PauseCamera.h"
#include "../Utility/Utility.h"
#include "../Application.h"

PauseCamera::PauseCamera(std::shared_ptr<CameraBase> returnCamera)
    : CameraBase(),
    returnCam_(returnCamera),
    radius_(300.0f),
    prevMouseX_(0),
    prevMouseY_(0),
    isDragging_(false)
{
}

void PauseCamera::Init()
{
    // 次の状態をクリア
    ChangeCamera(nullptr);

    // ポーズ中なのでマウスカーソルを表示する
    SetMouseDispFlag(TRUE);

	// ポーズカメラが有効になった時点で、元のカメラの操作を解除する
    returnCam_->ChangeCamera(nullptr);

    if (returnCam_)
    {
        // 1. 元のカメラの実際の座標・注視点を引き継ぐ
        pos_ = returnCam_->GetPos();
        targetPos_ = returnCam_->GetTargetPos();

        // 2. カメラ座標と注視点の差分ベクトルを求める
        VECTOR diff = VSub(pos_, targetPos_);
        radius_ = VSize(diff);
        if (radius_ < 50.0f) radius_ = 50.0f;

        // 3. 差分ベクトルから「現在の回転角度（angles_）」を正確に逆算する
        // (PlayerCameraがどのように座標を作っていても、現在の見た目から角度を再現する)
        angles_.y = atan2f(diff.x, -diff.z); // 横回転 (Yaw)
        angles_.x = asinf(diff.y / radius_); // 縦回転 (Pitch)
        angles_.z = 0.0f;
    }
    else
    {
        pos_ = VGet(0.0f, 100.0f, -300.0f);
        targetPos_ = VGet(0.0f, 100.0f, 0.0f);
        angles_ = VGet(Utility::Deg2RadF(20.0f), 0.0f, 0.0f);
        radius_ = 300.0f;
    }

    // 初期マウス位置を取得
    GetMousePoint(&prevMouseX_, &prevMouseY_);
}

void PauseCamera::Update()
{
    SetCameraNearFar(cameraNear_, cameraFar_);

    int mouseX = 0, mouseY = 0;
    GetMousePoint(&mouseX, &mouseY);

    int diffX = mouseX - prevMouseX_;
    int diffY = mouseY - prevMouseY_;

    // --- 1. マウスドラッグによる視点回転 ---
    // 右クリック（または左クリック）を押している間、自由回転
    if ((GetMouseInput() & MOUSE_INPUT_RIGHT) || (GetMouseInput() & MOUSE_INPUT_LEFT))
    {
        const float ROT_SPEED = 0.005f;

        angles_.y += static_cast<float>(diffX) * ROT_SPEED; // 横回転
        angles_.x += static_cast<float>(diffY) * ROT_SPEED; // 縦回転

        // 縦回転の限界角度制限（真上・真下での反転防止）
        const float LIMIT_UP = Utility::Deg2RadF(85.0f);
        const float LIMIT_DOWN = Utility::Deg2RadF(-85.0f);
        angles_.x = std::clamp(angles_.x, LIMIT_DOWN, LIMIT_UP);
    }

    // --- 2. ホイール操作によるズームイン / ズームアウト ---
    int wheel = GetMouseWheelRotVol();
    if (wheel != 0)
    {
        const float ZOOM_SPEED = 30.0f;
        radius_ -= static_cast<float>(wheel) * ZOOM_SPEED;
        radius_ = std::clamp(radius_, 50.0f, 1500.0f); // 最小・最大距離制限
    }

    prevMouseX_ = mouseX;
    prevMouseY_ = mouseY;

    // --- 3. 角度と距離からカメラ座標（pos_）を計算 ---
    // 球面座標系の計算
    pos_.x = targetPos_.x + radius_ * cosf(angles_.x) * sinf(angles_.y);
    pos_.y = targetPos_.y + radius_ * sinf(angles_.x);
    pos_.z = targetPos_.z - radius_ * cosf(angles_.x) * cosf(angles_.y);

    // --- 4. DXライブラリへ反映 ---
    DxLib::SetCameraPositionAndTarget_UpVecY(pos_, targetPos_);
    Effekseer_Sync3DSetting();
}

void PauseCamera::Draw()
{
}

void PauseCamera::Exit()
{
    // ポーズカメラ終了時にマウスカーソルを非表示に戻す
    SetMouseDispFlag(FALSE);

    // 必要に応じて画面中央へマウスを戻す（PlayerCameraのマウス操作の暴発防止）
    int centerX = Application::SCREEN_SIZE_X / 2;
    int centerY = Application::SCREEN_SIZE_Y / 2;
    SetMousePoint(centerX, centerY);
}

void PauseCamera::Resume()
{
    // ポーズ解除時の後処理
    SetMouseDispFlag(FALSE);

    int centerX = Application::SCREEN_SIZE_X / 2;
    int centerY = Application::SCREEN_SIZE_Y / 2;
    SetMousePoint(centerX, centerY);

    if (returnCam_)
    {
        // 戻るPlayerCameraの状態を整える
        returnCam_->ResetShake();
        returnCam_->SetNeedInit(false);

        // ★重要
        // PlayerCamera -> PauseCamera の参照を先に切る
        //
        // これをしないと
        //
        // PlayerCamera
        //     ↓ shared_ptr
        // PauseCamera
        //     ↓ shared_ptr
        // PlayerCamera
        //
        // という循環参照が残る
        //returnCam_->ChangeCamera(nullptr);
    }

    // PauseCamera -> PlayerCamera
    ChangeCamera(returnCam_);
}
