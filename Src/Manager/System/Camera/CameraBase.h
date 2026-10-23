#pragma once
#include <memory>
#include <DxLib.h>
#include "../../../Common/Quaternion.h"
#include "../../../Object/ActorBase.h"

class CameraBase abstract:
	public ActorBase
{
public:

	// コンストラクタ
	CameraBase(void);
	// デストラクタ
	virtual ~CameraBase(void) = default;


	virtual void SetBeforeDraw();
	virtual void Exit() = 0;

	// カメラ位置
	VECTOR GetPos(void) const { return pos_; }
	// カメラの操作角度
	//VECTOR GetAngles(void) const { return pos_ }
	// カメラの注視点
	VECTOR GetTargetPos(void) const { return targetPos_; }

	//次の状態を取得
	std::weak_ptr<CameraBase> GetNextCamera() { return m_nextCamera; };

	// 状態変化
	virtual void ChangeCamera(std::shared_ptr<CameraBase> nextCamera);

	// angles_の取得
	const VECTOR GetAngles() const { return angles_; }

	/// @brief カメラシェイクを開始する
	/// @param frame 揺れる時間（フレーム数）
	/// @param shakePower 揺れの強さ
	void CameraShake(int frame, float shakePower);

	/// @brief カメラシェイクを強制的に停止・リセットする
	void ResetShake(void)
	{
		shakeFrame_ = 0;
		shakeCountFrame_ = 0;
		shakePower_ = 0.0f;
	}

	// Initが必要かどうかのフラグ制御
	void SetNeedInit(bool need) { m_needInit = need; }
	bool IsNeedInit() const { return m_needInit; }

private:
	// 次の状態
	std::shared_ptr<CameraBase> m_nextCamera;
	bool m_needInit = true; // デフォルトはtrue
protected:

	// GUI表示
	virtual void UpdateDebugImGui(void);

	////カメラ距離
	//float m_distance;
	////上下制限
	//float m_vertexAngle;
	////向き
	//VECTOR m_front;	 //前方向
	//VECTOR m_right;	 //右方向
	//VECTOR m_look;	 //注視方向
	////回転
	//Quaternion m_rotH;
	////位置
	//VECTOR m_viewPos; //注視点
	//VECTOR m_cameraPos; //カメラ位置

	// カメラシェイク用の変数
	int shakeFrame_ = 0;       // シェイクの総時間
	int shakeCountFrame_ = 0;  // シェイクの残り時間カウント
	float shakePower_ = 0.0f;  // シェイクの強さ

	// カメラの位置
	VECTOR pos_;
	// 注視点
	VECTOR targetPos_;

	float cameraNear_ = 10.0f;
	float cameraFar_ = 30000.0f;

	// カメラ角度(rad)
	VECTOR angles_;
	// X軸回転が無い角度
	Quaternion rotOutX_;
	// カメラ角度
	Quaternion rot_;
	// カメラの上方向
	VECTOR cameraUp_;

	//std::weak_ptr<CameraBase> owner_;
};

