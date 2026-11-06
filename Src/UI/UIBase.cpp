
#include "UIBase.h"
#include "UIManager.h"

UIBase::UIBase(void)
	:
	m_isFrontDraw(false),
	m_isDelete(false),
	m_isDraw(true)
{
}

UIBase::~UIBase(void)
{
}

void UIBase::Init(void)
{
	//UIManager‚É“o˜^
	UIManager::GetInstance().Entry(shared_from_this());
}

void UIBase::End(void)
{
	//UIManager‚©‚ç“o˜^‰ğœ
	UIManager::GetInstance().Exit(shared_from_this());
}