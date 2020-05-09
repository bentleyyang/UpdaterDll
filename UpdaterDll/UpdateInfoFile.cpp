#include "pch.h"
#include "UpdateInfoFile.h"


UpdateInfoFile::UpdateInfoFile()
{
}


UpdateInfoFile::~UpdateInfoFile()
{
}

bool UpdateInfoFile::parseConstants(Poco::XML::Node * pNode, std::map<std::string, std::wstring>& info)
{
	using namespace Poco;
	using namespace Poco::XML;

	info.clear();
	try {
		AutoPtr<NodeList> list = static_cast<Element*>(pNode)->getElementsByTagName("CONSTANT");
		for (int i = 0; i < list->length(); i++)
		{
			Element *constant = (Element*)list->item(i);
			std::string constantName;
			std::wstring constantVal;

			Attr *attr;
			std::string val;
			attr = constant->getAttributeNode("name");
			val = attr->value();
			constantName = val;

			attr = constant->getAttributeNode("value");
			val = attr->value();
			constantVal = to_wstr(val);

			info.insert({ constantName, constantVal });
		}
	}
	catch (std::exception& err) {
		return false;
	}
	return true;
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

		pChildNode = pNode->getNodeByPath("/LICENSE");
		if (pChildNode && pChildNode->firstChild()) { info.license = pChildNode->firstChild()->nodeValue(); }

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

	info.clear();

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
			fileInfo.m_name = replaceConstants(val);


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
					fileInfo.m_version = replaceConstants(val);
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
			if (attr) { val = attr->value(); fileInfo.m_section = replaceConstants(val); }

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
				if (!attr) { return false; }
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
				else if (type == "delete")
				{
					Element *location = elem->getChildElement("LOCATION");
					std::string src = replaceConstants(location->firstChild()->nodeValue());

					Element *askUser = elem->getChildElement("ASKUSER");
					bool bAskUser = strToBoolean(askUser->firstChild()->nodeValue());
					action = std::make_shared<ActionDelete>(to_wstr(src), bAskUser);
				}
				else if (type == "run")
				{
					Element *location = elem->getChildElement("LOCATION");
					std::string src = replaceConstants(location->firstChild()->nodeValue());

					Element *elemParameters = elem->getChildElement("PARAMETERS");
					std::string parameters = elemParameters->firstChild()->nodeValue();

					Element *elemWait = elem->getChildElement("WAIT");
					bool bWait = strToBoolean(elemWait->firstChild()->nodeValue());
					action = std::make_shared<ActionExecute>(to_wstr(src), to_wstr(parameters), bWait);
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

bool UpdateInfoFile::parseInis(Poco::XML::Node * pNode, std::vector<IniItem>& info)
{
	using namespace Poco;
	using namespace Poco::XML;

	info.clear();

	try {
		AutoPtr<NodeList> list = static_cast<Element*>(pNode)->getElementsByTagName("ITEM");
		for (int i = 0; i < list->length(); i++)
		{
			Element *elemIniItem = (Element*)list->item(i);

			IniItem iniItem;

			Element *e_location = elemIniItem->getChildElement("LOCATION");
			Element *e_group = elemIniItem->getChildElement("GROUP");
			Element *e_key = elemIniItem->getChildElement("KEY");
			Element *e_value = elemIniItem->getChildElement("VALUE");

			if(!e_location||!e_location->firstChild()
				|| !e_group || !e_group->firstChild()
				|| !e_key || !e_key->firstChild()
				|| !e_value || !e_value->firstChild()
				) 
			{
				return false;
			}

			iniItem.location = replaceConstants(e_location->firstChild()->nodeValue());
			iniItem.group = e_group->firstChild()->nodeValue();
			iniItem.key = e_key->firstChild()->nodeValue();
			iniItem.value = replaceConstants(e_value->firstChild()->nodeValue());

			info.push_back(iniItem);
		}
	}
	catch (std::exception& err) {
		return false;
	}
	return true;
}

