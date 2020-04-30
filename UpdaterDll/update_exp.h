#pragma once


#include <inttypes.h>
#include <string>

#define UPDATE_EXP __declspec(dllexport)

/*
升级信息类
*/
class UPDATE_EXP UpdateInfo
{
public:
	UpdateInfo();
	~UpdateInfo();

	char strVersion[50];//升级版本号
	std::string strUpdateMsg;//升级说明信息
	bool forceUpdate;//是否强制升级
	std::string strLicence;//用户协议
};

/*
回调函数
*/

/*
收到新的更新信息
输入：UpdateInfo
返回：bool，代表是否升级，false表示取消升级（如果是强制升级，该值将被忽略）
*/
typedef bool(*OnRecvUpdate)(UpdateInfo info);

/*
下载文件进度通知
*/
typedef void(*OnDownloadProgress)(int percent);

/*
安装文件进度通知
*/
typedef void(*OnInstallProgress)(int percent);

/*
下载完成结果
*/
typedef void(*OnDownloadResult)(bool isSuccessful);

/*
安装完成结果
*/
typedef void(*OnInstallResult)(bool isSuccessful);

/*
初始化以及启动轮询升级信息线程
*/
UPDATE_EXP void init(OnRecvUpdate onRecvUpdate
	, OnDownloadProgress  onDownloadProgress
	, OnInstallProgress  onInstallProgress
	, OnDownloadResult  onDownloadResult
	, OnInstallResult  onInstallResult
);

/*
停止线程以及释放资源
*/
UPDATE_EXP void uninit(
);

/*
获取更新信息
输入：UpdateInfo型指针，用于存储升级信息
返回：int型，-1表示获取失败，0表示无升级信息，1表示有升级信息
*/
UPDATE_EXP int get_update_info(
	UpdateInfo *info
);
