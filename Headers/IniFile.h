#pragma once

#include <fstream>
#include <string>

void InIFileSetValue(std::string filename, std::string key, std::string value, std::ios_base::openmode ios)
{
    std::ofstream ofs(filename, ios);
    ofs << key << " = " << value << "\n";
    ofs.close();
}

std::string InIFileGetValue(std::string filename, std::string key)
{
    std::ifstream ifs(filename);

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

    return value;
}