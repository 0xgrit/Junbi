#include "../include/discord.h"
#include "../include/json/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <winhttp.h>

void sendDataToDiscord ( const std::string &data )
{
  HINTERNET hSession = WinHttpOpen (
      L"Discord webhook",
      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
      WINHTTP_NO_PROXY_NAME,
      WINHTTP_NO_PROXY_BYPASS,
      0 );

  if ( hSession == NULL )
  {
    std::cerr << "Error in WinHttpOpen" << GetLastError () << std::endl;
    return;
  }

  HINTERNET hConnect = WinHttpConnect (
      hSession,
      L"discord.com",
      INTERNET_DEFAULT_HTTPS_PORT,
      0 );

  if ( hConnect == NULL )
  {
    std::cerr << "Error in WinHttpConnect" << GetLastError () << std::endl;
    WinHttpCloseHandle ( hSession );
    return;
  }

  HINTERNET hRequest = WinHttpOpenRequest (
      hConnect,
      L"POST",
      L"WEBHOOK_URL_HERE",
      NULL,
      WINHTTP_NO_REFERER,
      WINHTTP_DEFAULT_ACCEPT_TYPES,
      WINHTTP_FLAG_SECURE );

  if ( hRequest == NULL )
  {
    std::cerr << "Error in WinHttpOpenRequest" << std::endl
              << GetLastError ();
    WinHttpCloseHandle ( hConnect );
    WinHttpCloseHandle ( hSession );
    return;
  }

  nlohmann::json payload;
  payload["content"] = data;
  std::string jsonPayload = payload.dump ();

  LPCWSTR headers{ L"Content-Type: application/json\r\n" };

  BOOL hSend = WinHttpSendRequest (
      hRequest,
      headers,
      -1L,
      jsonPayload.data (),
      jsonPayload.size (),
      jsonPayload.size (),
      0 );

  if ( hSend == FALSE )
  {
    std::cerr << "Error in WinHttpSendRequest" << GetLastError () << std::endl;
    return;
  }

  WinHttpCloseHandle ( hRequest );
  WinHttpCloseHandle ( hConnect );
  WinHttpCloseHandle ( hSession );
}

std::string parseData ( const std::string &fileName )
{
  std::ifstream logFile ( fileName );

  if ( !logFile.is_open () )
  {
    std::cerr << "Error unable to open log file." << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << "```\n";
  buffer << logFile.rdbuf ();
  buffer << "\n```";

  logFile.close ();

  return buffer.str ();
}
