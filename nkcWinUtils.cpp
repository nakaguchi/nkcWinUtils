#include "nkcWinUtils.h"

#include <system_error>
#include <fstream>
#include <sstream>
#include <stdio.h>

namespace nkc {
namespace wut {

// �}���`�o�C�g����������C�h������iUNICODE�j�ɕϊ�
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

// �����t���f�o�b�O�o��
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

// �e�[�u����ǂݍ���
bool ReadTable(const std::string& filename, STR_TABLE& table, const char delimiter) {
	// �t�@�C�����J��
	std::fstream filestream(filename);
	if (!filestream.is_open()) return false; // �t�@�C�����J���Ȃ������ꍇ�͏I��

	// �t�@�C����ǂݍ���
	while (!filestream.eof()) {
		// �P�s�ǂݍ���
		std::string buffer;
		filestream >> buffer;

		// �t�@�C������ǂݍ��񂾂P�s�̕��������؂蕶���ŕ����ă��X�g�ɒǉ�����
		std::vector<std::string> record;              // �P�s���̕�����̃��X�g
		std::istringstream streambuffer(buffer); // ������X�g���[��
		std::string token;                       // �P�Z�����̕�����
		while (std::getline(streambuffer, token, delimiter)) {
			// �P�Z�����̕���������X�g�ɒǉ�����
			record.push_back(token);
		}

		// �P�s���̕�������o�͈����̃��X�g�ɒǉ�����
		table.push_back(record);
	}

	return true;
}

// �}���`�f�B�X�v���C�̏����擾
// �v�f0�F���C���E�C���h�E
std::vector<RECT> GetDisplayInfo() {
	// �f�B�X�v���C���擾
	std::vector<RECT> displays;
	EnumDisplayMonitors(NULL, NULL,
		(MONITORENUMPROC)MonitorEnumProc, (LPARAM)&displays);

	// ���C�����j�^��擪�v�f�Ɉړ�
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

// �f�B�X�v���C���擾�R�[���o�b�N
BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData) {

	std::vector<RECT>* displays = (std::vector<RECT>*)dwData;
	displays->push_back(*lpMon);
	return TRUE;
}

// ���S�ȃf���[�g
void SafeDelete(void** ptr) {
	if (*ptr) {
		delete* ptr;
		*ptr = NULL;
	}
}

// ���t����������擾 (C++20�ɂȂ����� std::format �g�������j
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
