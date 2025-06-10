#pragma once
class CSettings
{
public:
	CSettings();
	~CSettings();

	void SetPath(const CString& path) { m_Path = path; }
	CString GetPath() const { return m_Path; }

	bool Load();
	bool Save();

protected:
	bool GetPath(CString& path, CString filename);

	CString SettingsToString();

	void StringToSettings(CString settingsTxt);

private:
	CString m_Path;
};
