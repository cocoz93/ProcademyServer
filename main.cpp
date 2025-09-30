#pragma once
#include "ChattingServer.h"

#define SERVER_PORT					6000
#define SERVER_IP					L"127.0.0.1"
#define MAX_CLIENT					15000
#define RUN_WORKER_TRHEAD_COUNT		10

/*
LAN_SERVER <-> NET_SERVER
1. Msg.h의 HEADER_SIZE사이즈 조절
2. Encoding, Decoding
3. Setheader
파서로 설정빼기.
*/

/*
22.5.2
voliatile변수 추가, Release모드로 변경
문제발견 : CMsg계속 우상향 그래프
*/


int main(void)
{
	int WorkerThreadCount = 0;
	printf("RunWorkerThread : ");
	scanf_s("%d", &WorkerThreadCount);

	// CMsg Count방식으로 증가/차감하는것
	// Json으로 설정파일 따로 빼기
	CChattingServer* ChattingServer = new CChattingServer;
	if (FALSE == ChattingServer->Start(CNetServer::SERVER_TYPE::LAN_SERVER, SERVER_IP, SERVER_PORT, WorkerThreadCount, TRUE, MAX_CLIENT, false))
		return false;

	//컨텐츠
	Sleep(INFINITE);

	return 0;
}

