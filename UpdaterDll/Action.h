#pragma once

#include <string>
enum class ActionType :int
{
	ACTION_DOWNLOAD,
	ACTION_COPY,
	ACTION_DELETE,
	ACTION_RUN,
	ACTION_REGISTRY,
	ACTION_INI,
	ACTION_UNZIP,
	ACTION_SETFILEATTR,
	ACTION_REGISTER,
};

class ActionBase {
public:
	ActionBase(ActionType type) { this->m_type = type; }
	~ActionBase() {}

	virtual bool doAction() = 0;

	ActionType m_type;
};

class ActionDownload :public ActionBase {
public:
	ActionDownload(const std::wstring& url) :ActionBase(ActionType::ACTION_DOWNLOAD) { this->m_url = url; }
	~ActionDownload() {}

	bool doAction() override {
		std::printf("start download\n");
		return true;
	}

private:
	std::wstring m_url;
};

class ActionCopy :public ActionBase {
public:
	ActionCopy(const std::wstring& src, const std::wstring& dst, bool overwriteOrigin) :ActionBase(ActionType::ACTION_COPY) { 
		this->m_src = src;
		this->m_dst = dst;
		this->m_overwriteOrigin = overwriteOrigin;
	}
	~ActionCopy() {}

	bool doAction() override {
		std::printf("start copy\n");
		return true;
	}

private:
	std::wstring m_src;
	std::wstring m_dst;
	bool m_overwriteOrigin;
};

class ActionDelete :public ActionBase {
public:
	ActionDelete(const std::wstring& path, bool askUser=false) :ActionBase(ActionType::ACTION_DELETE) {
		this->m_path = path;
		this->m_askUser = askUser;
	}

	~ActionDelete() {

	}

	bool doAction() override {
		std::printf("start delete\n");
		return true;
	}

private:
	std::wstring m_path;
	bool m_askUser;
};

class ActionUnzip :public ActionBase {
public:
	ActionUnzip(const std::wstring& src, const std::wstring& dst) :ActionBase(ActionType::ACTION_UNZIP) {
		this->m_src = src;
		this->m_dst = dst;
	}
	~ActionUnzip() {}

	bool doAction() override {
		std::printf("start unzip\n");
		return true;
	}

private:
	std::wstring m_src;
	std::wstring m_dst;
};

class ActionExecute :public ActionBase {
public:
	ActionExecute(const std::wstring& src, const std::wstring& params, bool waitUntilReady) :ActionBase(ActionType::ACTION_RUN) {
		this->m_src = src;
		this->m_params = params;
		this->m_waitUntilReady = waitUntilReady;
	}
	~ActionExecute() {}

	bool doAction() override {
		std::printf("start execute\n");
		return true;
	}

private:
	std::wstring m_src;
	std::wstring m_params;
	bool m_waitUntilReady;
};