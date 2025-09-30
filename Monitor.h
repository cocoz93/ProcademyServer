#pragma once

#ifndef ____CSYSTEM_MONITOR_H_____
#define ____CSYSTEM_MONITOR_H_____

#include "PDH.h"
#include "PDH_EtherNet.h"

class CSYSTEM_MONITOR
{
public:
	explicit CSYSTEM_MONITOR() {}
	~CSYSTEM_MONITOR() {}

	void Run()
	{
		int CoreCount = pdh.GetCoreCount();
		double pCore[50];

		// ����
		pdh.PDHUpdate();
		pdhEtherNet.PDH_EtherNetUpdate();

		wprintf(L"Processor Total Time : %lf%%\n", pdh.GetCpuTotal());

		pdh.GetCpuCore(pCore, CoreCount);
		wprintf(L"[ %d Core:", CoreCount);
		for (int i = 0; i < CoreCount; ++i)
			wprintf(L" [%0.1llf%%] ", pCore[i]);
		wprintf(L"]\n");

		wprintf(L"CPU [ T:%0.1llf%% ] [ U:%0.1llf%% ]\n", pdh.GetProcessTotal(), pdh.GetProcessUser());
		wprintf(L"HandleCount [ P:%d ] [ T:%d ]\n", pdh.GetThreadHandleCount(), pdh.GetProcessHandleCount());

		wprintf(L"\n");
		wprintf(L"Process Private(UserMemory) : %lld MBytes\n", pdh.GetProcessPrivateBytes() / 1024 / 1024);
		wprintf(L"Total Available Memory : %lld MBytes\n", pdh.GetAvailableMemoryMBytes());
		wprintf(L"Total Use Non-PagedMemory : %lld MBytes \n", pdh.GetTotalUseNonPagedMemoryBytes() / 1024 / 1024);
		wprintf(L"NetWork SendBytes/RecvBytes : %llf / %llf\n", pdhEtherNet.GetNetWorkSendBytes(), pdhEtherNet.GetNetWorkRecvBytes());
	}
	CPdh_EtherNet pdhEtherNet;
	CPdh_Monitor pdh;
};



#endif