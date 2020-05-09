#include "pch.h"
#include "Setting.h"
#include "Path.h"

#include <vector>

#include <Windows.h>
#include <WinBase.h>

#include "convert.hpp"


Setting::Setting()
{
}


Setting::~Setting()
{
}

Setting* Setting::instance()
{
	static Setting ins;
	return &ins;
}

bool Setting::parseFile(const std::wstring & path)
{
	std::vector<wchar_t> sTemp(_MAX_PATH+1, '\0');
	GetPrivateProfileString(L"APPLICATION", L"LOCATION", L"", sTemp.data(), _MAX_PATH, path.data());
	m_sAppLocation = Path::instance()->replaceConstants(sTemp.data());

	GetPrivateProfileString(L"APPLICATION", L"ROOT", L"", sTemp.data(), _MAX_PATH, path.data());
	m_sAppRoot = Path::instance()->replaceConstants(sTemp.data());

	GetPrivateProfileString(L"APPLICATION", L"NAME", L"Unknown app", sTemp.data(), _MAX_PATH, path.data());
	m_sAppName = sTemp.data();

	GetPrivateProfileString(L"APPLICATION", L"VERSION", L"0.0.0.0", sTemp.data(), _MAX_PATH, path.data());
	m_sAppVersion = Path::instance()->replaceConstants(sTemp.data());



	GetPrivateProfileString(L"UPDATEINFO", L"url", L"", sTemp.data(), _MAX_PATH, path.data());
	m_sUrl = Path::instance()->replaceConstants(sTemp.data());
	return false;
}
