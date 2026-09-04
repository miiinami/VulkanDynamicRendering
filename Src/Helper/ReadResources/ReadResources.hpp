#pragma once
#include<vector>
#include<string>

class ReadResources
{
public:
	static std::vector<char> readShaderFile(const std::string filename);
};