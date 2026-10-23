#include <EffekseerForDXLib.h>

#include "NormalCamera.h"
#include "../../../Utility/Utility.h"
#include "../../../Object/ActorBase.h"

NormalCamera::NormalCamera(void)
	:
	CameraBase()
{
}

void NormalCamera::Init()
{
	// 次の状態を自分の状態を入れる
	//ChangeCamera(shared_from_this());
	ChangeCamera(nullptr);

	// カメラの初期設定
	pos_ = { 0.0f, 100.0f, -500.0f };
	pos_ = VECTOR{ 0.0f, 200.0f, -300.0f };

	// 注視点
	targetPos_ = Utility::VECTOR_ZERO;
	targetPos_ = VECTOR{ 0.0f, 100.0f, 0.0f };

	// カメラの上方向
	cameraUp_ = Utility::DIR_U;

	angles_.x = Utility::Deg2RadF(30.0f);
	angles_.y = 0.0f;
	angles_.z = 0.0f;

	rot_ = Quaternion();
}

void NormalCamera::Update()
{
	//レンダーターゲットが毎フレーム更新されるので
	//ニアクリップとファークリップを毎フレーム設定し直す
	SetCameraNearFar(cameraNear_, cameraFar_);

	// 更新処理
#ifdef _DEBUG
	UpdateDebugImGui();
#endif // _DEBUG

    // DxLibに反映
    DxLib::SetCameraPositionAndTarget_UpVecY(
        pos_,
        targetPos_
    );

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	// 参考だとEffctManagerに設置
	Effekseer_Sync3DSetting();
}

void NormalCamera::Draw()
{
}

void NormalCamera::Exit()
{
}
