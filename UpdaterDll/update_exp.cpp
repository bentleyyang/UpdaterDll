#include "pch.h"
#include "update_exp.h"
#include "UpdateProcess.h"

UpdateInfo::UpdateInfo()
{

}

UpdateInfo::~UpdateInfo()
{

}

static UpdateProcess updateProcess;

void init(OnRecvUpdate onRecvUpdate
	, OnDownloadProgress  onDownloadProgress
	, OnInstallProgress  onInstallProgress
	, OnDownloadResult  onDownloadResult
	, OnInstallResult  onInstallResult
)
{
	updateProcess.init(onRecvUpdate, onDownloadProgress, onInstallProgress, onDownloadResult, onInstallResult);
}

void uninit(
)
{
	updateProcess.uninit();
}

int get_update_info(
	UpdateInfo *info
)
{
	return -1;
}