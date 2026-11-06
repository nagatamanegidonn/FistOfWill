#pragma once
#include <memory>

class PixelRenderer;
class PixelMaterial;

class Fader
{

public:

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5.0f;

	// 状態
	enum class NET_STATE
	{
		NONE,
		FADE_OUT,	// 徐々に暗転
		FADE_IN,	// 徐々に明転
		FADE_STOP,
	};
	
	// コンストラクタ
	Fader(void);

	// デストラクタ
	~Fader(void);

	// 状態の取得
	NET_STATE GetState(void) const;

	// フェード処理が終了しているか
	bool IsEnd(void) const;

	// 指定フェードを開始する
	void SetFade(NET_STATE state);

	void Init(void);
	void Update(void);
	void Draw(void);

private:

	// マテリアル
	std::unique_ptr<PixelMaterial> material_;

	// レンダラー
	std::unique_ptr<PixelRenderer> renderer_;

	// フェード最大値
	static constexpr float MAX_ALPHA = 255.0f;

	// フェード画像
	int fadeImage_;

	// 透明度
	float alpha_;

	// 状態(STATE)を保ったまま終了判定を行うため、
	// Update->Draw->Updateの1フレーム判定用
	bool isPreEnd_;

	// フェード処理の終了判定
	bool isEnd_;

	// 状態
	NET_STATE state_;

};
