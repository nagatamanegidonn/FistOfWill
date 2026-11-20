#pragma once
#include "CharacterStateBase.h"
class CharacterStateNone :
    public CharacterStateBase,
    public std::enable_shared_from_this<CharacterStateNone>
{
public:

    // コンストラクタ
    CharacterStateNone(std::weak_ptr<ActorBase> _actor);
    // デストラクタ
    ~CharacterStateNone(void) override = default;

    void Init(void) override;
    void Update(void) override;
    void Exit(void) override;
};
