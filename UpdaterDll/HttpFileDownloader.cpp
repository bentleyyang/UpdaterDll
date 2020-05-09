#include "pch.h"
#include "HttpFileDownloader.h"

#include "convert.hpp"
#include "util.hpp"

HttpFileDownloader::HttpFileDownloader()
{
}


HttpFileDownloader::~HttpFileDownloader()
{
}

void HttpFileDownloader::downloadFile()
{
	//TODO:https
	try {
		std::ostream& objHttpResOs = objHttpCliSess.sendRequest(objHttpReq);
		std::istream& objHttpResIs = objHttpCliSess.receiveResponse(objHttpRes);

		std::string mime = objHttpRes.getContentType();
		//TODO:重定向
		//if (objHttpRes.getStatus() == Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND) {}
		if (objHttpRes.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) { throw std::logic_error("file not found"); }
		std::streamsize fileSize = objHttpRes.getContentLength();
		if (fileSize == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH) {
			throw Poco::ProtocolException(objUri.toString());
		}

		Poco::FileStream fs;
		fs.open(m_downloadPath, std::ios::binary | std::ios::out);
		if (fs.fail()) {
			throw Poco::CreateFileException("");
		}

		uint64_t leftSize = fileSize;

		Timer timer;
		if (m_onDownloadProgress) {
			timer.StartTimer(500, [&]() {m_onDownloadProgress(std::min<uint64_t>(fileSize - leftSize, fileSize), fileSize); });
		}

		while (leftSize-- > 0) {
			int dat = objHttpResIs.get();
			if (objHttpResIs.fail()) { 
				throw std::logic_error("objHttpResIs.get() faild"); 
			}
			fs.put(dat);
			if (fs.fail()) { throw std::logic_error("write file faild"); }
			//if (m_onDownloadProgress) { m_onDownloadProgress(fileSize - leftSize, fileSize); }
			bool bEof = objHttpResIs.eof();
			if (objHttpResIs.fail()) { throw std::logic_error("objHttpResIs.eof() faild"); }
			//if (bEof&&leftSize > 0) { throw std::logic_error("objHttpResIs.eof() faild"); }
			if (bEof) { break; }
		}
		timer.Expire();
	}
	catch (std::exception& err) { throw std::exception("network errors"); }

}

void HttpFileDownloader::cancelDownload()
{
	objHttpCliSess.reset();
}

bool HttpFileDownloader::obtainFileSize(uint64_t& dst)
{
	try {
		std::ostream& objHttpResOs = objHttpCliSess.sendRequest(objHttpReq);
		std::istream& objHttpResIs = objHttpCliSess.receiveResponse(objHttpRes);
		std::string mime = objHttpRes.getContentType();
		//重定向
		//if (objHttpRes.getStatus() == Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND) {}
		if (objHttpRes.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) { return false; }
		std::streamsize fileSize = objHttpRes.getContentLength();
		if (fileSize == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH) {
			return false;
		}
		dst = fileSize;
	}
	catch (std::exception& err) { return false; }
	return true;
}
