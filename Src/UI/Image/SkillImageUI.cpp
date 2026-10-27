#include "SkillImageUI.h"
#include "../Object/CharaStatus.h"
#include <DxLib.h>
#include <algorithm>

SkillImageUI::SkillImageUI(std::shared_ptr<CharaStatus> status, Vector2 pos, const std::wstring& resourceKey)
	:
	RoundRateImageUI(pos,resourceKey),
	m_charaStatus(status)
{
}

void SkillImageUI::Update()
{
	ImageUI::Update();

	if (m_charaStatus.expired()) return;
	rate_ = m_charaStatus.lock()->GetSkillTime();
}
