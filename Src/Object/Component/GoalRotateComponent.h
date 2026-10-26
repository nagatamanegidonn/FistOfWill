#pragma once
#include "ComponentBase.h"
#include "../../Utility/Utility.h"

/// <summary>
/// 移動方向をもとに、キャラクターの回転を設定するコンポーネント
/// </summary>
class GoalRotateComponent :
    public ComponentBase
{
public:

    // 回転完了までの時間
    static constexpr float TIME_ROT = 0.1f;

    // コンストラクタ
    GoalRotateComponent(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    virtual ~GoalRotateComponent(void);

    virtual void Init(void) override;
    virtual void Update(void) override;
    virtual void PostUpdate(void) override;
    virtual void Draw(void) override;

protected:

    VECTOR rotateDir_ = Utility::VECTOR_ZERO;	//回転方向

    void SetGoalRotate(double rotRad);

};

