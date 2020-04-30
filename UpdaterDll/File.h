#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Action.h"

class FileInfo {
public:
	FileInfo() {

	}

	~FileInfo() {

	}

	static const std::string CHECK_TYPE_VERSION;
	static const std::string CHECK_TYPE_HASH;
	static const std::string CHECK_TYPE_DATE;
	

	std::string m_name;
	std::string m_location;
	std::string m_version;
	std::string m_check;
	std::string m_date;//yyyy-mm-dd
	std::string m_hash;
	std::string m_section;
	std::vector<std::shared_ptr<ActionBase>> m_actions;

};