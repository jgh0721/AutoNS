#include "stdafx.h"
#include "cmnString.hpp"

#include <stdio.h>

namespace nsDetail
{
    WString format_arg_list( const wchar_t* fmt, va_list args )
    {
        if( !fmt ) return L"";
        int   result = -1, length = 512;
        wchar_t* buffer = nullptr;
        while( result == -1 )
        {
            if( buffer )
                free( buffer );
            buffer = static_cast< wchar_t* >( malloc( ( length + 1 ) * sizeof( wchar_t ) ) );;
            memset( buffer, 0, ( length + 1 ) * sizeof( wchar_t ) );
            result = _vsnwprintf_s( buffer, length, _TRUNCATE, fmt, args );
            length *= 2;
        }

        WString s;
        s.SetBuffer( buffer );
        return s;
    }
}

int wcscmp( const WString& Lhs, const WString& Rhs )
{
    return ::wcscmp( Lhs.Ptr(), Rhs.Ptr() );
}

int wcsicmp( const WString& Lhs, const WString& Rhs )
{
    return ::_wcsicmp( Lhs.Ptr(), Rhs.Ptr() );
}

bool startWith( const std::string& str, const std::string& with )
{
    if( str.length() >= with.length() )
        return ( 0 == str.compare( 0, with.length(), with ) );

    return false;
}

bool startWithi( const std::string& str, const std::string& with )
{
    return startWith( toStrLower( str ), toStrLower( with ) );
}

bool startWith( const std::wstring& str, const std::wstring& with )
{
    if( str.length() >= with.length() )
        return ( 0 == str.compare( 0, with.length(), with ) );

    return false;
}

bool startWithi( const std::wstring& str, const std::wstring& with )
{
    return startWith( toStrLower( str ), toStrLower( with ) );
}

bool endWith( const std::string& str, const std::string& with )
{
    if( str.length() >= with.length() )
        return ( 0 == str.compare( str.length() - with.length(), with.length(), with ) );
    return false;
}

bool endWithi( const std::string& str, const std::string& with )
{
    return endWith( toStrLower( str ), toStrLower( with ) );
}

bool endWith( const std::wstring& str, const std::wstring& with )
{
    if( str.length() >= with.length() )
        return ( 0 == str.compare( str.length() - with.length(), with.length(), with ) );
    return false;
}

bool endWithi( const std::wstring& str, const std::wstring& with )
{
    return endWith( toStrLower( str ), toStrLower( with ) );
}

std::wstring trim_left( const std::wstring& Str )
{
    std::wstring::size_type n = Str.find_first_not_of( L" \t\v\n\r\f" );
    return n == std::wstring::npos ? L"" : Str.substr( n, Str.length() );
}

std::wstring trim_right( const std::wstring& Str )
{
    std::wstring::size_type n = Str.find_last_not_of( L" \t\v\n\r\f" );
    return n == std::wstring::npos ? L"" : Str.substr( 0, n + 1 );
}

WString format( const wchar_t* fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    auto s = nsDetail::format_arg_list( fmt, args );
    va_end( args );
    return s;
}

WString convertACPToUTF16LE( const char* Buffer, int BufferSize )
{
    int Ret = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, Buffer, BufferSize, nullptr, 0 );
    if( Ret == 0 )
        return {};

    wchar_t* Dst = static_cast< wchar_t* >( malloc( ( Ret + 1 ) * sizeof( wchar_t ) ) );
    if( Dst == nullptr )
        return {};

    Ret = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, Buffer, BufferSize, Dst, Ret );
    if( Ret == 0 )
    {
        free( Dst );
        return {};
    }

    WString Str;
    Str.SetBuffer( Dst );
    return Str;
}

WString convertUTF8ToUTF16LE( const char* Buffer, int BufferSize )
{
    int Ret = MultiByteToWideChar( CP_UTF8, 0, Buffer, BufferSize, nullptr, 0 );
    if( Ret == 0 )
        return {};

    wchar_t* Dst = static_cast< wchar_t* >( malloc( ( Ret + 1 ) * sizeof( wchar_t ) ) );
    if( Dst == nullptr )
        return {};

    Ret = MultiByteToWideChar( CP_UTF8, 0, Buffer, BufferSize, Dst, Ret );
    if( Ret == 0 )
    {
        free( Dst );
        return {};
    }

    WString Str;
    Str.SetBuffer( Dst );
    return Str;
}
