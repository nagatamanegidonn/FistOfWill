
#pragma once
#include <memory>

class UIBase abstract :
	public std::enable_shared_from_this<UIBase>
{
public:

	// コンストラクタ
	UIBase(void);
	// デストラクタ
	~UIBase(void);

	//初期化
	virtual void Init(void);
	//終了
	virtual void End(void);
	//更新
	virtual void Update(void) abstract;
	//描画
	virtual void Draw(void) const abstract;
	//描画順
	bool IsFrontDraw(void) const { return m_isFrontDraw; };
	void SetFrontDraw(bool value) { m_isFrontDraw = value; }
	//削除
	bool IsDelete(void) const { return m_isDelete; };
	void Delete(void) { m_isDelete = true; };
	//描画しているか
	bool IsDraw(void) const { return m_isDraw; };
	// 描画を有効にする
	virtual void EnableDraw(void) { m_isDraw = true; };
	// 描画を無効にする
	virtual void DisableDraw(void) { m_isDraw = false; };

protected:

	//描画順
	bool m_isFrontDraw;
	//削除
	bool m_isDelete;
	//描画するか
	bool m_isDraw;
};

