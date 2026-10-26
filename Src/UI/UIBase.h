
#pragma once
#include <memory>

class UIBase abstract :
	public std::enable_shared_from_this<UIBase>
{
public:

	// コンストラクタ
	UIBase();
	// デストラクタ
	~UIBase();

	//初期化
	virtual void Init();
	//終了
	virtual void End();
	//更新
	virtual void Update() abstract;
	//描画
	virtual void Draw()const abstract;
	//描画順
	bool IsFrontDraw()const { return m_isFrontDraw; };
	void SetFrontDraw(bool value) { m_isFrontDraw = value; }
	//削除
	bool IsDelete()const { return m_isDelete; };
	void Delete() { m_isDelete = true; };
	//描画しているか
	bool IsDraw()const { return m_isDraw; };
	// 描画を有効にする
	virtual void EnableDraw() { m_isDraw = true; };
	// 描画を無効にする
	virtual void DisableDraw() { m_isDraw = false; };

protected:

	//描画順
	bool m_isFrontDraw;
	//削除
	bool m_isDelete;
	//描画するか
	bool m_isDraw;
};

