#ifndef __HDR_CMN_STRING__
#define __HDR_CMN_STRING__

#include "cmnDataType_String.hpp"

#include <algorithm>
#include <string>
#include <vector>

int                         wcscmp( const WString& Lhs, const WString& Rhs );
int                         wcsicmp( const WString& Lhs, const WString& Rhs );

bool                        startWith( const std::wstring& str, const std::wstring& with );
bool                        startWithi( const std::wstring& str, const std::wstring& with );
bool                        endWith( const std::wstring& str, const std::wstring& with );
bool                        endWithi( const std::wstring& str, const std::wstring& with );

std::wstring                trim_left( const std::wstring& Str );
std::wstring                trim_right( const std::wstring& Str );

WString                     format( const wchar_t* fmt, ... );

WString                     convertACPToUTF16LE( const char* Buffer, int BufferSize = -1 );
WString                     convertUTF8ToUTF16LE( const char* Buffer, int BufferSize = -1 );

template< typename CHAR >
std::basic_string<CHAR> toStrLower( const std::basic_string<CHAR>& str );

template<>
inline std::string toStrLower( const std::string& str )
{
    std::string ret( str );
    std::transform( ret.begin(), ret.end(), ret.begin(), ::tolower );
    return ret;
}

template<>
inline std::wstring toStrLower( const std::wstring& str )
{
    std::wstring ret( str );
    std::transform( ret.begin(), ret.end(), ret.begin(), ::towlower );
    return ret;
}

template< typename CHAR >
std::basic_string<CHAR> toStrUpper( const std::basic_string<CHAR>& str );

template<>
inline std::string toStrUpper( const std::string& str )
{
    std::string ret( str );
    std::transform( ret.begin(), ret.end(), ret.begin(), ::toupper );
    return ret;
}

template<>
inline std::wstring toStrUpper( const std::wstring& str )
{
    std::wstring ret( str );
    std::transform( ret.begin(), ret.end(), ret.begin(), ::towupper );
    return ret;
}

// 구분자 문자를 사용하여 문자열을 벡터 형태로 분리하여 반환
template< typename CharT >
std::vector< std::basic_string<CharT> > simpleTokenizer( const std::basic_string<CharT>& src, CharT sep = L'|' )
{
    typename std::basic_string<CharT>::size_type sPos = 0;
    typename std::basic_string<CharT>::size_type pos = std::basic_string<CharT>::npos;
    std::vector< std::basic_string<CharT> > vecToekns;
    int idx = 0;

    do
    {
        if( src.empty() == true )
            break;

        pos = src.find( sep );
        if( pos == std::basic_string<CharT>::npos )
        {
            vecToekns.push_back( src );
        }
        else
        {
            while( pos != std::basic_string<CharT>::npos )
            {
                vecToekns.push_back( src.substr( sPos, pos - sPos ) );

                sPos = pos + 1;
                pos = src.find( sep, sPos );

                if( pos == std::basic_string<CharT>::npos )
                {
                    // 여분의 글자가 더 있다면 넣는다.
                    if( src.length() > sPos )
                    {
                        vecToekns.push_back( src.substr( sPos ) );
                    }
                }
            }
        }

    } while( false );

    return vecToekns;
}

#endif // __HDR_CMN_STRING__