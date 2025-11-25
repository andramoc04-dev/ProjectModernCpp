#include "Utils.h"

namespace http
{
    std::vector<std::string> split(const std::string& str, const std::string& delim)
    {
        std::vector<std::string> result;
        size_t startIndex = 0;

        for (size_t found = str.find(delim); found != std::string::npos; found = str.find(delim, startIndex))
        {
            result.emplace_back(str.begin() + startIndex, str.begin() + found);
            startIndex = found + delim.size();
        }
        if (startIndex != str.size())
            result.emplace_back(str.begin() + startIndex, str.end());

        return result;
    }

   
}