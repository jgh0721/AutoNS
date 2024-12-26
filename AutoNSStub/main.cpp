#include "stdafx.h"
#include <windows.h>

#include "cmnScript.hpp"

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    int Ret = 0;
    OleInitialize( nullptr );
    SetErrorMode( SEM_NOOPENFILEERRORBOX | SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS );

    do
    {
        int argc = 0;
        wchar_t** argv = CommandLineToArgvW( GetCommandLineW(), &argc );

        if( CheckImage( argv[0]) == false )
        {
            Ret = 1;
            break;
        }

        CScript Script( lpCmdLine );
        
        
    } while( false );

    OleUninitialize();
    return Ret;
}

