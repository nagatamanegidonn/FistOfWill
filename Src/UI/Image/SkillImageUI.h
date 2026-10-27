#pragma once
#include "RoundRateImageUI.h"

class CharaStatus;

class SkillImageUI :
    public RoundRateImageUI
{
public:

    // コンストラクタ
    SkillImageUI(std::shared_ptr<CharaStatus> status, Vector2 pos, const std::wstring& resourceKey = L"");
    // デストラクタ
    virtual ~SkillImageUI(void) = default;

    // 更新処理
    void Update(void) override;

private:

    // プレイヤーのステータス
    std::weak_ptr<CharaStatus> m_charaStatus;

};