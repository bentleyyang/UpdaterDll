#include "pch.h"
#include "File.h"

#include <algorithm>
#include <cmath>
#include "convert.hpp"

const std::string FileInfo::CHECK_TYPE_VERSION = "version";
const std::string FileInfo::CHECK_TYPE_HASH = "hash";
const std::string FileInfo::CHECK_TYPE_DATE = "date";

FileVersion FileInfo::m_fv;

bool FileInfo::checkNeedUpdate(const FileInfo& file)
{
	if (file.m_check == FileInfo::CHECK_TYPE_VERSION)
	{
		if (!m_fv.Open(to_wstr(file.m_location).data())) { throw std::logic_error("can not check file version!"); }
		try {
			bool b = cmpfileversion(m_fv.GetFixedFileVersion(), to_wstr(file.m_version)) < 0;
			return b;
		}catch(std::exception& err){ throw std::logic_error("can not check file version!"); }
	}
	else if (file.m_check == FileInfo::CHECK_TYPE_HASH)
	{
	}
	else if (file.m_check == FileInfo::CHECK_TYPE_DATE)
	{
	}
	return false;
}

int FileInfo::cmpfileversion(const std::wstring& first, const std::wstring& second)
{
	std::vector<uint32_t> versionCodesFirst, versionCodesSecond;

	auto fn = [](const std::wstring& src, std::vector<uint32_t>& dst) {
		std::vector<std::wstring> versionCodesStr;
		split(src, versionCodesStr, std::wstring(L"."));
		if (versionCodesStr.empty()) { throw std::logic_error("version string is invalid"); }
		for (auto &&str : versionCodesStr)
		{
			uint32_t i;
			try {
				i = std::stoul(str);
			}
			catch (std::exception& err) { throw std::logic_error("version string is invalid"); }
			dst.push_back(i);
		}
	};

	fn(first, versionCodesFirst);
	fn(second, versionCodesSecond);

	size_t maxCmpTime = std::min<size_t>(versionCodesFirst.size(), versionCodesSecond.size());
	for (size_t i = 0; i < maxCmpTime; i++)
	{
		if (versionCodesFirst[i] < versionCodesSecond[i]) { return -2; }
		else if (versionCodesFirst[i] > versionCodesSecond[i]) { return 2; }
		else { continue; }
	}
	return 0;
}
