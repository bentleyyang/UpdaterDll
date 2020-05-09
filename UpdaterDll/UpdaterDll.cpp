// UpdaterDll.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include "flossy.h"

#include "Path.h"
#include "Setting.h"
#include "Download.h"
#include "UpdateInfoFile.h"
#include <crtdbg.h>


#include "HttpFileDownloader.h"
void testDownloadOneFile()
{
	HttpFileDownloader d;
	d.setUrl("http://dl.360safe.com/setup.exe");
	d.setDownloadPath("D:/t/aaaa.exe");
	d.setOnDownloadProgress([](uint64_t c, uint64_t t) {
		std::printf(flossy::format("====={}/{}, {_02d}%\n", c, t, static_cast<int>(static_cast<double>(c)/t*100)).data());
	});
	d.downloadFile();
}

void testObtainFileSize()
{
	HttpFileDownloader d;
	for (int i = 0; i < 102; i++) {
		d.setUrl("http://dl.360safe.com/setup.exe");
		uint64_t dst;
		if (d.obtainFileSize(dst))
		{
			std::printf("%02d:%d\n", i, dst);
		}
		else
		{
			std::printf("=========================Err\n");
		}
	}
}

#include "FileVersion.h"
void testFileInfo()
{
	FileVersion fv;
	if (fv.Open(L"D:/a.ocx"))
	{
		auto ss = fv.GetFixedFileVersion();
		ss = fv.GetProductVersion();
		ss = fv.GetCompanyName();
	}
}

//void testCmpVersion()
//{
//	try {
//		int i = FileInfo::cmpfileversion(L"0.1.3.6.1", L"0.1.3.6");
//	}
//	catch (std::exception& w) {
//		int i = 9;
//	}
//	
//}

#include "update_exp.h"

bool onRecvUpdate(UpdateInfo info)
{
	std::string forma=R"(
===============
version:{}
msg:{}
force:{}
licence:{}
===============
)";
	std::printf(flossy::format(forma, info.strVersion, info.strUpdateMsg, info.forceUpdate, info.strLicence).data());
	return true;
}

void onDownProgress(int p)
{
	std::printf("==%02d%%\n", p);
}

void onDownResult(bool r)
{
	std::printf("==%s\n", r ? "s" : "f");
}

int main()
{
	init(onRecvUpdate, onDownProgress, nullptr, onDownResult, nullptr);

	std::this_thread::sleep_for(std::chrono::seconds(1000));
}

#if 0
int main()
{
	//testObtainFileSize();
	//testDownloadOneFile();
	//testCmpVersion();
    std::cout << "Hello World!\n"; 

	UpdateInfoFile *insUpdateInfos = UpdateInfoFile::instance();
	Download *insDownload = Download::instance();

	//初始化路径常量
	Path::instance()->init();
	Setting::instance()->parseFile(Path::instance()->getPathUpdater() + L"/settings.ini");
	Path::instance()->createDirectories(Setting::instance());

	const std::string infoFileUrl = to_u8(Setting::instance()->getUrl());
	const std::string infoFileLocation = to_u8(Path::instance()->getPathUpdaterTemp() + L"/UpdateFile.xml");
	while (true)
	{
		//下载升级信息
		if (!insUpdateInfos->downloadFile(infoFileUrl, infoFileLocation))
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			//TODO:
			continue;
		}

		//解析升级信息
		if (!insUpdateInfos->parseFile(infoFileLocation))
		{
			//TODO:
			continue;
		}
		if (!insUpdateInfos->collectValidFiles()) 
		{
			//TODO:
			continue;
		}

		auto & files = insUpdateInfos->FileInfos();
		insDownload->collectDownloadInfo(files);
		

		insDownload->setOnDownloadProgress([](int p) {
			std::printf("==%02d%%\n", p);
		});
		insDownload->setOnDownloadResult([](bool r) {
			std::printf("==%s\n", r ? "s" : "f");
		});
		insDownload->startDownload();
		insDownload->waitDownload();
		//insDownload->forceStop();
	}

	
	_CrtDumpMemoryLeaks();
}
#endif

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
