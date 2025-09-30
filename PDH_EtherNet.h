
#ifndef ____PDH_ETHERNET_H____
#define ____PDH_EHTERNET_H____


#include <windows.h>
#include <tchar.h>
#include <pdh.h>
#include <strsafe.h>

#define PDH_ETHERNET_MAX		8

class CPdh_EtherNet
{

	//--------------------------------------------------------------
	// 이더넷 하나에 대한 Send,Recv PDH 쿼리 정보.
	//--------------------------------------------------------------
	struct ETHERNET
	{
		bool		_bUse;
		WCHAR		_szName[128];

		PDH_HCOUNTER	_pdh_Counter_Network_RecvBytes;
		PDH_HCOUNTER	_pdh_Counter_Network_SendBytes;
	};

public:
	explicit CPdh_EtherNet()
	{
		// PDH 쿼리 핸들 생성
		PdhOpenQuery(NULL, NULL, &this->_Query);


		int Cnt = 0;
		bool Err = false;
		WCHAR* pCur = NULL;
		WCHAR* pCounters = NULL;
		WCHAR* pInterfaces = NULL;
		DWORD CounterSize = 0;
		DWORD dwInterfaceSize = 0;
		WCHAR QueryStr[1024] = { 0, };


		//	PDH enum Object 를 사용하는 방법.
		//	모든 이더넷 이름이 나오지만 실제 사용중인 이더넷, 가상이더넷 등등을 확인불가 함.
		//---------------------------------------------------------------------------------------
		// PdhEnumObjectItems 을 통해서 "NetworkInterface" 항목에서 얻을 수 있는
		// 특정항목(Counters) / 인터페이스 항목(Interfaces) 를 얻음. 그런데 그 개수나 길이를 모르기 때문에
		// 먼저 버퍼의 길이를 알기 위해서 Out Buffer 인자들을 NULL 포인터로 넣어서 사이즈만 확인.
		//---------------------------------------------------------------------------------------
		PdhEnumObjectItems(NULL, NULL, L"Network Interface", pCounters, &CounterSize, pInterfaces, &dwInterfaceSize, PERF_DETAIL_WIZARD, 0);

		pCounters = new WCHAR[CounterSize];
		pInterfaces = new WCHAR[dwInterfaceSize];

		//---------------------------------------------------------------------------------------
		// 버퍼의 동적할당 후 다시 호출!
		// 
		// Counters 와 Interfaces 버퍼에는 여러개의 문자열이 들어온다. 2차원 배열도 아니고,
		// 그냥 NULL 포인터로 끝나는 문자열들이 dwCounterSize, dwInterfaceSize 길이만큼 줄줄이 들어있음.
		// 이를 문자열 단위로 끊어서 개수를 확인 해야 함.    aaa\0bbb\0ccc\0ddd  이런 식
		//---------------------------------------------------------------------------------------
		if (ERROR_SUCCESS != PdhEnumObjectItems(NULL, NULL, L"Network Interface", pCounters, &CounterSize, pInterfaces, &dwInterfaceSize, PERF_DETAIL_WIZARD, 0))
		{
			delete[] pCounters;
			delete[] pInterfaces;
			return;
		}

		Cnt = 0;
		pCur = pInterfaces;


		//---------------------------------------------------------
		// szInterfaces 에서 문자열 단위로 끊으면서 , 이름을 복사받는다.
		//---------------------------------------------------------
		for (; *pCur != L'\0' && Cnt < PDH_ETHERNET_MAX; pCur += wcslen(pCur) + 1, Cnt++)
		{
			_EthernetStruct[Cnt]._bUse = true;
			_EthernetStruct[Cnt]._szName[0] = L'\0';

			wcscpy_s(_EthernetStruct[Cnt]._szName, pCur);

			QueryStr[0] = L'\0';
			StringCbPrintf(QueryStr, sizeof(WCHAR) * 1024, L"\\Network Interface(%s)\\Bytes Received/sec", pCur);
			res = PdhAddCounter(_Query, QueryStr, NULL, &_EthernetStruct[Cnt]._pdh_Counter_Network_RecvBytes);

			QueryStr[0] = L'\0';
			StringCbPrintf(QueryStr, sizeof(WCHAR) * 1024, L"\\Network Interface(%s)\\Bytes Sent/sec", pCur);
			res = PdhAddCounter(_Query, QueryStr, NULL, &_EthernetStruct[Cnt]._pdh_Counter_Network_SendBytes);

			PDH_EtherNetUpdate();
		}
	}
	virtual ~CPdh_EtherNet() {}
public:
	void PDH_EtherNetUpdate(void)
	{
		PDH_FMT_COUNTERVALUE CounterVal = { 0, };

		///////////////////////////////////////////////////////////////////////////
		// 
		//  위에서 만들어진
		//	_EthernetStruct[iCnt]._pdh_Counter_Network_SendBytes
		//	_EthernetStruct[iCnt]._pdh_Counter_Network_RecvBytes
		//
		//	PDH 카운터를 다른 PDH 카운터와 같은 방법으로 사용 해주면 됨.
		////////////////////////////////////////////////////////////////////////////

		PdhCollectQueryData(this->_Query);

		//-----------------------------------------------------------------------------------------------
		// 이더넷 개수만큼 돌면서 총 합을 뽑음.
		//-----------------------------------------------------------------------------------------------
		for (int iCnt = 0; iCnt < PDH_ETHERNET_MAX; iCnt++)
		{
			if (_EthernetStruct[iCnt]._bUse)
			{
				_Status = PdhGetFormattedCounterValue(_EthernetStruct[iCnt]._pdh_Counter_Network_RecvBytes, PDH_FMT_DOUBLE, NULL, &CounterVal);

				if (_Status == 0)
					_pdh_value_Network_RecvBytes += CounterVal.doubleValue;

				_Status = PdhGetFormattedCounterValue(_EthernetStruct[iCnt]._pdh_Counter_Network_SendBytes, PDH_FMT_DOUBLE, NULL, &CounterVal);

				if (_Status == 0)
					_pdh_value_Network_SendBytes += CounterVal.doubleValue;
			}
		}
	}

public:
	double GetNetWorkRecvBytes(void) { return this->_pdh_value_Network_RecvBytes; }
	double GetNetWorkSendBytes(void) { return this->_pdh_value_Network_SendBytes; }


private:
	ETHERNET	_EthernetStruct[PDH_ETHERNET_MAX];	// 랜카드 별 PDH 정보
	double		_pdh_value_Network_RecvBytes;		// 총 Recv Bytes  모든 이더넷의 Recv 수치 합산
	double		_pdh_value_Network_SendBytes;		// 총 Send Bytes 모든 이더넷의 Send 수치 합산
	PDH_HQUERY	_Query;
	PDH_STATUS  _Status;

	//Deubg
	int res = 0;
};
#endif