#include "stdafx.h"
#include "cmnPath.hpp"

#include <filesystem>
#include <__msvc_filebuf.hpp>

namespace nsCmn
{
    bool IsFileExist( const std::wstring& FilePath )
    {
        const DWORD Ret = GetFileAttributesW( FilePath.c_str() );
        if( Ret == INVALID_FILE_ATTRIBUTES )
        {
            if( ::GetLastError() == ERROR_BAD_NETPATH )
            {
                // NOTE: FindFirstFile 등을 이용하여 파일을 검색해야 한다. 
            }

            return false;
        }

        if( Ret & FILE_ATTRIBUTE_DIRECTORY )
            return false;

        return true;
    }

    TyEnFileEncoding DetectFileEncode( const std::wstring& FilePath )
    {
        FILE* File = nullptr;
        TyEnFileEncoding Enc = FILE_ENC_UNK;

        constexpr unsigned char BOM_UTF8[ 3 ] = { 0xEF, 0xBB, 0xBF };
        constexpr unsigned char BOM_UTF16LE[ 2 ] = { 0xFF, 0xFE };

        do
        {
            _wfopen_s( &File, FilePath.c_str(), L"rb" );
            if( File == nullptr )
                break;

            fseek( File, 0, FILE_END );
            const auto Size = ftell( File );
            if( Size < 2 )
                break;

            char BOM[ 3 ] = { 0, };
            fread( BOM, 1, 3, File );

            if( memcmp( BOM, BOM_UTF16LE, 2 ) == 0 )
            {
                Enc = FILE_ENC_UTF16LE;
                break;
            }

            if( memcmp( BOM, BOM_UTF8, 3 ) == 0 )
            {
                Enc = FILE_ENC_UTF8;
                break;
            }

            Enc = FILE_ENC_ANSI;

        } while( false );

        if( File )
            fclose( File );

        return Enc;
    }

    std::wstring NormalizeFilePath( const std::wstring& FilePath )
    {
        return FilePath;
    }

    std::wstring PathRemoveFilePart( const std::wstring& u16filePath )
    {
        std::filesystem::path f( u16filePath );
        if( is_directory( f ) == true )
            return f.wstring();

        return f.parent_path().wstring();
    }
}
