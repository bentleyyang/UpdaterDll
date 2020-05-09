#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Action.h"
#include "FileVersion.h"

using u8string = std::string;
class FileInfo {
public:
	FileInfo() {

	}

	~FileInfo() {

	}

	static const std::string CHECK_TYPE_VERSION;
	static const std::string CHECK_TYPE_HASH;
	static const std::string CHECK_TYPE_DATE;

	//check whether this file needs to be update
	static bool checkNeedUpdate(const FileInfo& file);

#ifdef _DEBUG
public:
#else
private:
#endif
	static FileVersion m_fv;

	//compare version
	//return positive number if first version is newer, nagative number if first version is oldder, 0 if two version is equal
	//note:only "." and numbers is valid
	static int cmpfileversion(const std::wstring& first, const std::wstring& second);
	
public:
	u8string m_name;
	u8string m_location;
	u8string m_version;
	u8string m_check;
	u8string m_date;//yyyy-mm-dd
	u8string m_hash;
	u8string m_section;
	std::vector<std::shared_ptr<ActionBase>> m_actions;
	//bool m_needUpdate = false;

};