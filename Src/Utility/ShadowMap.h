#pragma once

class ShadowMap
{

public:

	// コンストラクタ
	ShadowMap(void);

	// デストラクタ
	~ShadowMap(void);

	// 初期化処理
	void Init(void);

	// 描画処理
	void DrawStartSetUp(void) const;
	void DrawSetUp(void) const;
	void DrawEndSetUp(void);

	int GetSyadowHandle(void) const { return ShadowMapHandle; }

private:

	int ShadowMapHandle;

};

