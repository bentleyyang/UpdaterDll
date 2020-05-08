#pragma once

#include <cinttypes>
#include <string>
#include < functional>

#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/FileStream.h"

class HttpFileDownloader
{
public:
	HttpFileDownloader();
	~HttpFileDownloader();

	using u8string = std::string;
	using OnDownloadProgress = std::function<void(uint64_t curr, uint64_t totoal)>;

	void setUrl(const u8string& _url) {
		objHttpCliSess.reset();
		objUri = _url;
		objHttpReq.setMethod(Poco::Net::HTTPRequest::HTTP_GET);
		objHttpReq.setURI(objUri.getPathAndQuery());
		objHttpReq.setHost(objUri.getHost());

		objHttpCliSess.setHost(objUri.getHost());
		objHttpCliSess.setPort(objUri.getPort());
	}
	void setDownloadPath(const u8string& _path) { m_downloadPath = _path; }
	void setOnDownloadProgress(OnDownloadProgress fn) { m_onDownloadProgress = fn; }
	bool obtainFileSize(uint64_t& dst);
	void downloadFile();

	

private:
	Poco::URI objUri;
	Poco::Net::HTTPClientSession objHttpCliSess;
	Poco::Net::HTTPRequest objHttpReq;
	Poco::Net::HTTPResponse objHttpRes;
	u8string m_downloadPath;

	OnDownloadProgress m_onDownloadProgress;

};

