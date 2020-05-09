#pragma once

#include <string>
#include <vector>
#include <thread>
#include <future>
#include <functional>

#include "update_exp.h"
#include "File.h"
#include "Action.h"
#include "HttpFileDownloader.h"
class Download
{
public:
	Download();
	~Download();

	static Download* instance() {
		static Download ins;
		return &ins;
	}

	void startDownload();
	void forceStop();
	bool waitDownload();

	using OnDownloadProgressInternal = std::function<void(int percent)>;
	using OnDownloadResultInternal = std::function<void(bool res)>;

	template<typename __Fn>
	void setOnDownloadProgress(__Fn fn) { m_onDownloadProgress = fn; }

	template<typename __Fn>
	void setOnDownloadResult(__Fn fn) { m_onDownloadResult = fn; }

private:
	bool flagStopDownload = true;
	std::promise<bool> promDownloadFiles;
	std::future<bool> fuDownloadFiles;
	
	std::thread thDownloadFiles;
	std::vector<std::shared_ptr<ActionBase>> m_downloadActions;
	HttpFileDownloader downloader;

	

	OnDownloadProgressInternal m_onDownloadProgress;
	OnDownloadResultInternal m_onDownloadResult;

public:
	
	//整合需要下载的文件
	void collectDownloadInfo(const std::vector<FileInfo>& files);
	void downloadFiles(std::promise<bool>& res);

	void downloadInis() {};
};

