#pragma once
#include <memory>
#include <DxLib.h>

#include "../Lib/nlohmann/json.hpp"

// 長いのでnamespaceの省略
using json = nlohmann::json;

class TutorialManager
{

public:

	// ゲーム結果
	enum class GAME_RESULT
	{
		NONE,
		GAME_CLEAR,
		GAME_OVER,
		TIME_OVER,
		DRAW
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static TutorialManager& GetInstance(void);

	void Init(void);
	void GameInit(void);
	void Update(void);
	void Destroy(void);

	void LoadTutorialTexts(const std::wstring& path);

	// 次のチュートリアルに備える
	void NextTutorial(void);

	// チュートリアルテキストの取得
	std::vector<std::wstring> GetTutorialTexts(void) const { return tutorialTexts_; }
	std::wstring GetCurrentTutorialText(void) const
	{
		if (tutorialIndex_ < 0 || tutorialIndex_ >= static_cast<int>(tutorialTexts_.size()))
		{
			return L""; // インデックスが範囲外の場合は空文字を返す
		}
		return tutorialTexts_[tutorialIndex_];
	}

	int GetCurrentTutorialIndex(void) const { return tutorialIndex_; }

	// 次のチュートリアルに進むかどうかのフラグ
	bool IsNextTutorial(void) const { return waitTime_ <= 0.0f && isNextTutorial_; }
	void StartNextTutorial(bool flag, float dleay = 0.1f);

	// tutorialIndex_がtutorialTexts_の範囲内かどうかをチェックする関数
	bool IsTutorialIndexValid() const
	{
		return tutorialIndex_ >= 0 && tutorialIndex_ < static_cast<int>(tutorialTexts_.size());
	}

private:

	// 静的インスタンス
	static TutorialManager* instance_;
	
	// コンストラクタ
	TutorialManager(void);

	// コピーの禁止
	TutorialManager(const TutorialManager& _copy) = delete;
	TutorialManager& operator= (const TutorialManager& _copy) = delete;

	// デストラクタ
	~TutorialManager(void) = default;

	int tutorialIndex_;
	std::vector<std::wstring> tutorialTexts_;

	bool isNextTutorial_;

	float waitTime_; // 次のチュートリアルに進むまでの待機時間
};