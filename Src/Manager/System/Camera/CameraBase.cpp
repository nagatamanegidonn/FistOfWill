#include <EffekseerForDXLib.h>
#include <ImGui/imgui.h>

#include "CameraBase.h"
#include "../../../Utility/Utility.h"

CameraBase::CameraBase(void)
	:
	ActorBase(),
	shakeFrame_(0),
	shakeCountFrame_(0),
	shakePower_(0.0f),
	pos_(Utility::VECTOR_ZERO),
	targetPos_(Utility::VECTOR_ZERO),
	cameraNear_(10.0f),
	cameraFar_(30000.0f),
	angles_(Utility::VECTOR_ZERO),
	cameraUp_(Utility::VECTOR_ZERO)
{
}

void CameraBase::SetBeforeDraw()
{
	// SetDrawScreen の直後にこれを呼ぶことで、リセットされた Near/Far を再設定する
	SetCameraNearFar(cameraNear_, cameraFar_);

	// ─── 参考コード準拠のカメラシェイク計算 ───
	VECTOR renderPos = pos_;
	VECTOR renderTargetPos = targetPos_;

	if (shakeCountFrame_ > 0)
	{
		--shakeCountFrame_;

		// 0～1の進行率
		float t = 1.0f - static_cast<float>(shakeCountFrame_) / shakeFrame_;

		// 2乗による滑らかな減衰
		float attenuation = 1.0f - t;
		attenuation = attenuation * attenuation;

		// 揺れの周期（100.0fの値を大きくすると細かく、小さくするとゆっくり揺れます）
		float wave = std::cos(t * 100.0f);

		// 揺れの強さ
		float power = shakePower_ * attenuation * wave;

		// ★ カメラの「真右（横軸）方向」のベクトルを計算して画面酔いを防止する
		VECTOR camFront = VNorm(VSub(targetPos_, pos_));       // 正面
		VECTOR camRight = VCross(camFront, Utility::DIR_U);     // 横（外積）

		// 横揺れベクトルを作成
		VECTOR shakeVec = VScale(camRight, power);

		// 描画用の一時座標にのみ加算
		renderPos = VAdd(renderPos, shakeVec);
		renderTargetPos = VAdd(renderTargetPos, shakeVec);
	}

	// カメラの位置と注視点をDXライブラリに反映（シェイク適用済み座標）
	DxLib::SetCameraPositionAndTarget_UpVecY(renderPos, renderTargetPos);

	// DXライブラリのカメラとEffekseerのカメラを同期する
	Effekseer_Sync3DSetting();
}

void CameraBase::ChangeCamera(std::shared_ptr<CameraBase> nextCamera)
{
	//状態変化
	m_nextCamera = move(nextCamera);
}

void CameraBase::CameraShake(int frame, float shakePower)
{
	// 新しい衝撃の方が大きい、または新しいリクエストがあれば上書き設定
	if (shakeCountFrame_ < frame)
	{
		shakeFrame_ = frame;
		shakeCountFrame_ = frame;
		shakePower_ = shakePower;
	}
}

void CameraBase::UpdateDebugImGui(void)
{
	return;

	// ウィンドウタイトル&開始処理
	ImGui::Begin("Camera");

	// 大きさ
	ImGui::Text("NearFar");
	ImGui::InputFloat("Near", &cameraNear_);
	ImGui::InputFloat("Far", &cameraFar_);

	// 位置
	ImGui::Text("position");
	// 構造体の先頭ポインタを渡し、xyzと連続したメモリ配置へアクセス
	ImGui::InputFloat3("Pos", &pos_.x);

	// 位置
	ImGui::Text("target");
	// 構造体の先頭ポインタを渡し、xyzと連続したメモリ配置へアクセス
	ImGui::InputFloat3("TargetPos", &targetPos_.x);

	// 位置
	ImGui::Text("distance");
	// 2. 注視点（プレイヤーの頭）から衝突点までの距離を計算
	VECTOR toHitVec = VSub(pos_, targetPos_);
	float hitDistance = VSize(toHitVec);
	// 構造体の先頭ポインタを渡し、xyzと連続したメモリ配置へアクセス
	ImGui::InputFloat("dis", &hitDistance);

	// 終了処理
	ImGui::End();

}