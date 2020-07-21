#include "ComPort.h"

namespace nkc {

// �R���X�g���N�^
ComPort::ComPort(void) :
	_ComHandle(INVALID_HANDLE_VALUE)
{
}

// �f�X�g���N�^(�I������)
ComPort::~ComPort(void)
{
	this->Close();
}

// �|�[�g��T���ĊJ���isendStr�𑗐M���āCcheckStr��Ԃ��|�[�g��T���j
// TCHAR* config : �V���A���|�[�g�ݒ蕶���� ��j""
// char* sendStr : ���M������i�o�C�i���f�[�^�j
// int sendLen : ���M������̒��� (byte)
// char* checkStr : �`�F�b�N������i�o�C�i���f�[�^�j
// int checkLen : �`�F�b�N������̒��� (byte)
// �߂�l : ���������|�[�g�ԍ��i����������0�j
int ComPort::Open(TCHAR* config, char* sendStr, int sendLen, char* checkStr, int checkLen)
{
	for (int findPort = 1; findPort <= COM_SEARCH_MAX; findPort++) {
		if (this->Open(findPort, config)) {
			this->Send((BYTE*)sendStr, sendLen);
			::Sleep(100); // �����҂�
			BYTE buf[256];
			this->Receive(buf, 256);
			if (!::strncmp((char*)buf, checkStr, checkLen)) return findPort;
		}
	}
	return 0;
}

// �|�[�g���J��
bool ComPort::Open(int port, const TCHAR* config)
{
	this->Close();

	// �|�[�g�ԍ������񐶐�
	if (port < 1) return false;
	TCHAR portStr[16];
	::wsprintf(portStr, _T("\\\\.\\COM%d"), port);

	// Com�|�[�g���J��
	_ComHandle = ::CreateFile(portStr, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (_ComHandle == INVALID_HANDLE_VALUE) return false;

	// �|�[�g�ݒ�
	DCB dcb;
	::GetCommState(_ComHandle, &dcb);
	::BuildCommDCB(config, &dcb);
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fAbortOnError = FALSE;
	::SetCommState(_ComHandle, &dcb);

	// �^�C���A�E�g�ݒ�
	COMMTIMEOUTS timeout;
	::GetCommTimeouts(_ComHandle, &timeout);
	timeout.ReadIntervalTimeout = MAXDWORD;
	timeout.ReadTotalTimeoutMultiplier = 1;
	timeout.ReadTotalTimeoutConstant = 500;
	timeout.WriteTotalTimeoutMultiplier = 1;
	timeout.WriteTotalTimeoutConstant = 500;
	::SetCommTimeouts(_ComHandle, &timeout);

	return true;
}

// �|�[�g�����
void ComPort::Close()
{
	if (_ComHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_ComHandle);
		_ComHandle = INVALID_HANDLE_VALUE;
	}
}

// �f�[�^���M
DWORD ComPort::Send(const BYTE* data, DWORD dataLen)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	DWORD sendSize; // ���M�����o�C�g��
	::WriteFile(_ComHandle, data, dataLen, &sendSize, NULL);

	return sendSize;
}

// �f�[�^��M
DWORD ComPort::Receive(BYTE* buffer, DWORD bufferLen)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	DWORD errors;
	COMSTAT stat;
	::ClearCommError(_ComHandle, &errors, &stat);

	DWORD queSize = stat.cbInQue;
	if (queSize < 1) return 0;
	if (bufferLen < queSize) queSize = bufferLen;

	// �o�b�t�@�̃N���A
	::memset(buffer, 0, bufferLen);

	// ��M
	DWORD receiveSize; // ��M�����o�C�g��
	::ReadFile(_ComHandle, buffer, queSize, &receiveSize, NULL);

	return receiveSize;
}

// �^�C���A�E�g�t���f�[�^��M
DWORD ComPort::WaitReceive(BYTE* buffer, DWORD bufferLen, const int timeout)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	int receiveSize;
	for (int i = 0; i < timeout &&
		(receiveSize = this->Receive(buffer, bufferLen)) < 1; i++) ::Sleep(1);

	return receiveSize;
}

}; // namespace nkc
