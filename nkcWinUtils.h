#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>

namespace nkc {

// �X���b�v�N���X
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

// �}���`�o�C�g����������C�h������iUNICODE�j�ɕϊ�
std::wstring Multi2Wide(std::string const& src);

// �����t���f�o�b�O�o��
int DebugPrintf(LPCTSTR format, ...);

// �e�[�u����ǂݍ���
bool ReadTable(const std::string& filename, STR_TABLE& table, const char delimiter = ',');

// �}���`�f�B�X�v���C�̏����擾
// �v�f0�F���C���E�C���h�E
std::vector<RECT> GetDisplayInfo();

// �f�B�X�v���C���擾�R�[���o�b�N
BOOL CALLBACK MonitorEnumProc(
	HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData);

// ���S�ȃf���[�g
void SafeDelete(void** ptr);

// ���t����������擾
std::string DateTimeStr();

}; // namespace wut
}; // namespace nkc
