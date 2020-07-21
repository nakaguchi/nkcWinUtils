#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>

namespace nkc {

// スワップクラス
template<class T>
class SWAP {
public:
	static void DO(T& a, T& b) {
		T tmp = a;
		a = b;
		b = tmp;
	}
};

typedef std::vector<std::vector<std::string>> STR_TABLE;

namespace wut {

// マルチバイト文字列をワイド文字列（UNICODE）に変換
std::wstring Multi2Wide(std::string const& src);

// 書式付きデバッグ出力
int DebugPrintf(LPCTSTR format, ...);

// テーブルを読み込み
bool ReadTable(const std::string& filename, STR_TABLE& table, const char delimiter = ',');

// マルチディスプレイの情報を取得
// 要素0：メインウインドウ
std::vector<RECT> GetDisplayInfo();

// ディスプレイ情報取得コールバック
BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData);

// 安全なデリート
void SafeDelete(void** ptr);

// 日付時刻文字列取得
std::string DateTimeStr();

}; // namespace wut
}; // namespace nkc
