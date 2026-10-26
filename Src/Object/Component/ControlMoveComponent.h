#pragma once
#include <memory>
#include "ComponentBase.h"

class ActorBase;
class InputController;

// 操作移動コンポーネント
class ControlMoveComponent :
    public ComponentBase
{
public:

	static constexpr float SPEED = 12.0f;// 移動速度

    // コンストラクタ
    ControlMoveComponent(std::weak_ptr<ActorBase> _actor, std::shared_ptr <InputController> _input);
    // デストラクタ
    virtual ~ControlMoveComponent(void);

    virtual void Init(void) override;
    virtual void Update(void) override;
    virtual void PostUpdate(void) override;
	virtual void Draw(void) override;

protected:

    //コンポーネントで操作するアクターベース
    std::weak_ptr<InputController> input_;

    void Move(void);
    void Lerp(void);
    void HitMove(void);

};

