#pragma once
#include <vector>
#include <string>
#include <memory>

// CSVの1行分の文字列データを保持する箱クラス
class CSVData
{
public:

    // コンストラクタ
    CSVData() = default;

    // デストラクタ
    ~CSVData() = default;

    // 分解された1行分のデータをセットする
    void SetData(const std::vector<std::wstring>& values) { m_values = values; }

    // 指定した列番号（インデックス）の文字列を取得する（0番目＝Index, 1番目＝AnimId...）
    const std::wstring& GetData(size_t index) const { return m_values[index]; }

private:

    std::vector<std::wstring> m_values;
};

// CSVファイルを読み込んで解析する職人クラス
class CSVDataLoader
{
public:

    // シングルトンインスタンスの取得
    static CSVDataLoader& GetInstance();

    // CSVファイルを読み込んで CSVData の配列にして返す
    std::vector<std::shared_ptr<CSVData>> LoadCSV(const wchar_t* path);

private:
    
    // コンストラクタ
    CSVDataLoader() = default;

    // デストラクタ
    ~CSVDataLoader() = default;

    // 実際にファイルを開いて文字列の2次元配列に分解する内部関数
    std::vector<std::vector<std::wstring>> GetWStringList(const wchar_t* path);
};