#include "SwordTrailData.h"

SwordTrailData::SwordTrailData(int bufferSize) : maxBufferSize(bufferSize)
{
    posArray.resize(maxBufferSize);
    // 1つの履歴（head/tail）につき2頂点作成するため
    vertices.resize(maxBufferSize * 2);
}

void SwordTrailData::SetPos(const VECTOR& head, const VECTOR& tail)
{
    // 最新の座標を先頭に格納
	posArray[0].headPos = head; // 剣の先端位置
	posArray[0].tailPos = tail; // 剣の根元位置
	posArray[0].isUsed = true;  // 使用中フラグを立てる
}

void SwordTrailData::Reset(const VECTOR& head, const VECTOR& tail)
{
    // 全ての履歴を一律同じデータで初期化
    for (int i = 0; i < maxBufferSize; ++i)
    {
        posArray[i].headPos = head;
        posArray[i].tailPos = tail;
        posArray[i].isUsed = false; // 非使用状態に戻す
    }

    // 頂点バッファも同様に更新
    Update();
}

void SwordTrailData::Update()
{
    // 1. 履歴を古い方へずらす
    for (int i = maxBufferSize - 1; i > 0; --i)
    {
        posArray[i] = posArray[i - 1];
    }

    // 2. 履歴から頂点バッファ（三角形ストリップ用）を作成
    for (int i = 0; i < maxBufferSize; ++i)
    {
        // 先端側の頂点
        vertices[i * 2].pos = posArray[i].headPos;
        vertices[i * 2].uv.x = (float)i / (float)(maxBufferSize - 1); // 横方向のUV
        vertices[i * 2].uv.y = 0.0f;                                  // 上端

        // 根元側の頂点
        vertices[i * 2 + 1].pos = posArray[i].tailPos;
        vertices[i * 2 + 1].uv.x = (float)i / (float)(maxBufferSize - 1);
        vertices[i * 2 + 1].uv.y = 1.0f;                                  // 下端
    }
}