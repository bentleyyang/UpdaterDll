#include "pch.h"
#include "Download.h"

#include <typeinfo>
#include <typeindex>
#include "util.hpp"


#include "Path.h"


Download::Download()
{
}


Download::~Download()
{
	forceStop();
}

void Download::startDownload()
{
	forceStop();
	flagStopDownload = false;
	promDownloadFiles = std::promise<bool>();
	fuDownloadFiles = promDownloadFiles.get_future();
	
	thDownloadFiles = std::thread(&Download::downloadFiles, this, std::ref(promDownloadFiles));
}

void Download::forceStop()
{
	downloader.cancelDownload();
	flagStopDownload = true;
	if (thDownloadFiles.joinable()) { thDownloadFiles.join(); }
}

bool Download::waitDownload()
{
	if (fuDownloadFiles.valid())
	{
		bool res = fuDownloadFiles.get();
		if (thDownloadFiles.joinable()) { thDownloadFiles.join(); }
		//if (m_onDownloadResult) { m_onDownloadResult(res); }
		return res;
	}
	else {
		throw std::logic_error("call waitDownload more than once");
	}
}

void Download::collectDownloadInfo(const std::vector<FileInfo>& files)
{
	m_downloadActions.clear();
	for (auto &&file : files)
	{	
		for (auto && action : file.m_actions) 
		{
			if (std::string(typeid(ActionDownload).name()) == typeid(*action).name())//if action==ActionDownload
			{
				m_downloadActions.push_back(action);
			}
		}
	}
}

void Download::downloadFiles(std::promise<bool>& res)
{
	auto onRes = [&](bool re) {
		m_onDownloadResult(re);
		res.set_value(re);
	};
	std::vector<std::pair<std::shared_ptr<ActionDownload>, uint64_t>> downloadInfo;
	uint64_t totalSize = 0, downloadedSize=0;
	
	downloader.setOnDownloadProgress([this, &downloadedSize, &totalSize](uint64_t c, uint64_t t) {
		if (m_onDownloadProgress) {
			m_onDownloadProgress(static_cast<int>(
				static_cast<double>(c + downloadedSize) / totalSize * 100));
		}
	});

	for (auto && action : m_downloadActions)
	{
		if(flagStopDownload) { onRes(false); return; }
		try {
			auto act=std::static_pointer_cast<ActionDownload>(action);
			downloader.setUrl(to_u8(act->getUrl()));
			uint64_t t;
			if (!downloader.obtainFileSize(t)) { onRes(false); return; }
			totalSize += t;
			downloadInfo.push_back({ act, t });
		}
		catch (std::exception& err) { onRes(false); return; }
	}

	for (auto && action : downloadInfo)
	{
		if (flagStopDownload) { onRes(false); return; }
		try {
			auto act = action.first;
			downloader.setUrl(to_u8(act->getUrl()));
			std::wstring downloadPath = Path::instance()->getPathUpdaterTemp() + L"/" + extract_file_name(act->getUrl());
			downloader.setDownloadPath(to_u8(downloadPath));
			downloader.downloadFile();
			downloadedSize += action.second;
		}
		catch (std::exception& err) { onRes(false); return; }
	}

	onRes(true);
}

