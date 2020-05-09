// FileVersion.h: interface for the CFileVersion class.
// by Manuel Laflamme
//////////////////////////////////////////////////////////////////////

#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
#include <Windows.h>
#include <winver.h>
//#include <verrsrc.h>


class FileVersion
{
	// Construction
public:
	FileVersion();

	// Operations	
public:
	BOOL    Open(const wchar_t *lpszModuleName);
	void    Close();

	std::wstring QueryValue(const wchar_t *lpszValueName, DWORD dwLangCharset = 0);
	std::wstring GetFileDescription() { return QueryValue(L"FileDescription"); };
	std::wstring GetFileVersion() { return QueryValue(L"FileVersion"); };
	std::wstring GetInternalName() { return QueryValue(L"InternalName"); };
	std::wstring GetCompanyName() { return QueryValue(L"CompanyName"); };
	std::wstring GetLegalCopyright() { return QueryValue(L"LegalCopyright"); };
	std::wstring GetOriginalFilename() { return QueryValue(L"OriginalFilename"); };
	std::wstring GetProductName() { return QueryValue(L"ProductName"); };
	std::wstring GetProductVersion() { return QueryValue(L"ProductVersion"); };

	BOOL    GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
	std::wstring GetFixedFileVersion();
	std::wstring GetFixedProductVersion();

	// Attributes
protected:
	unsigned char*  m_lpVersionData;
	DWORD   m_dwLangCharset;

	// Implementation
public:
	~FileVersion();
};

#endif  // __FILEVERSION_H_