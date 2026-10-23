#pragma once
#include "ComponentBase.h"

// 重力適用コンポーネント
class GravityComponent :
    public ComponentBase
{
public:

	static constexpr float DEFAULT_GRAVITY_POW = 5.0f; // 重力の強さの初期値

    // コンストラクタ
    GravityComponent(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    virtual ~GravityComponent(void);

    virtual void Init(void) override;
    virtual void Update(void) override;
    virtual void PostUpdate(void) override;
    virtual void Draw(void) override;

protected:

	VECTOR gravityPow_; // 重力の強さ
};

