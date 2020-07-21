// シリアルポートクラス

#pragma once

#include <Windows.h>
#include <tchar.h>

namespace nkc {

class ComPort
{
	const int COM_SEARCH_MAX = 20;
	HANDLE _ComHandle;

public:

	ComPort(void);
	~ComPort(void);
	int Open(TCHAR* config, char* sendStr, int sendLen, char* checkStr, int checkLen);
	bool Open(int port, const TCHAR* config);
	void Close();
	DWORD Send(const BYTE* data, DWORD dataLen);
	DWORD Receive(BYTE* buffer, DWORD bufferLen);
	DWORD WaitReceive(BYTE* buffer, DWORD bufferLen, int timeout);
};

}; // namespace nkc
