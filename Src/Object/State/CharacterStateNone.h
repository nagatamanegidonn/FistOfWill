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
    ~CharacterStateNone() override = default;

    void Init() override;
    void Update() override;
    void Exit() override;
};
