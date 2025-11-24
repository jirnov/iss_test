#pragma once

#include <string>
#include <vector>

std::vector<char> createBinaryPacket(const std::vector<char>& data);
std::vector<char> createTextPacket(const std::string& text);
