// 高精度タイマークラス

#pragma once

#include <Windows.h>


namespace nkc {

// 高精度タイマークラス
class HPTimer {
	LARGE_INTEGER _nFreq;	// 周波数
	LARGE_INTEGER _nBegin;	// 開始時間
	LARGE_INTEGER _nLast;	// 前回問い合わせ時間
	double Calc(LARGE_INTEGER before);

public:
	HPTimer();
	double Elapse();	// 開始時からの経過時間(ms)
	double Interval();	// 前回からの経過時間(ms)
	void Reset();		// 開始時間リセット
	static void HPSleep(double delayms);	// 高精度Sleep
};

}; // namespace nkc
