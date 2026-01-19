#include "../include/utils.h"
#include <format>
#include <iostream>

std::string parseDate ()
{
  SYSTEMTIME systemTime;
  GetLocalTime ( &systemTime );

  return std::format ( "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
                       systemTime.wYear, systemTime.wMonth, systemTime.wDay,
                       systemTime.wHour, systemTime.wMinute, systemTime.wSecond );
}

bool checkFileSize ( const std::string &filePath, ULONGLONG targetSize )
{
  std::wstring wide ( filePath.begin (), filePath.end () );
  const wchar_t *wideFilePath = wide.c_str ();

  WIN32_FILE_ATTRIBUTE_DATA attributeData;
  if ( GetFileAttributesExW ( wideFilePath, GetFileExInfoStandard, &attributeData ) )
  {
    ULARGE_INTEGER fileSize;
    fileSize.LowPart = attributeData.nFileSizeLow;
    fileSize.HighPart = attributeData.nFileSizeHigh;
    return fileSize.QuadPart > targetSize;
  }

  std::cerr << "Error " << GetLastError () << std::endl;
  return false;
}
