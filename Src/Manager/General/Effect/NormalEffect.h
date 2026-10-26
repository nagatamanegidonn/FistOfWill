#pragma once
//#include "../Math/MyMath.h"
#include <DxLib.h>

class NormalEffect
{
public:

	// 
	NormalEffect(int playHandle, VECTOR pos);
	// 
	virtual ~NormalEffect();

	virtual void Update();			// 更新
	virtual void End();				// 終了処理

	virtual void Play();			// 再生
	virtual void Stop();			// 停止

	void EnableDraw() { m_isDraw = true; };		// 描画する
	void DisableDraw() { m_isDraw = false; };	// 描画しない

	void SetPlaySpeed(float speed);		// 再生速度

	virtual void Delete();				// 削除
	bool IsDelete() const { return m_isDelete; };
	bool IsDraw() const { return m_isDraw; };
	int GetPlayHandle() const { return m_playHandle; };

	// セッター
	virtual void SetPos(VECTOR pos);		// 座標
	virtual void SetRot(VECTOR rot);		// 回転
	virtual void SetScale(VECTOR scale);	// 拡大
	virtual void LookAt(VECTOR dir);		// 向きたい方向

	// ゲッター
	VECTOR GetPos()const { return m_pos; };
	VECTOR GetRot()const { return m_rot; };
	VECTOR GetScale()const { return m_scale; };

	// 指定フレーム再生
	void SpecificFrame(float frame);

	// タイムスケール
	void SetTimeScale(float scale);

	// 自分のタイムスケールを使う
	void EnableIsMyScale();

	// 使わない
	void DisableIsMyScale();

protected:

	// 再生ハンドル
	int m_playHandle;

	// 座標
	VECTOR m_pos;
	// 回転
	VECTOR m_rot;
	// 拡大
	VECTOR m_scale;

	// 再生速度
	float m_playSpeed;
	// 削除フラグ
	bool m_isDelete;
	// 指定フレーム後に削除
	float m_specificFrame;
	bool m_isSpecificFrame;

	// エフェクトの向き(Z方向)
	VECTOR m_dir;

	// タイムスケール
	float m_myTimeScale;
	bool m_isMyTimeScale;

	// 描画
	bool m_isDraw;

};

