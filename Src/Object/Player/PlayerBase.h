#pragma once
#include "../CharaBase.h"
#include <memory>

class InputController;

class StateBase;

class PlayerBase :
	public CharaBase
{
public:

	// コンストラクタ
	PlayerBase(int key);
	// デストラクタ
	virtual ~PlayerBase(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;


	// プレイヤーの入力を取得する関数
	std::weak_ptr<InputController> GetInputController(void) { return inputController_; }

protected:

	// 操作コントローラー
	std::shared_ptr<InputController> inputController_;
};

