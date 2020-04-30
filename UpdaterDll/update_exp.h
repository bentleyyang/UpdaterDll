#pragma once


#include <inttypes.h>
#include <string>

#define UPDATE_EXP __declspec(dllexport)

/*
������Ϣ��
*/
class UPDATE_EXP UpdateInfo
{
public:
	UpdateInfo();
	~UpdateInfo();

	char strVersion[50];//�����汾��
	std::string strUpdateMsg;//����˵����Ϣ
	bool forceUpdate;//�Ƿ�ǿ������
	std::string strLicence;//�û�Э��
};

/*
�ص�����
*/

/*
�յ��µĸ�����Ϣ
���룺UpdateInfo
���أ�bool�������Ƿ�������false��ʾȡ�������������ǿ����������ֵ�������ԣ�
*/
typedef bool(*OnRecvUpdate)(UpdateInfo info);

/*
�����ļ�����֪ͨ
*/
typedef void(*OnDownloadProgress)(int percent);

/*
��װ�ļ�����֪ͨ
*/
typedef void(*OnInstallProgress)(int percent);

/*
������ɽ��
*/
typedef void(*OnDownloadResult)(bool isSuccessful);

/*
��װ��ɽ��
*/
typedef void(*OnInstallResult)(bool isSuccessful);

/*
��ʼ���Լ�������ѯ������Ϣ�߳�
*/
UPDATE_EXP void init(OnRecvUpdate onRecvUpdate
	, OnDownloadProgress  onDownloadProgress
	, OnInstallProgress  onInstallProgress
	, OnDownloadResult  onDownloadResult
	, OnInstallResult  onInstallResult
);

/*
ֹͣ�߳��Լ��ͷ���Դ
*/
UPDATE_EXP void uninit(
);

/*
��ȡ������Ϣ
���룺UpdateInfo��ָ�룬���ڴ洢������Ϣ
���أ�int�ͣ�-1��ʾ��ȡʧ�ܣ�0��ʾ��������Ϣ��1��ʾ��������Ϣ
*/
UPDATE_EXP int get_update_info(
	UpdateInfo *info
);
