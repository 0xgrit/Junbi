#pragma once
#include <string>
#include <windows.h>

std::string parseDate ();
bool checkFileSize ( const std::string &filePath, ULONGLONG targetSize );
