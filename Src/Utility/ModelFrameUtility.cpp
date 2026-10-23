#include "../Common/Quaternion.h"
#include "ModelFrameUtility.h"
#include "Utility.h"
#include "../Object/Common/Transform.h"


void ModelFrameUtility::SetFrameLocalMatrixPos(int modelId, int frameIdx, VECTOR& pos)
{

	VECTOR tmpScl;
	MATRIX tmpMatRot;
	VECTOR tmpPos;

	// 対象フレームのワールド行列を大きさ・回転・位置に分解してを取得する
	GetFrameLocalMatrix(modelId, frameIdx, tmpScl, tmpMatRot, tmpPos);

	// 合成
	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetScale(tmpScl));
	ret = MMult(ret, tmpMatRot);
	ret = MMult(ret, MGetTranslate(pos));

	// 対象フレームにワールド行列をセット
	//MV1SetFrameUserLocalMatrix(modelId, frameIdx, ret);
	MV1SetFrameUserLocalMatrix(modelId, frameIdx, MV1GetFrameLocalMatrix(modelId, frameIdx));

}

void ModelFrameUtility::GetFrameLocalMatrix(int modelId, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos)
{

	// 対象フレームのローカル行列を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId, frameIdx);

	// 拡大縮小成分
	scl = MGetSize(mat);

	// 回転成分＋拡大縮小成分
	matRot = MGetRotElem(mat);
	// 回転成分のみにする
	auto revScl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	matRot = MMult(matRot, MGetScale(revScl));

	// 移動成分
	pos = MGetTranslateElem(mat);

}


void ModelFrameUtility::GetFrameAnimAttachLocalMatrix(int modelId, int attachNo, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos)
{

	// 対象フレームのローカル行列を取得する
	auto mat = MV1GetAttachAnimFrameLocalMatrix(modelId, attachNo, frameIdx);

	// 拡大縮小成分
	scl = MGetSize(mat);

	// 回転成分＋拡大縮小成分
	matRot = MGetRotElem(mat);
	// 回転成分のみにする
	auto revScl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	matRot = MMult(matRot, MGetScale(revScl));

	// 移動成分
	pos = MGetTranslateElem(mat);

}

void ModelFrameUtility::SetFrameAnimAttachLocalMatrixPos(int modelId, int attachNo, int frameIdx, const VECTOR& pos)
{

	VECTOR tmpScl;
	MATRIX tmpMatRot;
	VECTOR tmpPos;

	// 対象フレームのローカル行列を大きさ・回転・位置に分解してを取得する
	GetFrameAnimAttachLocalMatrix(modelId, attachNo, frameIdx, tmpScl, tmpMatRot, tmpPos);

	// 合成
	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetScale(tmpScl));
	ret = MMult(ret, tmpMatRot);
	ret = MMult(ret, MGetTranslate(pos));

	// 対象フレームにローカル行列をセット
	MV1SetFrameUserLocalMatrix(modelId, frameIdx, ret);

}

void ModelFrameUtility::SetFrameAnimAttachLocalMatrixPos(
    const std::list<BlendData>& list,
    int rootFrame,
    int toAttachNo,
    const VECTOR& toFirstPos)
{
    int modelId = list.empty() ? 0 : list.front().modelId;

    VECTOR blendedScl = VGet(0, 0, 0);
    VECTOR blendedPos = VGet(0, 0, 0);
    Quaternion blendedQuat;
    float totalWeight = 0.0f;

    bool firstQuat = true;

    // --- from 側ブレンド ---
    for (const auto& e : list)
    {
        VECTOR tmpScl, tmpPos;
        MATRIX tmpMatRot;
        //大きさ、位置、回転の獲得
        GetFrameAnimAttachLocalMatrix(e.modelId, e.attachNo, rootFrame,
            tmpScl, tmpMatRot, tmpPos);

        //ブレンド倍率
        float w = e.weight;
        totalWeight += w;

        blendedScl = VAdd(blendedScl, VScale(tmpScl, w));
        blendedPos = VAdd(blendedPos, VScale(tmpPos, w));

        Quaternion q = Quaternion::GetRotation(tmpMatRot);

        if (firstQuat)
        {
            blendedQuat = q;
            firstQuat = false;
        }
        else
        {
            ///初期角度、終了角度、補間時間
            blendedQuat = Quaternion::Slerp(blendedQuat, q, w / totalWeight);
        }
    }

    // --- to 側（残り割合を適用） ---//目標となるアニメーション
    if (totalWeight < 1.0f && toAttachNo >= 0)
    {
        float rest = 1.0f - totalWeight;//0.05を超えたときおかしい
        VECTOR tmpScl, tmpPos;
        MATRIX tmpMatRot;
        //大きさ、位置、回転の獲得
        GetFrameAnimAttachLocalMatrix(modelId, toAttachNo, rootFrame,
            tmpScl, tmpMatRot, tmpPos);

        blendedScl = VAdd(blendedScl, VScale(tmpScl, rest));
        blendedPos = VAdd(blendedPos, VScale(tmpPos, rest));

        Quaternion q = Quaternion::GetRotation(tmpMatRot);
        blendedQuat = Quaternion::Slerp(blendedQuat, q, rest / (1.0f));

        // ここで1.0fになる
        totalWeight += rest;
    }

    // --- 平均化 ---
    blendedScl = VScale(blendedScl, 1.0f / totalWeight);
    blendedPos = VScale(blendedPos, 1.0f / totalWeight);

    MATRIX rotMat = blendedQuat.ToMatrix();

    MATRIX finalMat = MGetIdent();
    finalMat = MMult(finalMat, MGetScale(blendedScl));
    finalMat = MMult(finalMat, rotMat);
    finalMat = MMult(finalMat, MGetTranslate(toFirstPos));

    MV1SetFrameUserLocalMatrix(modelId, rootFrame, finalMat);
}

