#pragma once
#include <map>
#include <vector>


class InputController
{
public:
	enum class KEY
	{
		OK,
		OK_SECOND,
		OK_MOUSE,
		START,

		A_KEY,
		B_KEY,
		C_KEY,
		D_KEY,
		// 左スティック
		FORWARD,
		BACK,
		RIGHT,
		LEFT,
		// 右スティック
		R_FORWARD,
		R_BACK,
		R_RIGHT,
		R_LEFT,
		// アクション
		DRAW,
		CLOSE,
		
		ATTACK,
		SKILL,
		STEP,
		JUMP,

		DASH,
		ROLL,
		MOVE,	// 移動
	};

	enum class PeripheralType {
		KEYBOARD,		// キーボード
		GAME_PAD,		// ゲームパッド
		GAME_PAD_INS,	// INputManager用のTyoe
		GAME_PAD_STICK,	// INputManager用のTyoe
		MOUSE,			// マウス
	};

	InputController(int padType);
	~InputController(void);

	//更新
	void Update(void);
	void DebugDraw(void);

	bool IsTriggered(const KEY key)const;
	bool IsNew(const KEY key)const;

	// 外部から左スティックの値を安全に取得するための関数を追加
	float GetLStickX()const;
	float GetLStickY()const;

	// 外部から右スティックの値を安全に取得するための関数を追加
	float GetRStickX() const;
	float GetRStickY() const;

	// 最後に入力があった周辺機器の種類を取得する関数
	bool IsPeripheralTriggered(const PeripheralType type) const;

private:
	struct InputState
	{
		PeripheralType type;//周辺機器種別
		uint32_t code;			//入力コード（汎用）
	};

	using InputTable_t = std::map<KEY, std::vector<InputState>>;
	InputTable_t inputTable_;	//機器ごとによる判定を多なうためのkey

	using InputData_t = std::map<KEY, bool>;//機器ごとによる判定
	InputData_t currentInput_;	//最新の
	InputData_t lastInput_;		//直前の

	int padType_;

	bool IsStick(uint32_t value)const;
};

