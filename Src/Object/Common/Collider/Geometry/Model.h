#pragma once

#include"Geometry.h"

class Model : public Geometry
{
public:

	/// @brief コンストラクタ
	/// @param _pos 追従する親の座標
	/// @param _rot 追従する親の回転
	/// @param _modelId 追従する親のモデルID
	Model(const VECTOR& _pos, const VECTOR& _movedPos, const Quaternion& _rot, const int _modelId);

	/// @brief コピーコンストラクタ
	/// @param _copyBase コピー元
	Model(const Model& _copyBase);

	//デストラクタ
	~Model(void)override;

	//描画
	void Draw(const int _color = NORMAL_COLOR)override;

	//各種当たり判定
	const bool IsHit(Geometry& _geometry)override;
	const bool IsHit(Model& _model)override;
	const bool IsHit(Cube& _cube)override;
	const bool IsHit(Sphere& _sphere)override;
	const bool IsHit(Capsule& _capsule)override;
	const bool IsHit(Line& _line) override;

	//ヒット後の処理
	void HitAfter(void)override;

	//親モデルIDの取得
	inline const int GetParentModel(void)const { return parentModelId_; };

	//当たった時の情報取得
	inline const MV1_COLL_RESULT_POLY& GetHitLineInfo(void)const { return hitLineInfo_; }

	//当たった時の情報取得
	inline const MV1_COLL_RESULT_POLY_DIM& GetHitInfo(void)const { return hitInfo_; }

	//当たった時の情報設定
	inline void SetHitLineInfo(const MV1_COLL_RESULT_POLY _hitInfo) { hitLineInfo_ = _hitInfo; }

	//当たった時の情報設定
	//inline void SetHitInfo(MV1_COLL_RESULT_POLY_DIM _hitInfo) { std::swap(hitInfo_, _hitInfo); }
	void SetHitInfo(const MV1_COLL_RESULT_POLY_DIM& _hitInfo)
	{
		// 安全弁：本当に解放が必要なときだけTerminateを呼ぶ
		if (hitInfo_.HitNum > 0 && hitInfo_.Dim != nullptr)
		{
			MV1CollResultPolyDimTerminate(hitInfo_);
		}
		// 新しいデータをコピー
		hitInfo_ = _hitInfo;
	}
	void CalculateHitResult(const VECTOR& myPos)
	{
		if (hitInfo_.HitNum <= 0 || hitInfo_.Dim == nullptr) return;

		// 一番プレイヤーを押し戻すべきポリゴン（一番深くめり込んでいるポリゴン）を探す
		float maxDepth = -1.0f;
		VECTOR bestNormal = VGet(0, 1, 0); // デフォルトは上向き

		for (int i = 0; i < hitInfo_.HitNum; ++i)
		{
			// 衝突したポリゴンの法線
			VECTOR polyNormal = hitInfo_.Dim[i].Normal;

			// 簡易的なめり込み深さの計算（本来はコライダーの形状に合わせますが、
			// 一番手前の頂点とコライダー中心の距離などから概算、または仕様に合わせて調整）
			// ここではDxLibが計算した外積などをベースにするか、当たった事実から固定値を仮設定
			float currentDepth = 0.1f; // 暫定の押し戻し量（テスト用）

			if (currentDepth > maxDepth)
			{
				maxDepth = currentDepth;
				bestNormal = polyNormal;
			}
		}

		// 計算結果を hitResult_ に格納する
		hitResult_.normal = bestNormal;
		hitResult_.depth = maxDepth;
	}

private:

	int parentModelId_;					//親のモデルID
	MV1_COLL_RESULT_POLY hitLineInfo_;	//当たった時の情報(線)
	MV1_COLL_RESULT_POLY_DIM hitInfo_;	//当たった時の情報(球、カプセル)
};
