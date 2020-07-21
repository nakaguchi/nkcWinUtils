#include "nkcWinUtils.h"

#include <system_error>
#include <fstream>
#include <sstream>
#include <stdio.h>

namespace nkc {
namespace wut {

// マルチバイト文字列をワイド文字列（UNICODE）に変換
std::wstring Multi2Wide(std::string const& src) {
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);
	std::vector<wchar_t> dest(dest_size, L'\0');
	if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), (int)dest.size()) == 0) {
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}
	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();
	return std::wstring(dest.begin(), dest.end());
}

// 書式付きデバッグ出力
int DebugPrintf(LPCTSTR format, ...) {
	va_list args;
	va_start(args, format);

	int len = _vsctprintf(format, args) + 1;
	TCHAR* buf = (TCHAR*)malloc(len * sizeof(TCHAR));
	len = _vstprintf_s(buf, len, format, args);

	OutputDebugString(buf);
	free(buf);

	return len;
}

// テーブルを読み込み
bool ReadTable(const std::string& filename, STR_TABLE& table, const char delimiter) {
	// ファイルを開く
	std::fstream filestream(filename);
	if (!filestream.is_open()) return false; // ファイルが開けなかった場合は終了

	// ファイルを読み込む
	while (!filestream.eof()) {
		// １行読み込む
		std::string buffer;
		filestream >> buffer;

		// ファイルから読み込んだ１行の文字列を区切り文字で分けてリストに追加する
		std::vector<std::string> record;              // １行分の文字列のリスト
		std::istringstream streambuffer(buffer); // 文字列ストリーム
		std::string token;                       // １セル分の文字列
		while (std::getline(streambuffer, token, delimiter)) {
			// １セル分の文字列をリストに追加する
			record.push_back(token);
		}

		// １行分の文字列を出力引数のリストに追加する
		table.push_back(record);
	}

	return true;
}

// マルチディスプレイの情報を取得
// 要素0：メインウインドウ
std::vector<RECT> GetDisplayInfo() {
	// ディスプレイ情報取得
	std::vector<RECT> displays;
	EnumDisplayMonitors(NULL, NULL,
		(MONITORENUMPROC)MonitorEnumProc, (LPARAM)&displays);

	// メインモニタを先頭要素に移動
	int mainMonitor = 0;
	for (int i = 0; i < displays.size(); i++) {
		if (displays[i].left == 0 && displays[i].top == 0) mainMonitor = i;
	}
	if (mainMonitor != 0) {
		SWAP<RECT>::DO(displays[0], displays[mainMonitor]);
	}

#ifdef _DEBUG
	for (int i = 0; i < displays.size(); i++) {
		DebugPrintf(_T("Display %d%s  L %d T %d R %d B %d\n"),
			i, i ? _T("") : _T("(main)"), displays[i].left, displays[i].top,
			displays[i].right, displays[i].bottom);
		//std::cout << "Monitor" << i << " : "
		//	<< displays[i].left << "," << displays[i].top << ","
		//	<< displays[i].right << "," << displays[i].bottom << std::endl;
	}
#endif // _DEBUG

	return displays;
}

// ディスプレイ情報取得コールバック
BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData) {

	std::vector<RECT>* displays = (std::vector<RECT>*)dwData;
	displays->push_back(*lpMon);
	return TRUE;
}

// 安全なデリート
void SafeDelete(void** ptr) {
	if (*ptr) {
		delete* ptr;
		*ptr = NULL;
	}
}

// 日付時刻文字列取得 (C++20になったら std::format 使いたい）
std::string DateTimeStr() {
	SYSTEMTIME st;
	::GetLocalTime(&st);

	char buf[20];
	sprintf_s(buf, 19, "%04d%02d%02d_%02d%02d%02d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	std::string str = buf;
	return str;
}

}; // namespace wut
}; // namespace nkc
