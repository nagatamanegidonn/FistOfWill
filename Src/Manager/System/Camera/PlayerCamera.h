#pragma once
#include "CameraBase.h"

class PlayerBase;
class Geometry;

class PlayerCamera :
    public CameraBase,
	public std::enable_shared_from_this<PlayerCamera>
{
public:

	// コンストラクタ
	PlayerCamera(std::shared_ptr<PlayerBase> player);
	// デストラクタ
	~PlayerCamera(void) override = default;

	void Init() override;
	void Update() override;
	void Draw() override;
	//void SetBeforeDraw() override;
	void Exit() override;

private:

	void UpdateFollow(void);	// プレイヤーを追従する更新処理

	void OnHit(const std::weak_ptr<Collider> _hitCol) override;	// 当たり判定後の処理

	// プレイヤーの参照用
	std::weak_ptr<PlayerBase> player_;

	// 判定の参照
	Geometry* rayGeo_;

	float targetRadius_ = 300.0f; // プレイヤーが本来保ちたいカメラの最大距離
	float cameraRadius_; // 注視点からの距離
	float lerpTime_ = 0.0f; // カメラの移動にかける時間

};

