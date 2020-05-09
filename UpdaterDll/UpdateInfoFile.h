#pragma once

#include <ctype.h>
#include <vector>
#include <array>

#include "Poco/AutoPtr.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Attr.h"
#include "Poco/DOM/Node.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/Util/XMLConfiguration.h"

#include "convert.hpp"
#include "File.h"
#include "Path.h"
#include "HttpFileDownloader.h"
using u8string = std::string;
class UpdateInfoFile
{
public:
	UpdateInfoFile();
	~UpdateInfoFile();
	

	struct GeneralInfo {
		u8string version;
		bool closeApplication;
		bool checkSeparately;
		bool enableRollback;
		bool showErrors;
		bool selfUpdate;
		bool resumDownload;
		u8string license;
		bool forceUpdate;
	};

	struct IniItem {
		u8string location;
		u8string group;
		u8string key;
		u8string value;
	};

	static UpdateInfoFile* instance() {
		static UpdateInfoFile updateInfoFile;
		return &updateInfoFile;
	}

	bool downloadFile(const u8string& url, const u8string& dst)
	{
		
		//m_downloader.setUrl(to_u8(Setting::instance()->getUrl()));
		//m_downloader.setDownloadPath(to_u8(Path::instance()->getPathUpdaterTemp() + L"/UpdateFile.xml"));
		m_downloader.setUrl(url);
		m_downloader.setDownloadPath(dst);

		try {
			m_downloader.downloadFile();
		}
		catch (std::exception& e) {
			return false;
		}
		return true;
	}

	bool parseFile(const u8string& path)
	{
		using namespace Poco;
		using namespace Poco::XML;
		DOMParser parser;
		//parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACE_PREFIXES, false);
		AutoPtr<Document> pDoc = parser.parse(path);

		try {
			AutoPtr<NodeList> list = pDoc->childNodes();
			int size = list->length();
			if (size == 0) { return false; }
			Node *pRoot = list->item(0);

			Node *pNode = pRoot->getNodeByPath("/CONSTANTS");
			if (!parseConstants(pNode, m_customConstants)) { return false; }

			pNode = pRoot->getNodeByPath("/GENERAL");
			if (!parseGeneral(pNode, m_generalInfo)) { return false; }
			
			pNode = pRoot->getNodeByPath("/FILES");
			if (!parseFiles(pNode, m_fileInfos)) { return false; }

			pNode = pRoot->getNodeByPath("/INI");
			if (!parseInis(pNode, m_Inis)) { return false; }

			
			
		}
		catch (std::exception& err) { return false; }
		
		return true;
	}

	bool collectValidFiles()
	{
		std::vector<FileInfo> dst;
		for (auto &&file : m_fileInfos)
		{
			try {
				if (!FileInfo::checkNeedUpdate(file)) { continue; }
			}
			catch (std::exception& err) { return false; }
			dst.push_back(file);
		}
		m_fileInfos = dst;
		return true;
	}

	const GeneralInfo& generalInfo() { return this->m_generalInfo; }
	const std::vector<FileInfo>& FileInfos() { return this->m_fileInfos; }

	

	


private:
	HttpFileDownloader m_downloader;
	GeneralInfo m_generalInfo;
	std::vector<FileInfo> m_fileInfos;
	std::map<u8string, std::wstring> m_customConstants;//
	std::vector<IniItem> m_Inis;
	

private:
	bool strToBoolean(const std::string& str) 
	{ 
		std::string s;
		std::transform(str.begin(), str.end(), std::back_inserter(s),
			[](unsigned char c) { return ::tolower(c); });
		return s == "true";
	}

	u8string replaceConstants(const u8string& _u8_str)
	{
		std::wstring wstr = Path::instance()->replaceConstants(to_wstr(_u8_str));
		for (auto &&elem : m_customConstants)
		{
			auto it = std::search(wstr.begin(), wstr.end(), elem.first.begin(), elem.first.end());
			if (it != wstr.end())
			{
				wstr.replace(it, it + elem.first.length(), elem.second);
			}
		}
		return to_u8(wstr);
	}

	bool parseConstants(Poco::XML::Node* pNode, std::map<u8string, std::wstring>& info);
	bool parseGeneral(Poco::XML::Node* pNode, GeneralInfo& info);
	bool parseFiles(Poco::XML::Node* pNode, std::vector<FileInfo>& info);
	bool parseActions(Poco::XML::Node* pNode, std::vector<std::shared_ptr<ActionBase>>& info);
	bool parseInis(Poco::XML::Node* pNode, std::vector<IniItem>& info);
	
};

