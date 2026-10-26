#include <DxLib.h>
#include <fstream>
#include "../Lib/nlohmann/json.hpp"

#include "../Application.h"

#include "TutorialManager.h"
#include "../../../Utility/CSVDataLoader.h"

TutorialManager* TutorialManager::instance_ = nullptr;

void TutorialManager::StartNextTutorial(bool flag, float dleay)
{
	isNextTutorial_ = flag;
	waitTime_ = dleay;
}

TutorialManager::TutorialManager(void)
	:
	tutorialIndex_(0),
	isNextTutorial_(true),
	waitTime_(0.5f)
{
}

void TutorialManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new TutorialManager();
	}
	instance_->Init();
}

TutorialManager& TutorialManager::GetInstance(void)
{
	return *instance_;
}

void TutorialManager::Init(void)
{
	GameInit();
}

void TutorialManager::GameInit(void)
{
	tutorialIndex_ = 0;
	isNextTutorial_ = true;
	waitTime_ = 0.5f;
}

void TutorialManager::Update(void)
{
	if (waitTime_ > 0.0f)
	{
		waitTime_ -= 0.016f; // 仮のデルタタイム（例: 60FPSの場合）
	}
}

void TutorialManager::Destroy(void)
{
	// 自身のインスタンス削除
	delete instance_;
	instance_ = nullptr;
}

void TutorialManager::LoadTutorialTexts(const std::wstring& path)
{
	tutorialTexts_.clear();

	// 既存の CSVDataLoader から文字列データのリストを取得
	auto csvDatas = CSVDataLoader::GetInstance().LoadCSV(path.c_str());
	for (const auto& csvData : csvDatas)
	{
		int index = std::stoi(csvData->GetData(0));
		std::wstring text = csvData->GetData(1);

		tutorialTexts_.push_back(text);
	}
}

void TutorialManager::NextTutorial(void)
{
	tutorialIndex_++;
	isNextTutorial_ = false;
}
