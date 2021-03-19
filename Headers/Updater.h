#pragma once

#include <urlmon.h>             
#include <WinInet.h>
#pragma comment(lib, "urlmon.lib")  
#pragma comment(lib, "wininet.lib")

#include <fstream>
#include <string>

std::string DownloadBytes(std::string szUrl);
std::string removeSpecialCharacter(std::string s);

inline bool FileExists(const std::string& name)
{
	std::ifstream f(name.c_str());
	return f.good();
}

// return 1 if therws a updae return 2 if there is no update
int Update(std::string url = "user/repo", const char* intallurl = "", const char* dest = "", bool fistbootcheck = false)
{
	std::string str = DownloadBytes("https://api.github.com/repos/" + url + "/releases/latest");

	const char* stack{};
	size_t pos = str.find(":", str.find("tag_name"));
	const size_t lpos = pos;

	while (true)
		if (str.at(pos += 1) == ',' && stack != "") break;

	std::string v1 = str.substr(lpos + 1, (pos - lpos) - 1).c_str();

	if (!FileExists("Verstion"))
	{
		std::ofstream ofs("Verstion", std::ios::app);
		ofs << "Verstion" << " = " << v1 << "\n";
		ofs.close();
	}
	else
	{
		std::string key = "Verstion";

		std::ifstream ifs("Verstion");

		std::string line, value;

		while (std::getline(ifs, line))
		{
			if (line.find(key) != std::string::npos)
			{
				value += line.substr(key.length() + 3);
				break;
			}
		}

		ifs.close();

		int nv1 = atoi(removeSpecialCharacter(v1).c_str());
		int nv2 = atoi(removeSpecialCharacter(value).c_str());

		if (nv1 > nv2)
		{
			URLDownloadToFileA(NULL, intallurl, dest, 0, NULL);
			return 1;
		}
		else return 2;
	}

	if (fistbootcheck == true)
	{
		if (!FileExists("Verstion"))
		{
			std::ofstream ofs("Verstion", std::ios::app);
			ofs << "Verstion" << " = " << v1 << "\n";
			ofs.close();
			return 1;
		}
		else return 2;
	}

	return 2;
}

std::string DownloadBytes(std::string szUrl)
{
	DWORD dwBytesRead = 0;
	std::string output;

	HINTERNET hOpen = InternetOpenW(L"Updater", NULL, NULL, NULL, NULL);
	HINTERNET hFile = InternetOpenUrlA(hOpen, szUrl.c_str(), NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);

	while (true)
	{
		char buffer[2000] = {};
		InternetReadFile(hFile, static_cast<PVOID>(buffer), _countof(buffer), &dwBytesRead);
		if (dwBytesRead == 0) break;
		output.append(static_cast<char*>(buffer), dwBytesRead);
	}

	InternetCloseHandle(hFile);
	InternetCloseHandle(hOpen);
	return output;
}

std::string removeSpecialCharacter(std::string s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s.at(i) < '0' || s.at(i) > '9')
		{
			s.erase(i, 1);
			i--;
		}
	}
	return s;
}