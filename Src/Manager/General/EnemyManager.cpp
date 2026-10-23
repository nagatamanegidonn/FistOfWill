
#include "SceneManager.h"
#include "../Manager/Resource/SoundManager.h"
#include "../../Object/ActorBase.h"
#include "../../Object/Player/PlayerBase.h"
#include "../../Object/Enemy/EnemyBase.h"
#include "../../Object/Enemy/DemoEnemy.h"
#include "ActorManager.h"

#include "EnemyManager.h"


EnemyManager::EnemyManager(std::shared_ptr<ActorManager> actorM)
	:
	m_pActorManager(actorM)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Entry(std::shared_ptr<EnemyBase> _enemy)
{
	// std::find の代わりに std::find_if を使用
	// すでに登録されているならしない
	auto it = std::find_if(m_enemys.begin(), m_enemys.end(),
		[&_enemy](const std::weak_ptr<EnemyBase>& wp) {
			// lock() して shared_ptr に変換してから比較する
			return wp.lock() == _enemy;
		});

	if (it != m_enemys.end()) return;

	// 敵を追加
	m_enemys.emplace_back(_enemy); // shared_ptr から weak_ptr へは自動変換されます
}

void EnemyManager::Exit(std::shared_ptr<EnemyBase> _enemy)
{
	
}

void EnemyManager::Init()
{

}

void EnemyManager::Update()
{
	// ゲーム終了処理（仮）
	if (m_enemys.size() <= 0)
	{
		//SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// 寿命が切れた（すでに削除された）エネミーをリストから完全に掃除する
	std::erase_if(m_enemys, [](const std::weak_ptr<EnemyBase>& wp)
		{
			return wp.expired();
		});

	if (m_enemys.empty()) return;

	// ActorManagerが生存しているかチェック
	if (m_pActorManager.expired()) return;
	auto actorManager = m_pActorManager.lock();


	// 現在いるプレイヤーのリストを1回だけ取得
	const auto& players = actorManager->GetPlayers();

	// ループ内で安全に処理を行う
	for (auto it = m_enemys.begin(); it != m_enemys.end();)
	{
		// 安全に shared_ptr に変換
		auto enemy = it->lock();
		if (!enemy)
		{
			// 万が一、このループの瞬間に消えたら安全に消去して次へ
			it = m_enemys.erase(it);
			continue;
		}

		// --- ここに enemy を使った更新処理を書く ---
		// 距離判定など

		std::shared_ptr<PlayerBase> closestPlayer = nullptr;
		float minDistance = FLT_MAX;

		// 一番距離の小さいプレイヤーを選定する
		for (const auto& player : players)
		{
			float dist = VSize(VSub(player->GetTransform().pos, enemy->GetTransform().pos));
			if (dist < minDistance)
			{
				minDistance = dist;
				closestPlayer = player;
			}
		}

		// 敵に「お前に一番近いのはこのプレイヤーだよ」と教えてあげる
		enemy->SetClosestTarget(closestPlayer);

		++it; // 次の要素へ
	}
}

void EnemyManager::End()
{
	// weak_ptr の場合は、ここで無理に Exit を呼ばなくても
	// ActorManager 側が解放されれば自動的に expired になりますが、
	// 明示的にクリアしたい場合は clear でOKです。
	m_enemys.clear();
}

void EnemyManager::InAreaBattle(std::list<std::shared_ptr<EnemyBase>> activeEnemy)
{
}

void EnemyManager::FinishInAreaBattle()
{
}

bool EnemyManager::IsAllDeadEnemys() const
{
	// 寿命が残っている要素が一つもない = 全滅
	return std::all_of(m_enemys.begin(), m_enemys.end(),
		[](const std::weak_ptr<EnemyBase>& wp) {
			return wp.expired();
		});
}

void EnemyManager::DebugDraw() const
{
}


