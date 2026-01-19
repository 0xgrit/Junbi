#include "../include/logger.h"

namespace Logger
{
std::string fileName{ "logs.txt" };
std::ofstream outputFile ( fileName, std::ios::out | std::ios::app );
std::stringstream output;
} // namespace Logger
