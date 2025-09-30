#pragma once


#include "Log.h"

static CMyLog mylog;
bool CMyLog::SaveFlag;

//인위적으로 초기화줘야 한다.
CMyLog::CMyLog(int MaxCount)
{
	DebugHeader = new DEBUGLOG_HEADER[MaxCount];
	DebugIndex = NONE_VALUE;
	Remainder_Count = 0;
	SaveFlag = TRUE;
}

CMyLog::~CMyLog()
{
}

int CMyLog::MemorySaveLog(int* OutIndex)
{
	if (SaveFlag == FALSE)
		return false;

	UINT64 Index = InterlockedIncrement((UINT64*)&mylog.DebugIndex);

	//만약 IndexSize를 초과한다면, 다시 0으로 바꿔준다
	if (Index >= DEFAULT_INDEX_SIZE)
	{
		// IndexSize이상의 값이 들어올 수 있으므로, 나머지 연산
		ULONG64 NewIndex = Index % DEFAULT_INDEX_SIZE;

		InterlockedExchange((UINT64*)&mylog.DebugIndex, NewIndex);
		Index = NewIndex;

		++mylog.Remainder_Count;
	}
	return true;
}

void CMyLog::FL_FREE_UC_INC(LONG64 UniqueCount)
{
	int Index = -1;
	if (MemorySaveLog(&Index) == false)
		return;


	// 문자열을 보관하면 메모리많이씀 -> 메모리에는 간단한 정보만 보관
	// 출력하는곳에서 보기편하게 문자열로 출력한다.
	//wsprintf(SaveStr, L"[%d]\t UniqueCount++\t %d->%d\n", OldUniqueCount);

	// Memory Save Log
	mylog.DebugHeader[Index].ThreadId = GetCurrentThreadId();
	mylog.DebugHeader[Index].Data1 = UniqueCount;
}

void CMyLog::FL_FREE_TOP_BACKUP(LONG64* pbackupNode)
{
}

void CMyLog::FL_FREE_NEXT_CONNECT(LONG64* pNewNodeNext)
{
}

void CMyLog::FL_FREE_CAS_FAIL(LONG64* pbackupNode, LONG64* CurTopNode)
{
}

void CMyLog::FL_FREE_CAS_COMP(LONG64* pbackupNode, LONG64* CurTopNode)
{
}

void CMyLog::FL_FREE_USE_INC(LONG64 UseSize)
{
}

void CMyLog::FL_UC_INC_FILE(int Index)
{

}


int CMyLog::FileSaveLog()
{
	if (FALSE == InterlockedExchange8((CHAR*)&SaveFlag, FALSE))
		return false;


	FILE* fp;
	WCHAR FileName[256];
	
	SYSTEMTIME NowTime;
	GetLocalTime(&NowTime);

	// 파일이름 저장 - DebugLOG_2021.08.20_20.13.01
	StringCchPrintf
	(
		FileName,
		sizeof(FileName),
		L"DebugLog_%d.%d.%d_%d.%d.txt",
		NowTime.wYear, NowTime.wMonth, NowTime.wDay,
		NowTime.wHour, NowTime.wMinute
	);


	// 파일오픈
	if (NULL != _wfopen_s(&fp, FileName, L"wt"))
		wprintf(L"***** LOGFILE SAVE ERROR ***** \n\n");


	for (int i = 0; i < mylog.DebugIndex; ++i)
	{
		switch (mylog.DebugHeader[mylog.DebugIndex].Type)
		{
		FL_UC_INC: FL_UC_INC_FILE(i); break;
		}
	}


	//fwprintf(fp, L"  Index |  ThreadId |                           Type  |         Data1  |                Data2  |                Data3  |");
	//fwprintf(fp, L"  ======================================================================================================================\n");

	//WCHAR TypeString[64];
	//for (int i = 0; i < mylog.DebugIndex; ++i)
	//{
	//	//문자열 뽑기
	//	mylog.TypeToString(TypeString, sizeof(TypeString), mylog.DebugArr[i].Type);
	//	
	//	fwprintf(fp, L"  %5d | %9d | %30s  |   %10d   |  0x %8p  |  0x %8p  |",
	//		i,	mylog.DebugArr[i].ThreadId, TypeString,
	//		mylog.DebugArr[i].Data1, mylog.DebugArr[i].pData2, mylog.DebugArr[i].pData3);
	//	fwprintf(fp, L"  ----------------------------------------------------------------------------------------------------------------------\n");
	//}

	// 리소스 반환
	fclose(fp);
	return true;
}

void CMyLog::TypeToString(WCHAR* TypeString, int StrSize,  DEBUGLOG_TYPE Type)
{
//	switch (Type)
//	{
//	case STACK_PUSH___UC_INC:			StringCchCopy(TypeString, StrSize, L"STACK_PUSH___UC_INC");			break;
//	case STACK_PUSH___ALLOC:			StringCchCopy(TypeString, StrSize, L"STACK_PUSH___ALLOC");			break;
//	case STACK_PUSH___NODE_BACKUP:		StringCchCopy(TypeString, StrSize, L"STACK_PUSH___NODE_BACKUP");	break;
//	case STACK_PUSH___UC_BACKUP:		StringCchCopy(TypeString, StrSize, L"STACK_PUSH___UC_BACKUP");		break;
//	case STACK_PUSH___NEXT_NODE_SET:	StringCchCopy(TypeString, StrSize, L"STACK_PUSH___NEXT_NODE_SET");	break;
//	case STACK_PUSH___CAS_FAIL:			StringCchCopy(TypeString, StrSize, L"STACK_PUSH___CAS_FAIL");		break;
//	case STACK_PUSH___CAS_COMPLETE:		StringCchCopy(TypeString, StrSize, L"STACK_PUSH___CAS_COMPLETE");	break;
//	case STACK_PUSH___USE_INC:			StringCchCopy(TypeString, StrSize, L"STACK_PUSH___USE_INC");		break;
//
//		//-----------------------------	-----------------------------------------------------------------------------
//	
//	case STACK_POP___UC_INC:			StringCchCopy(TypeString, StrSize, L"STACK_POP___UC_INC");			break;
//	case STACK_POP___NODE_BACKUP:		StringCchCopy(TypeString, StrSize, L"STACK_POP___NODE_BACKUP");		break;
//	case STACK_POP___UC_BACKUP:			StringCchCopy(TypeString, StrSize, L"STACK_POP___UC_BACKUP");		break;
//	case STACK_POP___CAS_FAIL:			StringCchCopy(TypeString, StrSize, L"STACK_POP___CAS_FAIL");		break;
//	case STACK_POP___CAS_COMPLETE:		StringCchCopy(TypeString, StrSize, L"STACK_POP___CAS_COMPLETE");	break;
//	case STACK_POP___FREE_NODE:			StringCchCopy(TypeString, StrSize, L"STACK_POP___FREE_NODE");		break;
//	
//		//-----------------------------	-----------------------------------------------------------------------------
//	
//	case MAIN_PUSH___COUNT:				StringCchCopy(TypeString, StrSize, L"MAIN_PUSH___COUNT");			break;
//	case MAIN_POP___COUNT:				StringCchCopy(TypeString, StrSize, L"MAIN_POP___COUNT");			break;
//
//	}
}

