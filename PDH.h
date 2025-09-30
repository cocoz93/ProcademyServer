#pragma once

#ifndef ____PDH_MONITOR_H_____
#define ____PDH_MONITOR_H_____

#include <windows.h>
#include <Pdh.h>
#include <Psapi.h>		//GetModuleBaseName

#include <stdio.h>		// CallString
#include <conio.h>		// CallString
#include <pdhmsg.h>		// CallString
#include <strsafe.h>

#pragma comment(lib, "pdh.lib")


//_________________________________________________________________________
//
// 항목 추가시 로직변경 사항
//_________________________________________________________________________
//
//	1. 클래스에 PDH_HCOUNTER, PDH_FMT_COUNTERVALUE 변수 추가
//	2. 생성자내부에 PdhAddCounter()로, 스트링을 넣고 PDH_HCOUNTER 값 얻음
//		2-1. 이 스트링은 https://docs.microsoft.com/en-us/windows/win32/perfctrs/browsing-performance-counters
//			
//	3. PDH_Update에서 PdhGetFormattedCounterValue() 호출 (+ 자료형 설정)
//	4. Get...()함수를 추가
//	5. 밖에서 호출
//_________________________________________________________________________
class CPdh_Monitor
{
public:
	explicit CPdh_Monitor();
	virtual ~CPdh_Monitor();

public:
	void PDHUpdate(void);

public:
	double GetCpuTotal(void) { return _CpuTotalVal.doubleValue; }			// CPU전체 사용량
	double GetProcessUser(void) { return _ProcessUserVal.doubleValue; }		// 프로세스의 CPU유저 사용량
	double GetProcessTotal(void) { return _ProcessTotalVal.doubleValue; } // 프로세스의 CPU커널 사용량
	void GetCpuCore(double pArr[], int size);								// 코어별 사용량
	int GetCoreCount(void) { return _NumberOfProcessors; }

public:
	long GetProcessHandleCount(void) { return _ProcessHandleCountVal.longValue; }
	long GetThreadHandleCount(void) { return _ThreadHandleCountVal.longValue; }
	__int64 GetProcessPrivateBytes(void) { return _ProcessPrivateBytesVal.largeValue; }		// 프로세스 유저할당메모리
	__int64 GetAvailableMemoryMBytes(void) { return _AvailableMemoryMBytesVal.largeValue; } // 사용가능 메모리
	__int64 GetTotalUseNonPagedMemoryBytes(void) { return _TotalUseNonPagedMemoryVal.largeValue; }

	//__int64 GetProcessNonPagedMemoryBytes(void);  // NonPagedMemory로 문제가 터질때 해당항목으로 잡히지않음.

	// 최종적으로는  private(유저할당 메모리), Available(사용가능) 만 모니터링함.
	// private는 상승하는데 available이 줄어든다면 메모리 누수임



public:
	void CallStringPDH(void);	// MS Sample



private:
	PDH_FMT_COUNTERVALUE _CpuTotalVal;
	PDH_FMT_COUNTERVALUE* _pCpuCoreVal;
	PDH_FMT_COUNTERVALUE _ProcessUserVal;
	PDH_FMT_COUNTERVALUE _ProcessTotalVal;
	PDH_FMT_COUNTERVALUE _ProcessHandleCountVal;
	PDH_FMT_COUNTERVALUE _ThreadHandleCountVal;
	PDH_FMT_COUNTERVALUE _ProcessPrivateBytesVal;
	PDH_FMT_COUNTERVALUE _AvailableMemoryMBytesVal;
	PDH_FMT_COUNTERVALUE _TotalUseNonPagedMemoryVal;


private:
	PDH_HCOUNTER _CpuTotal;
	PDH_HCOUNTER* _pCpuCore;
	PDH_HCOUNTER _ProcessUser;
	PDH_HCOUNTER _ProcessTotal;
	PDH_HCOUNTER _ProcessHandleCount;
	PDH_HCOUNTER _ThreadHandleCount;
	PDH_HCOUNTER _ProcessPrivateBytes;
	PDH_HCOUNTER _AvailableMemoryMBytes;
	PDH_HCOUNTER _TotalUseNonPagedMemory;

private:
	WCHAR _AppName[256];
	SYSTEM_INFO _SystemInfo;
	UINT _NumberOfProcessors;
	PDH_HQUERY _Query;
};




#endif