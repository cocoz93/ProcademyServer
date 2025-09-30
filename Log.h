#pragma once

#ifndef ____LOG_H____
#define ____LOG_H____

#include <windows.h>
#include <stdio.h>
#include <strsafe.h>


	


// 여기를 주석처리해서 DEBUG ON/OFF가 가능하게끔 한다.
#define DEBUG____//(Type, Data1, pData2, pData3) CMyLog::DebugLog(Type, (LONG64)Data1, (LONG64*)pData2, (LONG64*)pData3);


#define NONE_VALUE			-1					// 유효하지않은 값
#define NONE_PTR			(LONG64*)0x08123	// 유효하지않은 포인터
#define	DEFAULT_INDEX_SIZE	50000				


/*
	1. 매크로함수로 정의된 함수를 타고가서, 메모리에 정의한다.
	2. Crash가 날때, 메모리에 있는 데이터를 타입별로 각기 다르게 파일에 저장한다.

*/



class CMyLog
{
public:
	
	// 어떤일을 ?는지 추적하기위한 ENUM
	enum DEBUGLOG_TYPE
	{
	};


	/*
	FL Free - Free

	UniqueCount증가  	1. 증가값
	UniqueCount백업 	1. 백업값
	노드백업	 	3. TopNode값
	새노드에 현재Top연결	3. TopNode값
	CAS성공 		1. 유니크카운트 3. 새로운탑, 4. 새로운탑의 pNext 5. 원래있던값
	CAS실패			1. 유니크카운트 3. 현재탑    4. 현재탑의pNext    5. 원래있던값



	FL Free - Alloc

	UniqueCount증가 	1. 증가값
	노드생성		3. 새노드

	UniqueCount백업		1. 백업값
	노드백업		3. 백업노드

	CAS성공 		1. 유니크카운트 3. 새로운탑, 4. 새로운탑의 pNext 5. 원래있던값
	CAS실패			1. 유니크카운트 3. 현재탑    4. 현재탑의pNext    5. 원래있던값
		
	
	*/

	struct DEBUGLOG_HEADER
	{
		DWORD			ThreadId;
		DEBUGLOG_TYPE	Type;
		LONG64			Data1;
		LONG64			Data2;
		LONG64*			pData3;
		LONG64*			pData4;
	};

public:
	explicit CMyLog(int MaxCount = DEFAULT_INDEX_SIZE);
	virtual ~CMyLog();

public:
	// 사용법
	// 상황에 맞는 메모리로그(함수)가 호출된다.
	
	static int MemorySaveLog(int* OutIndex);		// 메모리에 저장하는 로그. 어떤함수든 공통적으로 호출한다.
	static int FileSaveLog(void);			

	// 최종적으로 파일로 저장한느 로그.
#define DEBUG_LOG____FREE_UC_INC		CMyLog::FL_FREE_UC_INC(nUniqueCount);
#define DEBUG_LOG____FREE_TOP_BACKUP	CMyLog::FL_FREE_TOP_BACKUP((LONG64*)bTopNode.pNode);
#define DEBUG_LOG____FREE_NEXT_CONNECT	CMyLog::FL_FREE_NEXT_CONNECT((LONG64*)fNode->pNextNode);
#define DEBUG_LOG____FREE_CAS_FAIL		CMyLog::FL_FREE_CAS_FAIL((LONG64*)bNode, (LONG64*)bTopNode.pNode);
#define DEBUG_LOG____FREE_CAS_COMP		CMyLog::FL_FREE_CAS_COMP((LONG64*)bTopNode.pNode, (LONG64*)fNode);
#define DEBUG_LOG____FREE_USE_INC		CMyLog::FL_FREE_USE_INC(lUseSize);

#define DEBUG_LOG____ALLOC_UC_INC		
#define DEBUG_LOG____ALLOC_USE_INC
#define DEBUG_LOG____ALLOC_MALLOC		//UseSize포함
#define DEBUG_LOG____ALLOC_ALLOC_INC
#define DEBUG_LOG____ALLOC_TOP_BACKUP
#define DEBUG_LOG____ALLOC_NEXT_CONNECT
#define DEBUG_LOG____ALLOC_CAS_FAIL
#define DEBUG_LOG____ALLOC_CAS_COMP

public:
	static void FL_FREE_UC_INC(LONG64 UniqueCount);
	static void FL_FREE_TOP_BACKUP(LONG64* pbackupNode);
	static void FL_FREE_NEXT_CONNECT(LONG64* pNewNodeNext);
	static void FL_FREE_CAS_FAIL(LONG64* pbackupNode, LONG64* CurTopNode);
	static void FL_FREE_CAS_COMP(LONG64* pbackupNode, LONG64* CurTopNode);
	static void FL_FREE_USE_INC(LONG64 UseSize);
	

public:
	static void FL_UC_INC_FILE(int Index);



public:
	void TypeToString(WCHAR* TypeString, int StrSize, DEBUGLOG_TYPE Type);

public:

private:
	DEBUGLOG_HEADER* DebugHeader;
	__int64			 DebugIndex;
	__int64			 Remainder_Count;
	static bool		 SaveFlag;
};

extern CMyLog mylog;


#endif
//