void ModelFrameUtility::SetFreamRotation(int _modelId, int _rootFrame, Quaternion _rot)
{
    // 8. 回転行列を上半身ボーンに適用
    MV1SetFrameUserLocalMatrix(_modelId, _rootFrame, _rot.ToMatrix());
}

VECTOR ModelFrameUtility::GetFreamPosFromDistance(Transform modelTrans, std::wstring boneName, VECTOR distance)
{
	// プレイヤーモデルのスケールを逆にして、武器モデルのスケール計算に利用
	VECTOR scl = modelTrans.scl;
	scl.x = 1.0f / scl.x;
	scl.y = 1.0f / scl.y;
	scl.z = 1.0f / scl.z;

	// フレームの取得
	int frmNo = MV1SearchFrame(modelTrans.modelId, boneName.c_str());
	if (frmNo == -1) {
		// エラー処理またはログ出力
		return VGet(0.0f, 0.0f, 0.0f);
	}

	// 手の位置とグローバルマトリクスを取得
	const auto& posFream = MV1GetFramePosition(modelTrans.modelId, frmNo);
	MATRIX handWorldMatrix = MV1GetFrameLocalWorldMatrix(modelTrans.modelId, frmNo);
	// 手のワールド回転を取得
	Quaternion handWorldRot = Quaternion::GetRotation(handWorldMatrix);

	// 最終的な武器の位置を設定
	// オフセット位置は、手のワールド回転のバック方向を基準としていると仮定
	VECTOR ResultPos = VECTOR(0.0f, 0.0f, 0.0f);
	ResultPos = VAdd(posFream, VScale(handWorldRot.GetRight(), distance.x));
	ResultPos = VAdd(ResultPos, VScale(handWorldRot.GetUp(), distance.y));
	ResultPos = VAdd(ResultPos, VScale(handWorldRot.GetForward(), distance.z));

	return ResultPos;
}

// オブジェクトのフレーム追従♭
void ModelFrameUtility::SyncWeaponToFream(const TCHAR* frameName, const VECTOR& offsetRot, const VECTOR& offsetPos,
	const Transform& modelTransform, Transform& outWeaponTransform)
{
	// プレイヤーモデルのスケールを逆にして、武器モデルのスケール計算に利用
	VECTOR scl = modelTransform.scl;
	scl.x = 1.0f / scl.x;
	scl.y = 1.0f / scl.y;
	scl.z = 1.0f / scl.z;

	// フレームの取得
	int frmNo = MV1SearchFrame(modelTransform.modelId, frameName);
	if (frmNo == -1) {
		// エラー処理またはログ出力
		return;
	}

	// 手の位置とグローバルマトリクスを取得
	const auto& posFream = MV1GetFramePosition(modelTransform.modelId, frmNo);
	MATRIX handWorldMatrix = MV1GetFrameLocalWorldMatrix(modelTransform.modelId, frmNo);

	// 手のワールド回転を取得
	Quaternion handWorldRot = Quaternion::GetRotation(handWorldMatrix);

	// 手のワールド回転とモデルスケールから、武器の基準となる行列を生成
	MATRIX mixMat = MMult(MGetRotElem(handWorldMatrix), MGetScale(scl));

	// 武器固有のオフセット回転を適用
	// X, Y, Z軸それぞれに回転を適用している現在のロジックを再現
	mixMat = MMult(mixMat,
		MGetRotAxis(VNorm(VTransformSR(Utility::DIR_R, handWorldMatrix)), Utility::Deg2RadF(offsetRot.x)));
	mixMat = MMult(mixMat,
		MGetRotAxis(VNorm(VTransformSR(Utility::DIR_U, handWorldMatrix)), Utility::Deg2RadF(offsetRot.y)));
	mixMat = MMult(mixMat,
		MGetRotAxis(VNorm(VTransformSR(Utility::DIR_F, handWorldMatrix)), Utility::Deg2RadF(offsetRot.z)));

	// 最終的な武器の回転行列を設定
	outWeaponTransform.matRot = mixMat;

	// 最終的な武器の位置を設定
	// オフセット位置は、手のワールド回転のバック方向を基準としていると仮定
	outWeaponTransform.pos = VAdd(posFream, VScale(handWorldRot.GetRight(), offsetPos.x));
	// 必要に応じて offsetPos.x や offsetPos.y も handWorldRot.GetRight() などで加算
	outWeaponTransform.pos = VAdd(outWeaponTransform.pos, VScale(handWorldRot.GetUp(), offsetPos.y));
	outWeaponTransform.pos = VAdd(outWeaponTransform.pos, VScale(handWorldRot.GetForward(), offsetPos.z));

	// モデルの更新
	outWeaponTransform.Update(true);
}
