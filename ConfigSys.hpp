#pragma once

class CConfigSystem
{
private:
	std::string ConfigPath;

public:
	CConfigSystem() = default;
	~CConfigSystem() = default;

	void LoadConfig(std::string path);
	void SaveConfig(std::string path);

	std::string GetConfigFolder();
};

extern CConfigSystem config_system;

