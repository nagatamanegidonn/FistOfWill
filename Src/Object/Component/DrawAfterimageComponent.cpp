#include "DrawAfterimageComponent.h"

#include "../ActorBase.h"
#include "../CharaBase.h"
#include "../Common/AnimationController.h"

#include "../../Utility/Utility.h"
#include "../../Manager/General/SceneManager.h"
#include "../../Manager/System/InputManager.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"
#include "../../Utility/ModelFrameUtility.h"

DrawAfterimageComponent::DrawAfterimageComponent(std::weak_ptr<ActorBase> _actor)
	:
	ComponentBase()
{
	actor_ = _actor;
}

DrawAfterimageComponent::~DrawAfterimageComponent()
{
	// 複製した残像専用モデルをすべて解放
	for (int modelId : afterimageModels_)
	{
		if (modelId != -1)
		{
			MV1DeleteModel(modelId);
		}
	}
	afterimageModels_.clear();
	renderers_.clear();
	ghostHistory_.clear();
}

void DrawAfterimageComponent::Init()
{
	auto chara = GetChara();
	if (!chara) return;

	material_ = std::make_unique<ModelMaterial>(L"AfterimageVS.cso", 0, L"AfterimagePS.cso", 4);

	int texHandle = MV1GetTextureGraphHandle(chara->GetTransform().modelId, 0);
	material_->SetTextureBuf(0, texHandle);

	material_->AddConstBufPS({ 0.2f,0.8f,1.0f,1.0f }); // g_color
	material_->AddConstBufPS({ 0.0f,0.0f,0.0f,0.0f }); // g_light_dir + g_time
	material_->AddConstBufPS({ 1.0f,1.0f,1.0f,1.0f }); // g_ambient_color
	material_->AddConstBufPS({ 0.0f,0.0f,0.0f,0.0f }); // g_dissolve_param

	// 残像の最大数分、モデルのクローンとRendererをあらかじめ作っておく
	for (int i = 0; i < GHOST_MAX; ++i)
	{
		int cModel = MV1DuplicateModel(chara->GetTransform().modelId);
		afterimageModels_.push_back(cModel);

		// それぞれのモデルに対応した Renderer を生成して保持
		renderers_.push_back(std::make_unique<ModelRenderer>(cModel, *material_));
	}
}

void DrawAfterimageComponent::Update()
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	dissolveRate_ += 0.005f;
	if (dissolveRate_ > 1.2f) dissolveRate_ = -0.2f;
}

void DrawAfterimageComponent::PostUpdate(void)
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	auto animController = chara->GetAnimController().lock();
	if (!animController) return;

	ghostFrameCount_++;
	if (ghostFrameCount_ >= GHOST_INTERVAL)
	{
		GhostData data;
		data.pos = chara->GetTransform().pos;
		data.quaRot = chara->GetTransform().quaRot;

		// 現在再生中のアニメーション情報を保存
		data.animType = animController->GetPlayType();
		data.animStep = animController->GetPlayStep();

		ghostHistory_.push_front(data);
		if (ghostHistory_.size() > GHOST_MAX)
		{
			ghostHistory_.pop_back();
		}
		ghostFrameCount_ = 0;
	}
}

void DrawAfterimageComponent::Draw()
{
	if (!IsActive()) return;

	auto chara = GetChara();
	if (!chara) return;

	auto animController = chara->GetAnimController().lock();
	if (!animController) return;

	// 過去の履歴データをループ
	for (int i = 0; i < (int)ghostHistory_.size(); ++i)
	{
		const auto& data = ghostHistory_[i];
		if (data.animType == L"") continue;

		int cModel = afterimageModels_[i];

		// 分解用の変数を準備
		VECTOR dummyScl;
		MATRIX dummyRot;
		VECTOR animPos = { 0.0f, 0.0f, 0.0f }; // アニメーションが持つ生の移動成分

		// アニメーションの再生ステップを適用するための処理
		try
		{
			// 配列の先頭からアニメーションの定義データを取得
			const auto& animData = animController->GetAnimationData(data.animType);

			// 本体のアニメーション数を基準に正しいインデックスを算出
			int animIdx = 0;
			int mainModelId = chara->GetTransform().modelId;
			if (MV1GetAnimNum(mainModelId) > 2)
			{
				animIdx = animData.attachNo;
			}
			else if (MV1GetAnimNum(mainModelId) > 1)
			{
				animIdx = 1;
			}

			// アタッチ状態を維持しつつ過去の時間を進める
			int currentAttach = MV1GetAttachAnim(cModel, 0);
			if (currentAttach == -1)
			{
				currentAttach = MV1AttachAnim(cModel, animIdx, animData.model);
			}

			if (currentAttach != -1)
			{
				// 過去に記録したアニメーションの時間を適用
				MV1SetAttachAnimTime(cModel, currentAttach, data.animStep);

				// 腰（Hips）フレームを検索
				int rootFrame = MV1SearchFrame(cModel, L"mixamorig:Hips");
				if (rootFrame == -1) {
					rootFrame = MV1SearchFrame(cModel, L"Frame_Root");
				}

				if (rootFrame != -1)
				{
					// そのコマのアニメーションが持っている平行移動成分(animPos)を取得
					ModelFrameUtility::GetFrameAnimAttachLocalMatrix(cModel, currentAttach, rootFrame, dummyScl, dummyRot, animPos);
				}

				// ※手動でのボーン上書きによるバグを防ぐため、ユーザー行列はリセットしておく
				MV1ResetFrameUserLocalMatrix(cModel, rootFrame);
			}
		}
		catch (...)
		{
			continue;
		}

		// 拡大縮小行列
		MATRIX matScl = MGetScale(chara->GetTransform().scl);

		// 過去に記録した回転行列を作成 (Transform::Update と完全同期)
		Quaternion q = data.quaRot.Mult(chara->GetTransform().quaRotLocal);
		MATRIX matRot = q.ToMatrix();

		// アニメーション移動量のうち、X と Z だけを抽出して回転を適用する
		// 高さは抜かない（0.0fにする）ことで、ジャンプや空中ダッシュの軌跡の位置を完璧に守ります
		VECTOR pureHorizontalMove = VGet(animPos.x, 0.0f, animPos.z);
		VECTOR rotatedOffset = VTransform(pureHorizontalMove, matRot);

		// 過去のワールド座標（data.pos）から、アニメーションの水平移動分だけを「引き算」する
		VECTOR correctedPos = VSub(data.pos, rotatedOffset);
		MATRIX matPos = MGetTranslate(correctedPos);

		// DxLibの標準仕様 (Scale -> Rotate -> Translate) で行列を合成
		MATRIX mat = MGetIdent();
		mat = MMult(mat, matScl);
		mat = MMult(mat, matRot);
		mat = MMult(mat, matPos);

		// 確定した行列を残像モデルにセット
		MV1SetMatrix(cModel, mat);

		// 透明度設定
		float alpha = 0.4f * (1.0f - (float)i / GHOST_MAX);
		material_->SetConstBufPS(3, { alpha, 0.0f, 0.0f, 0.0f });

		// 描画
		renderers_[i]->Draw();
	}
}