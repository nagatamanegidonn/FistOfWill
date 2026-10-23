
#include "UIManager.h"
#include <DxLib.h>
#include <cassert>
#include <vector>
#include "UIBase.h"

void UIManager::Entry(std::shared_ptr<UIBase> ui)
{
	//すでに登録されているならしない
	auto it = std::find(m_uis.begin(), m_uis.end(), ui);
	if (it != m_uis.end())return;
	//UIの追加
	m_uis.emplace_back(ui);
}

void UIManager::Exit(std::shared_ptr<UIBase> ui)
{
	//登録されていないならしない
	auto it = std::find(m_uis.begin(), m_uis.end(), ui);
	if (it == m_uis.end())return;
	m_uis.erase(it);
}

void UIManager::Update()
{
	if (!m_isDraw)return;

	//UIの更新
	for (auto& ui : m_uis)
	{
		ui->Update();
	}

	//削除
	CheckDelete();
}

void UIManager::FrontDraw() const
{

	if (!m_isDraw)return;

	//描画
	for (auto& ui : m_uis)
	{
		if (!ui->IsFrontDraw())continue;
		ui->Draw();
	}
}

void UIManager::BackDraw() const
{
	if (!m_isDraw)return;

	//描画
	for (auto& ui : m_uis)
	{
		if (ui->IsFrontDraw())continue;
		ui->Draw();
	}
}

void UIManager::AllDeleteUI()
{
	//一気に登録解除
	m_uis.clear();
}

//消滅フラグをチェックして削除
void UIManager::CheckDelete()
{
	// 削除対象のUIを一時的に抽出する
	std::vector<std::shared_ptr<UIBase>> toDelete;
	for (const auto& ui : m_uis)
	{
		if (ui == nullptr || ui->IsDelete())
		{
			toDelete.push_back(ui);
		}
	}

	// 抽出したUIに対して終了処理を実行
	for (auto& ui : toDelete)
	{
		if (ui)
		{
			ui->End();
		}
	}

	// m_uis から対象要素を安全に削除する
	std::erase_if(m_uis, [](const std::shared_ptr<UIBase>& ui)
		{
			return ui == nullptr || ui->IsDelete();
		});
}