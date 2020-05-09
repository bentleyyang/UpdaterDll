#include "pch.h"
#include "UpdateProcess.h"


#include "flossy.h"

#include "Path.h"
#include "Setting.h"
#include "Download.h"
#include "UpdateInfoFile.h"


UpdateProcess::UpdateProcess()
{
}


UpdateProcess::~UpdateProcess()
{
}

void UpdateProcess::init(OnRecvUpdate onRecvUpdate, OnDownloadProgress onDownloadProgress, OnInstallProgress onInstallProgress, OnDownloadResult onDownloadResult, OnInstallResult onInstallResult)
{
	//���ûص�
	registerCallback(onRecvUpdate, onDownloadProgress, onInstallProgress, onDownloadResult, onInstallResult);

	flagStop = false;
	m_thProcess = std::thread([this]()
	{
		UpdateInfoFile *insUpdateInfos = UpdateInfoFile::instance();
		Download *insDownload = Download::instance();

		//��ʼ��·�������ʹ���Ŀ¼
		Path::instance()->init();
		if (!Setting::instance()->parseFile(Path::instance()->getPathUpdater() + L"/settings.ini"))
		{
			//TODO:
		}
		if (!Path::instance()->createDirectories(Setting::instance()))
		{
			//TODO:
		}

		const std::string infoFileUrl = to_u8(Setting::instance()->getUrl());
		const std::string infoFileLocation = to_u8(Path::instance()->getPathUpdaterTemp() + L"/UpdateFile.xml");
		while (!flagStop)
		{
			//����������Ϣ
			if (!insUpdateInfos->downloadFile(infoFileUrl, infoFileLocation))
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//TODO:
				continue;
			}

			//����������Ϣ
			if (!insUpdateInfos->parseFile(infoFileLocation))
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//TODO:
				continue;
			}
			if (!insUpdateInfos->collectValidFiles())
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//TODO:
				continue;
			}

			//������Ϣ�ص�
			{
				UpdateInfo info;
				auto generalInfo = insUpdateInfos->generalInfo();
				info.forceUpdate = generalInfo.forceUpdate;
				info.strLicence = generalInfo.license;
				info.strUpdateMsg = "";//TODO
				strcpy(info.strVersion, generalInfo.version.data());
				if (m_onRecvUpdate&&!m_onRecvUpdate(info))
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					//TODO:
					continue; 
				}
			}


			auto & files = insUpdateInfos->FileInfos();
			insDownload->collectDownloadInfo(files);
			insDownload->startDownload();
			if (!insDownload->waitDownload())
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//TODO:
				continue;
			}
			//insDownload->forceStop();
		}
	});
	
}

void UpdateProcess::uninit()
{
	flagStop = true;
	if (m_thProcess.joinable()) { m_thProcess.join(); }
	unregisterCallback();


}

void UpdateProcess::registerCallback(OnRecvUpdate onRecvUpdate, OnDownloadProgress onDownloadProgress, OnInstallProgress onInstallProgress, OnDownloadResult onDownloadResult, OnInstallResult onInstallResult)
{
	Download *insDownload = Download::instance();

	m_onRecvUpdate = onRecvUpdate;
	insDownload->setOnDownloadProgress(onDownloadProgress);
	insDownload->setOnDownloadResult(onDownloadResult);
}

void UpdateProcess::unregisterCallback()
{
	Download *insDownload = Download::instance();

	m_onRecvUpdate = nullptr;
	insDownload->setOnDownloadProgress(nullptr);
	insDownload->setOnDownloadResult(nullptr);
}
