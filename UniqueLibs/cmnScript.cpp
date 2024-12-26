#include "stdafx.h"
#include "cmnScript.hpp"

#include "stbFile_Defs.hpp"

#include <windows.h>

#pragma warning( disable: 4996 )

bool CheckImage( const std::wstring& FilePath )
{
    bool IsAutoNSImage = false;

    FILE* fp = _wfopen( FilePath.c_str(), L"rb" );
    if( fp == nullptr )
        return false;

    nsStub::TyFirstHDR HDR = { 0 };
    // 컨테이너의 Stub 은 Sector 단위로 정렬되어 저장되므로, 해당 크기에 맞추어 읽어들이면 된다.
    const size_t MAX_SEEK_POS = 1024 * 1024 * 5;

    size_t CurrentPos = 0;
    size_t ReadBytes = 0;
    int64_t Offset = 0;

    do
    {
        while( ( ReadBytes = fread( &HDR, sizeof( char ), sizeof( nsStub::TyFirstHDR ), fp ) ) > 0 )
        {
            CurrentPos += ReadBytes;
            if( CurrentPos > MAX_SEEK_POS )
                break;

            IsAutoNSImage = IsValidStubHDR( &HDR );
            if( IsAutoNSImage == true )
            {
                Offset = _ftelli64( fp ) - sizeof( nsStub::TyFirstHDR );
                break;
            }
        }

    } while( false );

    if( fp != nullptr )
        fclose( fp );

    return IsAutoNSImage;
}

CScript::CScript( wchar_t* lpCMDLine )
{
    int argc = 0;
    wchar_t** argv = CommandLineToArgvW( GetCommandLineW(), &argc );

    int a = 0;
    if( argc == 0 )
        return;

    currentFilePath_ = argv[ 0 ];
}
