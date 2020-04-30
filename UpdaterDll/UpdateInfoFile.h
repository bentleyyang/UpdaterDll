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

#include "File.h"
class UpdateInfoFile
{
public:
	UpdateInfoFile();
	~UpdateInfoFile();

	struct GeneralInfo {
		std::string version;
		bool closeApplication;
		bool checkSeparately;
		bool enableRollback;
		bool showErrors;
		bool selfUpdate;
		bool resumDownload;
		std::string license;
		bool forceUpdate;
	};

	static UpdateInfoFile* instance() {
		static UpdateInfoFile updateInfoFile;
		return &updateInfoFile;
	}

	bool parseFile(const std::string& path)
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

			Node *pNode = pRoot->getNodeByPath("/GENERAL");
			if (!parseGeneral(pNode, m_generalInfo)) { return false; }
			
			pNode = pRoot->getNodeByPath("/FILES");
			if (!parseFiles(pNode, m_fileInfos)) { return false; }
			
		}
		catch (std::exception& err) { return false; }
		
		return true;
	}

	const GeneralInfo& generalInfo() { return this->m_generalInfo; }
	const std::vector<FileInfo>& FileInfos() { return this->m_fileInfos; }

	

	
public:
	std::vector<std::string> m_customConstants;//ascii string
	static const std::array<const char*, 19> DefaultPathConstants;//ascii string

private:
	GeneralInfo m_generalInfo;
	std::vector<FileInfo> m_fileInfos;
	

private:
	bool strToBoolean(const std::string& str) 
	{ 
		std::string s;
		std::transform(str.begin(), str.end(), std::back_inserter(s),
			[](unsigned char c) { return ::tolower(c); });
		return s == "true";
	}

	std::string replaceConstants(const std::string& str)
	{
		return str;
	}

	bool parseGeneral(Poco::XML::Node* pNode, GeneralInfo& info);
	bool parseFiles(Poco::XML::Node* pNode, std::vector<FileInfo>& info);
	bool parseActions(Poco::XML::Node* pNode, std::vector<std::shared_ptr<ActionBase>>& info);
};

