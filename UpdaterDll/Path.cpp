#include "pch.h"
#include "Path.h"

#include <algorithm>

#include <direct.h>				// For getting path to current folder
#include <shlobj.h>				// For using SHGetSpecialFolderPath
#include <Shlwapi.h>			// For PathRelativePathTo function

#include "Poco/File.h"
#include "Poco/Path.h"

#include "convert.hpp"
#include "util.hpp"

std::wstring Path::m_sPathUpdater;						// Path to updater
std::wstring Path::m_sPathUpdaterExecutable;			// Path to updater executable
std::wstring Path::m_sPathApp;							// Path to application
std::wstring Path::m_sPathUpdaterTemp;					// Path to updater temp files
std::wstring Path::m_sPathRollback;					// Path to rollback files
std::wstring Path::m_sPathTemp;						// Path to system temp files
std::wstring Path::m_sPathProgramfiles;				// Path to program files
std::wstring Path::m_sPathSystem;						// Path to system folder
std::wstring Path::m_sPathUserProfile;					// Path to user home directory
std::wstring Path::m_sPathAppData;						// Path to application data
std::wstring Path::m_sPathCommonAppData;				// Path to common application data
std::wstring Path::m_sPathQuickLaunchCurrentUser;		// Path to quick launch menu - current user
std::wstring Path::m_sPathMenuStartCurrentUser;		// Path to menu start - current user
std::wstring Path::m_sPathDesktopCurrentUser;			// Path to desktop - current user
std::wstring Path::m_sPathQuickLaunchAllUsers;			// Path to quick launch menu - all users
std::wstring Path::m_sPathMenuStartAllUsers;			// Path to menu start - all users
std::wstring Path::m_sPathDesktopAllUsers;				// Path to desktop - all users
std::wstring Path::m_sPathWindowsDirectory;			// Path to windows directory
std::wstring Path::m_sPathUpdateServerPath;			// Server + path where updateinfo is located
std::wstring Path::m_sPathUpdateServerFile;			// Server + path + file where updateinfo is located

std::map<std::string, std::wstring> Path::DefaultPathConstants;//
Path::Path()
{
}


Path::~Path()
{
}

Path * Path::instance()
{
	static Path ins;
	return &ins;
}

void Path::init()
{
	wchar_t szTemp[_MAX_PATH] = { 0 };
	wchar_t map[_MAX_PATH];

	// Find several paths
	_wgetcwd(map, _MAX_PATH);

	// Updater executable
	GetModuleFileName(NULL, szTemp, _MAX_PATH);
	m_sPathUpdaterExecutable = szTemp;

	// Updater (working) path
	m_sPathUpdater = map;
	//m_sPathUpdater += (L"\\");//TODO:去掉路径最后一个\\

	// Temp path
	GetTempPath(_MAX_PATH, szTemp);
	m_sPathTemp = szTemp;
	if (m_sPathTemp.back() == '\\') { m_sPathTemp.pop_back(); }

	// System path
	GetSystemDirectory(szTemp, _MAX_PATH);
	m_sPathSystem = szTemp;
	//m_sPathSystem += (L"\\");

	// Program files path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROGRAM_FILES, FALSE);
	m_sPathProgramfiles = szTemp;
	//m_sPathProgramfiles += (L"\\");

	// Application data path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_APPDATA, FALSE);
	m_sPathAppData = szTemp;
	//m_sPathAppData += (L"\\");

	// Common application data path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_APPDATA, FALSE);
	m_sPathCommonAppData = szTemp;
	//m_sPathCommonAppData += (L"\\");

	// User home directory path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROFILE, FALSE);
	m_sPathUserProfile = szTemp;
	//m_sPathUserProfile += (L"\\");

	// Quick launch - current user path
	m_sPathQuickLaunchCurrentUser = m_sPathAppData + (L"\\Microsoft\\Internet Explorer\\Quick Launch");

	// Menu start - current user path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_STARTMENU, FALSE);
	m_sPathMenuStartCurrentUser = szTemp;
	//m_sPathMenuStartCurrentUser += (L"\\");

	// Desktop - current user path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_DESKTOPDIRECTORY, FALSE);
	m_sPathDesktopCurrentUser = szTemp;
	//m_sPathDesktopCurrentUser += (L"\\");

	// Quick launch - all users path
	m_sPathQuickLaunchAllUsers = m_sPathCommonAppData + (L"\\Microsoft\\Internet Explorer\\Quick Launch");

	// Menu start - all users path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_STARTMENU, FALSE);
	m_sPathMenuStartAllUsers = szTemp;
	//m_sPathMenuStartAllUsers += (L"\\");

	// Desktop - all users path
	SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
	m_sPathDesktopAllUsers = szTemp;
	//m_sPathDesktopAllUsers += (L"\\");

	// Windows Directory path
	GetWindowsDirectory(szTemp, _MAX_PATH);
	m_sPathWindowsDirectory = szTemp;
	//m_sPathWindowsDirectory += (L"\\");

	DefaultPathConstants["%updater%"] = m_sPathUpdater;
	DefaultPathConstants["%temp%"] = m_sPathTemp;
	DefaultPathConstants["%programfiles%"] = m_sPathProgramfiles;
	DefaultPathConstants["%system%"] = m_sPathSystem;
	DefaultPathConstants["%appdata%"] = m_sPathAppData;
	DefaultPathConstants["%userprofile%"] = m_sPathUserProfile;
	DefaultPathConstants["%commonappdata%"] = m_sPathCommonAppData;
	DefaultPathConstants["%quicklaunchcurrentuser%"] = m_sPathQuickLaunchCurrentUser;
	DefaultPathConstants["%menustartcurrentuser%"] = m_sPathMenuStartCurrentUser;
	DefaultPathConstants["%desktopcurrentuser%"] = m_sPathDesktopCurrentUser;
	DefaultPathConstants["%quicklaunchallusers%"] = m_sPathQuickLaunchAllUsers;
	DefaultPathConstants["%menustartallusers%"] = m_sPathMenuStartAllUsers;
	DefaultPathConstants["%desktopallusers%"] = m_sPathDesktopAllUsers;
	DefaultPathConstants["%windowsdirectory%"] = m_sPathWindowsDirectory;
}

bool Path::createDirectories(Setting * _setting)
{
	m_sPathApp = extract_file_directory(_setting->getAppLocation());
	m_sPathUpdaterTemp = m_sPathTemp + L"/updater_temp_" + _setting->getAppName() + L"/" + _setting->getAppVersion();
	

	try {
		Poco::File dirUpdaterTemp(to_u8(m_sPathUpdaterTemp));
		dirUpdaterTemp.createDirectories();
	}
	catch (std::exception& err) { return false; }
	

	DefaultPathConstants["%app%"] = m_sPathApp;//TODO:
	DefaultPathConstants["%approot%"] = _setting->getAppRoot();//TODO:
	DefaultPathConstants["%updatertemp%"] = m_sPathUpdaterTemp;//
	DefaultPathConstants["%updateserverpath%"] = m_sPathUpdateServerPath;//TODO:
	DefaultPathConstants["%updateserverfile%"] = m_sPathUpdateServerFile;//TODO:

	return true;
}

std::wstring Path::replaceConstants(const std::wstring& _wstr)
{
	std::wstring wstr = _wstr;
	for (auto &&elem : DefaultPathConstants)
	{
		auto it = std::search(wstr.begin(), wstr.end(), elem.first.begin(), elem.first.end());
		if (it != wstr.end())
		{
			wstr.replace(it, it + elem.first.length(), elem.second);
		}
	}
	return wstr;
}

