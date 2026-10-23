#include "CSVDataLoader.h"
#include <fstream>   // ファイル読み込み用
#include <sstream>   // 文字列分解用
#include <Windows.h> // MultiByteToWideChar用

CSVDataLoader& CSVDataLoader::GetInstance()
{
    // アプリ終了までメモリに残り続けるスタティック変数
    static CSVDataLoader csvDataLoader;
    return csvDataLoader;
}

std::vector<std::shared_ptr<CSVData>> CSVDataLoader::LoadCSV(const wchar_t* path)
{
    // データを格納する配列
    std::vector<std::shared_ptr<CSVData>> datas;

    // ファイル内容をすべて分解した文字列リストを取得
    auto valuesDatas = GetWStringList(path);

    // CSVData(箱)に詰め替えて配列に追加していく
    for (const auto& values : valuesDatas)
    {
        std::shared_ptr<CSVData> data = std::make_shared<CSVData>();
        data->SetData(values);
        datas.emplace_back(data);
    }

    return datas;
}

std::vector<std::vector<std::wstring>> CSVDataLoader::GetWStringList(const wchar_t* path)
{
    std::vector<std::vector<std::wstring>> valuesDatas;

    // ファイルを開く
    std::ifstream file(path);
    if (!file.is_open())
    {
        return valuesDatas;
    }

    // ファイル内容を一括で読み込み(UTF-8)
    std::string utf8Data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (utf8Data.empty())
    {
        return valuesDatas;
    }

    // UTF-8からUTF-16 (wstring) に変換
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), static_cast<int>(utf8Data.size()), nullptr, 0);
    std::wstring wdata(wideSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), static_cast<int>(utf8Data.size()), &wdata[0], wideSize);

    // 行ごとに処理
    std::wstringstream ws(wdata);
    std::wstring line;
    bool isHeader = false;
    std::vector<std::wstring> header;

    while (std::getline(ws, line))
    {
        std::wstringstream ss(line);
        std::wstring part;
        std::vector<std::wstring> values;

        // カンマ区切りで分解してリストに追加
        while (std::getline(ss, part, L','))
        {
            // 改行置換処理
            for (size_t i = 0; (i = part.find(L"\\n", i)) != std::wstring::npos; ++i)
            {
                part.replace(i, 2, L"\n");
            }
            values.emplace_back(part);
        }

        // ヘッダー行のスキップ
        if (!isHeader)
        {
            header = values;
            isHeader = true;
            continue;
        }

        // 要素数チェック（ヘッダーより少ない場合は不正な行としてスキップ）
        if (values.size() < header.size()) continue;

        valuesDatas.emplace_back(values);
    }

    return valuesDatas;
}