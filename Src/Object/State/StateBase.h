#pragma once
#include<memory>

class ActorBase;

class StateBase abstract
{
public:

	// 派生クラスで使用する状態列挙型
	// string型での管理もあり
	enum class STATE
	{
		IDLE,
		WALK,
		RUN,
		STEP,
		JUMP,
		FAll,
		ATTACK,
		DAMAGE,
		DEAD,
	};

	// コンストラクタ
	StateBase(std::weak_ptr<ActorBase> _actor);
	// デストラクタ
	virtual ~StateBase(void) = default;

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Exit(void) = 0;

	//次の状態を取得
	std::weak_ptr<StateBase> GetNextState() { return m_nextState; };
	// 状態変化
	virtual void ChangeState(std::shared_ptr<StateBase> nextState);
private:
	// 次の状態
	std::shared_ptr<StateBase> m_nextState;
protected:

	// 状態
	STATE m_State;

	// 持ち主
	std::weak_ptr<ActorBase> owner_;
};