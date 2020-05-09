#pragma once


#include <thread>
#include "update_exp.h"

class UpdateProcess
{
public:
	UpdateProcess();
	~UpdateProcess();

	void init(OnRecvUpdate onRecvUpdate
		, OnDownloadProgress  onDownloadProgress
		, OnInstallProgress  onInstallProgress
		, OnDownloadResult  onDownloadResult
		, OnInstallResult  onInstallResult
	);
	void uninit();

private:
	void registerCallback(
		OnRecvUpdate onRecvUpdate
		, OnDownloadProgress  onDownloadProgress
		, OnInstallProgress  onInstallProgress
		, OnDownloadResult  onDownloadResult
		, OnInstallResult  onInstallResult
	);
	void unregisterCallback();

private:
	std::thread m_thProcess;
	bool flagStop = true;

	OnRecvUpdate m_onRecvUpdate=nullptr;
};

