// FileVersion.cpp: implementation of the CFileVersion class.
// by Manuel Laflamme 
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "FileVersion.h"

#include <cassert>

#include "flossy.h"

#pragma comment(lib, "version")

//////////////////////////////////////////////////////////////////////

FileVersion::FileVersion() 
{ 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
}

FileVersion::~FileVersion() 
{ 
    Close();
} 

void FileVersion::Close()
{
	if (m_lpVersionData == NULL) { return; }
    delete[] m_lpVersionData; 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
}

BOOL FileVersion::Open(const wchar_t *lpszModuleName)
{
	if (m_lpVersionData != NULL) { Close(); }
    assert(wcslen(lpszModuleName) > 0);
	assert(m_lpVersionData == NULL);

    // Get the version information size for allocate the buffer
    DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize(lpszModuleName, &dwHandle); 
    if ( dwDataSize == 0 ) 
        return FALSE;

    // Allocate buffer and retrieve version information
    m_lpVersionData = new unsigned char[dwDataSize]; 
    if (!::GetFileVersionInfo(lpszModuleName, dwHandle, dwDataSize, 
	                          (void**)m_lpVersionData) )
    {
        Close();
        return FALSE;
    }

    // Retrieve the first language and character-set identifier
    UINT nQuerySize;
    DWORD* pTransTable;
    if (!::VerQueryValue(m_lpVersionData, L"\\VarFileInfo\\Translation",
                         (void **)&pTransTable, &nQuerySize) )
    {
        Close();
        return FALSE;
    }

    // Swap the words to have lang-charset in the correct format
    m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

    return TRUE;
}

std::wstring FileVersion::QueryValue(const wchar_t *lpszValueName,
	DWORD dwLangCharset /* = 0*/)
{
    // Must call Open() first
    assert(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return std::wstring();

    // If no lang-charset specified use default
    if ( dwLangCharset == 0 )
        dwLangCharset = m_dwLangCharset;

    // Query version information value
    UINT nQuerySize;
    LPVOID lpData;
    std::wstring strValue, strBlockName;
	strBlockName=flossy::format(L"\\StringFileInfo\\{_08x}\\{}", dwLangCharset, lpszValueName);
    if ( ::VerQueryValue((void **)m_lpVersionData, strBlockName.data(), 
	                     &lpData, &nQuerySize) )
        strValue = (wchar_t*)lpData;

    return strValue;
}

BOOL FileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
{
    // Must call Open() first
    assert(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return FALSE;

    UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
    if ( ::VerQueryValue((void **)m_lpVersionData, L"\\",
                         (void**)&pVsffi, &nQuerySize) )
    {
        vsffi = *pVsffi;
        return TRUE;
    }

    return FALSE;
}

std::wstring FileVersion::GetFixedFileVersion()
{
    std::wstring strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
		strVersion=flossy::format(L"{}.{}.{}.{}", HIWORD(vsffi.dwFileVersionMS),
			LOWORD(vsffi.dwFileVersionMS),
			HIWORD(vsffi.dwFileVersionLS),
			LOWORD(vsffi.dwFileVersionLS));
    }
    return strVersion;
}

std::wstring FileVersion::GetFixedProductVersion()
{
    std::wstring strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
		strVersion = flossy::format(L"{}.{}.{}.{}", HIWORD(vsffi.dwProductVersionMS),
			LOWORD(vsffi.dwProductVersionMS),
			HIWORD(vsffi.dwProductVersionLS),
			LOWORD(vsffi.dwProductVersionLS));
    }
    return strVersion;
}