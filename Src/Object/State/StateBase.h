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
	virtual ~StateBase() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;

	//次の状態を取得
	std::weak_ptr<StateBase> GetNextState() { return m_nextState; };
	// 状態変化
	virtual void ChangeState(std::shared_ptr<StateBase> nextState);
private:
	// 次の状態
	std::shared_ptr<StateBase> m_nextState;
protected:

	STATE m_State;

	std::weak_ptr<ActorBase> owner_;
};