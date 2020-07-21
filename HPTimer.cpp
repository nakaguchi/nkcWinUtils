// 高精度タイマークラス

#include "HPTimer.h"

namespace nkc {

// コンストラクタ
HPTimer::HPTimer() {
	QueryPerformanceFrequency(&_nFreq);
	QueryPerformanceCounter(&_nBegin);
	_nLast = _nBegin;
}

// 経過時間算出
double HPTimer::Calc(LARGE_INTEGER before) {
	LARGE_INTEGER cur;
	QueryPerformanceCounter(&cur);
	double elapse = (cur.QuadPart - before.QuadPart) * 1000.0 / _nFreq.QuadPart;
	_nLast = cur;
	return elapse;
}

// 開始時からの経過時間(ms)
double HPTimer::Elapse() {
	return this->Calc(_nBegin);
}

// 前回からの経過時間(ms)
double HPTimer::Interval() {
	return this->Calc(_nLast);
}

// 開始時間リセット
void HPTimer::Reset() {
	QueryPerformanceCounter(&_nBegin);
}

// 高精度Sleep
void HPTimer::HPSleep(double delayms) {
	HPTimer timer;
	while (1) {
		if (timer.Elapse() > delayms) break;
		Sleep(0);
	}
}

}; // namespace nkc
