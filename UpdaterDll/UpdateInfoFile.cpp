#include "pch.h"
#include "UpdateInfoFile.h"

#include "File.h"

#include "convert.hpp"

const std::array<const char*, 19> UpdateInfoFile::DefaultPathConstants = 
{
	"%app%",
	"%approot%",
	"%updater%",
	"%updatertemp%",
	"%temp%",
	"%programfiles%",
	"%system%",
	"%appdata%",
	"%userprofile%",
	"%commonappdata%",
	"%quicklaunchcurrentuser%",
	"%menustartcurrentuser%",
	"%desktopcurrentuser%",
	"%quicklaunchallusers%",
	"%menustartallusers%",
	"%desktopallusers%",
	"%windowsdirectory%",
	"%updateserverpath%",
	"%updateserverfile%",
};

UpdateInfoFile::UpdateInfoFile()
{
}


UpdateInfoFile::~UpdateInfoFile()
{
}

bool UpdateInfoFile::parseGeneral(Poco::XML::Node* pNode, GeneralInfo& info)
{
	using namespace Poco;
	using namespace Poco::XML;

	try {
		Node *pChildNode;
		std::string nodeVal;
		pChildNode = pNode->getNodeByPath("/VERSION")->firstChild();
		info.version = replaceConstants(pChildNode->getNodeValue());

		pChildNode = pNode->getNodeByPath("/CLOSEAPPLICATION")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.closeApplication = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/CHECKSEPARATELY")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.checkSeparately = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/ENABLEROLLBACK")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.enableRollback = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/SHOWERRORS")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.showErrors = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/SELFUPDATE")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.selfUpdate = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/RESUMEDOWNLOADS")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.resumDownload = strToBoolean(nodeVal);

		pChildNode = pNode->getNodeByPath("/LICENSE")->firstChild();
		info.license = pChildNode->getNodeValue();

		pChildNode = pNode->getNodeByPath("/FORCEUPDATE")->firstChild();
		nodeVal = pChildNode->getNodeValue();
		info.forceUpdate = strToBoolean(nodeVal);
	}
	catch (std::exception& err) {
		return false;
	}

	return true;
}

bool UpdateInfoFile::parseFiles(Poco::XML::Node * pNode, std::vector<FileInfo> &info)
{
	using namespace Poco;
	using namespace Poco::XML;

	try {
		AutoPtr<NodeList> list = static_cast<Element*>(pNode)->getElementsByTagName("FILE");
		for (int i = 0; i < list->length(); i++)
		{
			Element *file = (Element*)list->item(i);

			FileInfo fileInfo;
			Attr *attr;
			std::string val;
			attr = file->getAttributeNode("name");
			val = attr->value();
			fileInfo.m_name = val;


			attr = file->getAttributeNode("location");
			val = attr->value();
			fileInfo.m_location = replaceConstants(val);

			{
				attr = file->getAttributeNode("check");
				std::string checkType = attr->value();
				fileInfo.m_check = checkType;
				if (checkType == FileInfo::CHECK_TYPE_VERSION)
				{
					attr = file->getAttributeNode("version");
					val = attr->value();
					fileInfo.m_version = val;
				}
				else if (checkType == FileInfo::CHECK_TYPE_HASH)
				{
					attr = file->getAttributeNode("hash");
					val = attr->value();
					fileInfo.m_hash = val;
				}
				else if (checkType == FileInfo::CHECK_TYPE_DATE)
				{
					attr = file->getAttributeNode("date");
					val = attr->value();
					fileInfo.m_date = val;
				}
			}

			attr = file->getAttributeNode("section");
			if (attr) { val = attr->value(); fileInfo.m_section = val; }

			//±éÀú
			//auto attrs= file->attributes();
			//for (int j = 0; j < attrs->length(); j++)
			//{
			//	Attr* attr = (Attr*)attrs->item(j);
			//	std::string attrName = attr->localName();
			//	std::string attrVal = attr->value();
			//}

			if (!parseActions(file, fileInfo.m_actions)) { return false; }
			info.push_back(fileInfo);
		}
	}
	catch (std::exception& err) {
		return false;
	}
	return true;
}

bool UpdateInfoFile::parseActions(Poco::XML::Node* pNode, std::vector<std::shared_ptr<ActionBase>>& info)
{
	using namespace Poco;
	using namespace Poco::XML;

	try {
		AutoPtr<NodeList> list = static_cast<Element*>(pNode)->getElementsByTagName("ACTION");
		for (int i = 0; i < list->length(); i++)
		{
			std::shared_ptr<ActionBase> action;
			Element *elem = (Element*)list->item(i);
			Attr *attr;
			std::string val;
			{
				attr = elem->getAttributeNode("type");
				std::string type = attr->value();
				if (type=="copy")
				{
					Element *oldLocation=elem->getChildElement("OLDLOCATION");
					std::string src=replaceConstants(oldLocation->firstChild()->nodeValue());
					Element *newLocation = elem->getChildElement("NEWLOCATION");
					std::string dst = replaceConstants(newLocation->firstChild()->nodeValue());
					Element *overwriteOrigin = elem->getChildElement("OVERWRITE");
					bool overwrite = strToBoolean(overwriteOrigin->firstChild()->nodeValue());
					action = std::make_shared<ActionCopy>(to_wstr(src), to_wstr(dst), overwrite);
				}
				else if(type=="download")
				{
					Element *location = elem->getChildElement("LOCATION");
					std::string src = replaceConstants(location->firstChild()->nodeValue());
					action = std::make_shared<ActionDownload>(to_wstr(src));
				}
			}

			if (!action) { return false; }
			info.push_back(action);
			
		}
	}
	catch (std::exception& err) {
		return false;
	}
	return true;
}
