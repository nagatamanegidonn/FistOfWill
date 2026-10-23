#include <DxLib.h>
#include "ShadowMap.h"
#include "../Manager/General/SceneManager.h"
#include "../Manager/System/Camera/CameraBase.h"

namespace
{
	//シャドウマップの描画サイズ
	constexpr int kShadowMapWidth = 4096;
	constexpr int kShadowMapHeight = 4096;

	//シャドウマップの範囲
	constexpr float kShadowMapHorizon = 1500.0f;
	constexpr float kShadowMapVertical = 1500.0f;
}

ShadowMap::ShadowMap(void)
{
}

ShadowMap::~ShadowMap(void)
{
	// シャドウマップの削除
	DeleteShadowMap(ShadowMapHandle);
}

void ShadowMap::Init(void)
{
	// シャドウマップハンドルの作成
	ShadowMapHandle = MakeShadowMap(kShadowMapWidth, kShadowMapHeight);

	// カメラの位置と向きを設定
	//SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 800.0f, -800.0f), VGet(0.000f, 500.000f, 0.000f));

	// 描画する奥行き方向の範囲を設定
	//SetCameraNearFar(40.000f, 2000.000f);

	// ライトの方向を設定
	//SetLightDirection(VGet(0.3f, -0.7f, 0.8f));

	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(ShadowMapHandle, VGet(0.3f, -0.7f, 0.8f));

	// シャドウマップに描画する範囲を設定
	SetShadowMapDrawArea(ShadowMapHandle, VGet(-kShadowMapHorizon, -kShadowMapHorizon, -kShadowMapHorizon),
		VGet(kShadowMapVertical, kShadowMapVertical, kShadowMapVertical));
}

void ShadowMap::DrawStartSetUp(void) const
{
	//描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
	//ライトの向き
	SetLightDirection(VGet(0.3f, -0.7f, 0.8f));
	//シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(ShadowMapHandle, VGet(0.3f, -0.7f, 0.8f));
	//シャドウマップ位置を計算(カメラ基準)
	VECTOR cameraPos = SceneManager::GetInstance().GetCameraBase().lock()->GetPos();
	//最も低い位置を出す
	VECTOR minPos = cameraPos;
	minPos.x -= kShadowMapHorizon;
	minPos.z -= kShadowMapHorizon;
	minPos.y -= kShadowMapVertical;
	//最も高い位置
	VECTOR maxPos = cameraPos;
	maxPos.x += kShadowMapHorizon;
	maxPos.z += kShadowMapHorizon;
	maxPos.y += kShadowMapVertical;
	SetShadowMapDrawArea(ShadowMapHandle, minPos, maxPos);


	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(ShadowMapHandle);
}

void ShadowMap::DrawSetUp(void) const
{

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, ShadowMapHandle);
}

void ShadowMap::DrawEndSetUp(void)
{
	// 描画に使用するシャドウマップの設定を解除
	//SetUseShadowMap(0, -1);
}
