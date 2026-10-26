#include <DxLib.h>
#include <memory>
#include "../../Manager/Resource/ResourceManager.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Utility/Utility.h"

#include "../Common/Transform.h"
#include "Grid.h"
#include "../Common/Collider/Geometry/Geometry.h"
#include "../Common/Collider/Geometry/Cube.h"

namespace MyNamespace
{
	// もう一つのcubeの座標	
	constexpr VECTOR OTHER_CUBE_POS = { 200.0f, -150.0f, 100.0f };
	constexpr VECTOR OTHER_CUBE_POS2 = { 0.0f, -150.0f, -200.0f };
	constexpr VECTOR OTHER_CUBE_POS3 = { -180.0f, 0.0f, 230.0f };
	constexpr VECTOR OTHER_CUBE_POS4 = { 10.0f, -100.0f, 230.0f };

	constexpr VECTOR OTHER_CUBE_SIZE = { 100.0f, 100.0f, 100.0f };
	constexpr VECTOR OTHER_CUBE_SIZE2 = { 1000.0f, 10.0f, 1000.0f };

	const Quaternion OTHER_CUBE_ROT = Quaternion();
	const Quaternion OTHER_CUBE_ROT2 =
		Quaternion::Euler({ 0.0f, 0.0f, Utility::Deg2RadF(45.0f) });
}

Grid::Grid(void)
{
}

Grid::~Grid(void)
{
}

void Grid::Init(void)
{
	// ※モデルのテクスチャの参照先がおかしいの白く描画されます
	transform_ = Transform();
	transform_.pos.y = -100.0f;
	transform_.Update();

	return;

	// 当たり判定生成
	// 修正後：第3引数に Quaternion() を追加して引数を4個にする
	std::unique_ptr<Geometry> geo = std::make_unique<Cube>(
		transform_.pos,					// 現在位置
		transform_.pos,					// 移動後位置（静止物なら同じ値）
		transform_.quaRot,				// 回転（これが必要！）
		MyNamespace::OTHER_CUBE_SIZE	// ハーフサイズ（VECTORで渡す）
	);
	this->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });

	geo = std::make_unique<Cube>(
		MyNamespace::OTHER_CUBE_POS,           // 現在位置
		MyNamespace::OTHER_CUBE_POS,           // 移動後位置（静止物なら同じ値）
		MyNamespace::OTHER_CUBE_ROT,           // 回転（これが必要！）
		MyNamespace::OTHER_CUBE_SIZE		   // ハーフサイズ（VECTORで渡す）
	);
	this->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });

	geo = std::make_unique<Cube>(
		MyNamespace::OTHER_CUBE_POS2,           // 現在位置
		MyNamespace::OTHER_CUBE_POS2,           // 移動後位置（静止物なら同じ値）
		MyNamespace::OTHER_CUBE_ROT2,           // 回転（これが必要！）
		MyNamespace::OTHER_CUBE_SIZE			// ハーフサイズ（VECTORで渡す）
	);
	this->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });


	geo = std::make_unique<Cube>(
		MyNamespace::OTHER_CUBE_POS3,           // 現在位置
		MyNamespace::OTHER_CUBE_POS3,           // 移動後位置（静止物なら同じ値）
		MyNamespace::OTHER_CUBE_ROT2,           // 回転（これが必要！）
		MyNamespace::OTHER_CUBE_SIZE			// ハーフサイズ（VECTORで渡す）
	);
	this->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });


	geo = std::make_unique<Cube>(
		MyNamespace::OTHER_CUBE_POS4,           // 現在位置
		MyNamespace::OTHER_CUBE_POS4,           // 移動後位置（静止物なら同じ値）
		MyNamespace::OTHER_CUBE_ROT,			// 回転（これが必要！）
		MyNamespace::OTHER_CUBE_SIZE			// ハーフサイズ（VECTORで渡す）
	);
	this->MakeCollider(Collider::TAG::GROUND, std::move(geo), { Collider::TAG::GROUND });

}

void Grid::Update(void)
{
	transform_.Update();
}

void Grid::Draw(void)
{
	//DrawDebug();
}
void Grid::DrawDebug(void)
{
	// 【練習】最初の１本
	//VECTOR sPos = { 0.0f, 0.0f, 0.0f };
	//VECTOR ePos = { HLEN, 0.0f, 0.0f };
	//VECTOR sPos = { -HLEN, 0.0f, 0.0f };
	//VECTOR ePos = {  HLEN, 0.0f, 0.0f };
	//DrawLine3D(sPos, ePos, 0xff0000);

	// XZ基本軸(グリッド)
	float num;

	//今回回転させたい回転量をクォータニオンで作る
	Quaternion rotPow = Quaternion();

	float dirPow = 360.0f / (HNUM * 2);
	VECTOR dir = Utility::DIR_R;

	VECTOR sPos = Utility::VECTOR_ZERO;
	VECTOR ePos;
	VECTOR eAgoPos = Utility::VECTOR_ZERO;

	DrawSphere3D(sPos, 20.0f, 10, 0xff0000, 0xff0000, true);


	
	//□
	for (int i = -HNUM; i < HNUM; i++)
	{

		num = static_cast<float>(i);

		// X軸(赤)
		sPos = { -HLEN, -50.0f, num * TERM };
		ePos = { HLEN, -50.0f, num * TERM };
		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, 20.0f, 10, 0xff0000, 0xff0000, true);

		// Z軸(青)
		sPos = { num * TERM, -50.0f, -HLEN };
		ePos = { num * TERM, -50.0f,  HLEN };
		DrawLine3D(sPos, ePos, 0x0000ff);
		DrawSphere3D(ePos, 20.0f, 10, 0x0000ff, 0x0000ff, true);

	}

	// Y軸(緑)
	sPos = { 0.0f, -HLEN, 0.0f };
	ePos = { 0.0f,  HLEN, 0.0f };
	DrawLine3D(sPos, ePos, 0x00ff00);
	DrawSphere3D(ePos, 20.0f, 10, 0x00ff00, 0x00ff00, true);

	return;


	//〇
	for (int i = -HNUM; i < HNUM; i++)
	{

		float rad = Utility::Deg2RadF(dir.y);
		float cosA = cosf(rad);
		float sinA = sinf(rad);

		// Y軸回転行列を使って新しい座標を計算
		float newX = dir.x * cosA + dir.z * sinA;
		float newZ = -dir.x * sinA + dir.z * cosA;

		ePos = { newX, 0.0f, newZ };
		ePos = VScale(ePos, STAGE_RADIUS);

		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, 20.0f, 10, 0x0000ff, 0x0000ff, true);

		if (!Utility::EqualsVZero(eAgoPos))
		{
			DrawLine3D(ePos, eAgoPos, 0x0000ff);
		}
		eAgoPos = ePos;

		dir.y += dirPow;
	}

	// Y軸(緑)
	sPos = { 0.0f, -HLEN, 0.0f };
	ePos = { 0.0f,  HLEN, 0.0f };
	DrawLine3D(sPos, ePos, 0x00ff00);
	DrawSphere3D(ePos, 20.0f, 10, 0x00ff00, 0x00ff00, true);


	return;

}

void Grid::Release(void)
{
}

void Grid::OnHit(const std::weak_ptr<Collider> _hitCol)
{
}
