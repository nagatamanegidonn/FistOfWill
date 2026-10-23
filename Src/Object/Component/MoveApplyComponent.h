#pragma once
#include "ComponentBase.h"

// 移動適用コンポーネント
class MoveApplyComponent :
    public ComponentBase
{
public:

    // 回転完了までの時間
    static constexpr float TIME_ROT = 0.1f;

    // コンストラクタ
    MoveApplyComponent(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    virtual ~MoveApplyComponent(void);

    virtual void Init(void) override;
    virtual void Update(void) override;
    virtual void PostUpdate(void) override;
    virtual void Draw(void) override;

protected:

    void SetGoalRotate(double rotRad);
};


