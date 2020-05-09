#pragma once

#include <map>
#include <vector>

#include "Setting.h"

using u8string = std::string;
class Path
{
public:
	Path();
	~Path();
	
	static Path* instance();

	void init();
	bool createDirectories(Setting* _setting);

	std::wstring replaceConstants(const std::wstring& _wstr);

	std::wstring getPathUpdater() { return m_sPathUpdater; }
	std::wstring getPathUpdaterTemp() { return m_sPathUpdaterTemp; }

public:
	static std::map<u8string, std::wstring> DefaultPathConstants;//

private:
	static std::wstring m_sPathUpdater;						// Path to updater
	static std::wstring m_sPathUpdaterExecutable;			// Path to updater executable
	static std::wstring m_sPathApp;							// Path to application
	static std::wstring m_sPathUpdaterTemp;					// Path to updater temp files
	static std::wstring m_sPathRollback;					// Path to rollback files
	static std::wstring m_sPathTemp;						// Path to system temp files
	static std::wstring m_sPathProgramfiles;				// Path to program files
	static std::wstring m_sPathSystem;						// Path to system folder
	static std::wstring m_sPathUserProfile;					// Path to user home directory
	static std::wstring m_sPathAppData;						// Path to application data
	static std::wstring m_sPathCommonAppData;				// Path to common application data
	static std::wstring m_sPathQuickLaunchCurrentUser;		// Path to quick launch menu - current user
	static std::wstring m_sPathMenuStartCurrentUser;		// Path to menu start - current user
	static std::wstring m_sPathDesktopCurrentUser;			// Path to desktop - current user
	static std::wstring m_sPathQuickLaunchAllUsers;			// Path to quick launch menu - all users
	static std::wstring m_sPathMenuStartAllUsers;			// Path to menu start - all users
	static std::wstring m_sPathDesktopAllUsers;				// Path to desktop - all users
	static std::wstring m_sPathWindowsDirectory;			// Path to windows directory
	static std::wstring m_sPathUpdateServerPath;			// Server + path where updateinfo is located
	static std::wstring m_sPathUpdateServerFile;			// Server + path + file where updateinfo is located
};

