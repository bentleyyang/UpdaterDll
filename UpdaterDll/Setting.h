#pragma once
#include <string>
class Setting
{
public:
	Setting();
	~Setting();

	static Setting* instance();

	bool parseFile(const std::wstring& path);

	std::wstring getAppLocation() { return m_sAppLocation; }
	std::wstring getAppRoot() { return m_sAppRoot; }
	std::wstring getAppName() { return m_sAppName; }
	std::wstring getAppVersion() { return m_sAppVersion; }

	std::wstring getUrl() { return m_sUrl; }

private:
	std::wstring m_sAppLocation;
	std::wstring m_sAppRoot;
	std::wstring m_sAppName;
	std::wstring m_sAppVersion;
	std::wstring m_sUrl;
};

