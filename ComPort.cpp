#include "ComPort.h"

namespace nkc {

// コンストラクタ
ComPort::ComPort(void) :
	_ComHandle(INVALID_HANDLE_VALUE)
{
}

// デストラクタ(終了処理)
ComPort::~ComPort(void)
{
	this->Close();
}

// ポートを探して開く（sendStrを送信して，checkStrを返すポートを探す）
// TCHAR* config : シリアルポート設定文字列 例）""
// char* sendStr : 送信文字列（バイナリデータ可）
// int sendLen : 送信文字列の長さ (byte)
// char* checkStr : チェック文字列（バイナリデータ可）
// int checkLen : チェック文字列の長さ (byte)
// 戻り値 : 発見したポート番号（未発見時は0）
int ComPort::Open(TCHAR* config, char* sendStr, int sendLen, char* checkStr, int checkLen)
{
	for (int findPort = 1; findPort <= COM_SEARCH_MAX; findPort++) {
		if (this->Open(findPort, config)) {
			this->Send((BYTE*)sendStr, sendLen);
			::Sleep(100); // 少し待つ
			BYTE buf[256];
			this->Receive(buf, 256);
			if (!::strncmp((char*)buf, checkStr, checkLen)) return findPort;
		}
	}
	return 0;
}

// ポートを開く
bool ComPort::Open(int port, const TCHAR* config)
{
	this->Close();

	// ポート番号文字列生成
	if (port < 1) return false;
	TCHAR portStr[16];
	::wsprintf(portStr, _T("\\\\.\\COM%d"), port);

	// Comポートを開く
	_ComHandle = ::CreateFile(portStr, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (_ComHandle == INVALID_HANDLE_VALUE) return false;

	// ポート設定
	DCB dcb;
	::GetCommState(_ComHandle, &dcb);
	::BuildCommDCB(config, &dcb);
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fAbortOnError = FALSE;
	::SetCommState(_ComHandle, &dcb);

	// タイムアウト設定
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

// ポートを閉じる
void ComPort::Close()
{
	if (_ComHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_ComHandle);
		_ComHandle = INVALID_HANDLE_VALUE;
	}
}

// データ送信
DWORD ComPort::Send(const BYTE* data, DWORD dataLen)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	DWORD sendSize; // 送信したバイト数
	::WriteFile(_ComHandle, data, dataLen, &sendSize, NULL);

	return sendSize;
}

// データ受信
DWORD ComPort::Receive(BYTE* buffer, DWORD bufferLen)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	DWORD errors;
	COMSTAT stat;
	::ClearCommError(_ComHandle, &errors, &stat);

	DWORD queSize = stat.cbInQue;
	if (queSize < 1) return 0;
	if (bufferLen < queSize) queSize = bufferLen;

	// バッファのクリア
	::memset(buffer, 0, bufferLen);

	// 受信
	DWORD receiveSize; // 受信したバイト数
	::ReadFile(_ComHandle, buffer, queSize, &receiveSize, NULL);

	return receiveSize;
}

// タイムアウト付きデータ受信
DWORD ComPort::WaitReceive(BYTE* buffer, DWORD bufferLen, const int timeout)
{
	if (_ComHandle == INVALID_HANDLE_VALUE) return 0;

	int receiveSize;
	for (int i = 0; i < timeout &&
		(receiveSize = this->Receive(buffer, bufferLen)) < 1; i++) ::Sleep(1);

	return receiveSize;
}

}; // namespace nkc
