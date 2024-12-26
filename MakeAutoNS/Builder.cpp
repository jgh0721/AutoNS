#include "stdafx.h"
#include "Builder.hpp"

#include "cmnPath.hpp"

#include <stdio.h>

#include "cmnString.hpp"
#include "stbFile_Defs.hpp"

using namespace nsCmn;

bool ScriptBuilder::SetFile( const wchar_t* FilePath )
{
    bool IsSuccess = false;

    do
    {
        IsSuccess =  includeFile( FilePath );
        if( IsSuccess == false )
            break;

        IsSuccess = processMacros();
        if( IsSuccess == false )
            break;

    } while( false );

    return IsSuccess;
}

bool ScriptBuilder::SetStub( const std::wstring& StubPath )
{
    if( IsFileExist( StubPath ) == false )
        return false;

    stubFilePath = StubPath;
    return true;
}

void ScriptBuilder::AddIncludeDir( const wchar_t* IncludeDir )
{

}

std::wstring ScriptBuilder::GetCurrentProcessFilePath() const
{
    if( stkProcessFilePath.empty() == true )
        return L"";

    return stkProcessFilePath.top();
}

bool ScriptBuilder::Write( const std::wstring& Dst )
{
    bool IsSuccess = false;
    FILE* File = nullptr;

    do
    {
        if( IsFileExist( Dst ) == true )
            break;

        _wfopen_s( &File, Dst.c_str(), L"wb" );
        if( File == nullptr )
            break;

        // Stub 기록


        // 데이터 기록 시작
        nsStub::TyFirstHDR FirstHDR;
        FirstHDR.Sig[ 0 ] = nsStub::STUB_SIG_1;
        FirstHDR.Sig[ 1 ] = nsStub::STUB_SIG_2;
        FirstHDR.Sig[ 2 ] = nsStub::STUB_SIG_2;
        FirstHDR.Ver = 1;
        FirstHDR.Size = 0;
        FirstHDR.Flags = 0;
        FirstHDR.SplitSize = 0;

        fwrite( &FirstHDR, sizeof( nsStub::TyFirstHDR ), 1, File );


        // PE CheckSum 생성

        
    } while( false );

    if( File != nullptr )
        fclose( File );

    return IsSuccess;
}

bool ScriptBuilder::includeFile( const std::wstring& FilePath )
{
    bool IsSuccess = false;
    FILE* Fp = nullptr;
    char* Buffer = nullptr;

    do
    {
        if( nsCmn::IsFileExist( FilePath ) == false )
            break;

        const auto FileEncode = nsCmn::DetectFileEncode( FilePath );

        _wfopen_s( &Fp, FilePath.c_str(), L"rb");
        if( Fp == nullptr )
            break;

        fseek( Fp, 0, FILE_END );
        auto FileSize = ftell( Fp );

        fseek( Fp, 0, FILE_BEGIN );

        // 파일을 모두 읽기, UTF16LE 인코딩으로 변환하여 저장
        // CRLF 를 이용하여 줄 별로 구분하여 벡터에 저장
        // 한 줄씩 읽으면서 #include 등 처리
        Buffer = ( char* )malloc( FileSize + 2 );
        memset( Buffer, '\0', FileSize + 2 );
        if( fread( Buffer, 1, FileSize, Fp ) != FileSize )
            break;

        WString Text;
        if( FileEncode == nsCmn::FILE_ENC_ANSI )
            Text = convertACPToUTF16LE( Buffer );
        if( FileEncode == nsCmn::FILE_ENC_UTF8 )
            Text = convertUTF8ToUTF16LE( Buffer );

        uint32_t LineNo = 0;
        bool IsExistNextLine = false; bool IsAppendLastLine = false;

        stkProcessFilePath.push( FilePath );

        IsSuccess = true;

        for( auto& Line : simpleTokenizer( std::wstring( Text.Ptr() ), L'\n' ) )
        {
            Line = trim_right( trim_left( Line ) );

            if( endWith( Line, L"_" ) == true )
            {
                Line[ Line.length() - 1 ] = L'\0';
                IsExistNextLine = true;
            }
            else
            {
                IsExistNextLine = false;
            }

            if( IsAppendLastLine == true )
            {
                const auto Count = lines_.size();

                if( Count == 0 )
                    lines_.push_back( Line );
                else
                    lines_[ Count - 1 ] = lines_[ Count - 1 ] + Line;
            }
            else
            {
                const auto Ret = checkDirective( Line );
                if( Ret == 0 && Line.empty() == false )
                    lines_.push_back( Line );

                // 해당 줄 무시
                if( Ret == 1 )
                    ;

                if( Ret == 2 )
                {
                    IsSuccess = false;
                    break;
                }

                // 해당 파일은 이미 포함된 파일이므로 처리 중지
                if( Ret == 3 )
                {
                    IsSuccess = true;
                    break;
                }
            }

            if( IsExistNextLine == true )
                IsAppendLastLine = true;
            else
                IsAppendLastLine = false;
        }

        stkProcessFilePath.pop();

    } while( false );

    if( Buffer != nullptr )
        free( Buffer );

    if( Fp )
        fclose( Fp );

    return IsSuccess;
}

int ScriptBuilder::checkDirective( const std::wstring& Line )
{
    if( startWith( Line, L"#" ) == false )
        return 0;

    if( startWithi( Line, L"#include-once" ) == true )
    {
        if( setIncludedFilePath.count( GetCurrentProcessFilePath() ) > 0 )
        {
            // 해당 파일은 이미 포함시킨 파일이므로 무시한다.
            return 3;
        }

        setIncludedFilePath.insert( GetCurrentProcessFilePath() );
        return 1;
    }

    if( startWithi( Line, L"#include" ) == true )
    {
        std::wstring Include;
        if( parseInclude( Line, Include ) == true )
        {
            // Include 가 성공하면 해당 줄의 내용 대신 전체 파일 내용이 들어갈 것이므로 해당 줄 자체는 무시되어야 한다. 
            if( includeFile( Include ) == true )
                return 1;

            return 2;
        }

        return 2;
    }

    return 0;
}

bool ScriptBuilder::processMacros()
{
    int state = 0;
    int Depth = 0;
    bool IsSuccess = false;
    bool IsSkipLine = false;

    /*!
     *  #ifdef fff
     *  #endif
     *
     *  #ifdef fff
     *  #else
     *  #endif
     *
     *  #ifdef fff
     *  #if fff
     *  #endif
     *  #endif
     *
     */

    for( auto& Line : lines_ )
    {
        auto s = trim_right( trim_left( Line ) );
        if( s.empty() == true )
            continue;

        if( IsSkipLine == true )
        {
            // 일반 문장 건너뛰기
            if( startWith( s, L"#" ) == false )
                continue;
        }

        if( startWith( s, L"#define" ) == true )
        {
            if( IsSkipLine == true )
                continue;

            if( s.length() < 8 )
            {
                // 아무것도 선언하지 않음
                break;
            }

            // NOTE: 현재는 매크로 인자는 지원하지 않음
            s = trim_left( s.substr( 7 ) );
            auto wh = s.find( L' ' );
            std::wstring k, v;

            if( wh == std::wstring::npos )
            {
                k = s;
                v = L"";
            }
            else
            {
                k = s.substr( 0, wh );
                v = trim_left( s.substr( wh ) );
            }

            // 공백이 없다면 단순 선언이다.
            if( mapMacroToValue.find( s ) != mapMacroToValue.end() )
            {
                // 이미 선언한 것을 다시 선언하려 시도한다. 오류!
                break;
            }

            // NOTE: 선언하려는 매크로 이름이 KEYWORD 에 있으면 오류

            mapMacroToValue[ k ] = v;
            continue;
        }

        if( startWith( s, L"#ifdef" ) == true )
        {
            if( IsSkipLine == true )
                continue;

            if( s.length() < 7 )
                break;

            s = trim_left( s.substr( 6 ) );
            if( mapMacroToValue.contains( s ) == false )
                IsSkipLine = true;

            Depth++;
            continue;
        }

        if( startWith( s, L"#else" ) == true )
        {
            if( Depth == 0 )
                break;

            IsSkipLine = !IsSkipLine;
            continue;
        }

        if( startWith( s, L"#elif" ) == true )
        {
            
        }

        if( startWith( s, L"#ifndef" ) == true )
        {
            if( IsSkipLine == true )
                continue;

            if( s.length() < 8 )
                break;

            s = trim_left( s.substr( 7 ) );
            if( mapMacroToValue.contains( s ) == true )
                IsSkipLine = true;

            Depth++;
            continue;
        }

        if( startWith( s, L"#endif" ) == true )
        {
            if( Depth == 0 )
                break;

            IsSkipLine = false;
            Depth--;
            continue;
        }

        if( startWith( s, L"#if" ) == true )
        {
            Depth++;
        }

        // NOTE: 매크로 대체 후 라인 추가
        if( startWith( Line, L"#" ) == true )
            processedLines_.push_back( Line );
        else
            processedLines_.push_back( processMacro( Line ) );
    }

    if( Depth > 0 )
        IsSuccess = false;
    else
        IsSuccess = true;

    return IsSuccess;
}

bool ScriptBuilder::parseInclude( const std::wstring& Line, std::wstring& FullFilePath )
{
    bool IsSuccess = false;

    do
    {
        if( Line.length() < 9 )
            break;

        std::wstring FileName = trim_left( Line.substr( 8 ) );
        wchar_t S = FileName[ 0 ];
        // #include 는 ', ", <> 로 감싸야 한다. 
        if( S != L'"' && S != L'\'' && S != L'<' )
            break;

        if( S == L'<' )
            S = L'>';

        auto E = FileName.find( S, 1 );
        if( E == std::wstring::npos )
            break;

        FileName = FileName.substr( 1, E - 1 );
        
        // NOTE: 로컬에 존재하는지 확인
        const auto Base = PathRemoveFilePart( GetCurrentProcessFilePath() );

        if( IsFileExist( Base + L"\\" + FileName ) == true )
        {
            FullFilePath = Base + L"\\" + FileName;
            IsSuccess = true;
            break;
        }

        for( const auto& Inc : vecIncludeDirs )
        {
            if( IsFileExist( Inc + FileName ) == true )
            {
                FullFilePath = Inc + FileName;
                IsSuccess = true;
                break;
            }
        }
        
    } while( false );

    return IsSuccess;
}

std::wstring ScriptBuilder::processMacro( const std::wstring& Line )
{
    std::wstring Ret;

    do
    {
        auto Vec = lexer( Line );
        for( size_t idx = 0; idx < Vec.size(); ++idx )
            Ret += substitueMacro( Vec[ idx ] );
        
    } while( false );

    return Ret;
}

std::wstring ScriptBuilder::substitueMacro( const std::wstring& Tok )
{
    if( mapMacroToValue.find( Tok ) == mapMacroToValue.end() )
        return Tok;

    return processMacro( mapMacroToValue[ Tok ] );
}

std::vector< std::wstring > ScriptBuilder::lexer( const std::wstring& Line )
{
    std::vector< std::wstring > VecTokens;

    int Pos = 0;
    const wchar_t* L = Line.c_str();

    // NOTE: 이곳에서 매크로 처리를 위해 단순 토큰으로만 분리한다.
    bool IsWh = false;
    while( L[Pos] != L'\0' )
    {
        while( L[ Pos ] == L' ' || L[ Pos ] == L'\t' )
        {
            if( IsWh == false )
                VecTokens.push_back( L" " );
            IsWh = true;
            Pos++;
        }

        IsWh = false;
        if( L[ Pos ] == L'\0' )
            break;

        wchar_t Ch = L[ Pos ];
        // 숫자로 시작하면 숫자
        if( Ch >= L'0' && Ch <= L'9' )
        {
            std::wstring K;
            while( ( Ch >= '0' && Ch <= '9' ) || Ch == L'.' )
            {
                K += Ch;
                Pos++;
                Ch = L[ Pos ];
            }

            VecTokens.push_back( K );
            continue;
        }

        if( ( Ch >= L'A' && Ch <= L'Z' ) || ( Ch >= L'a' && Ch <= L'z' ) || Ch == L'_' )
        {
            std::wstring K;

            while( ( Ch >= 'A' && Ch <= 'Z' ) || ( Ch >= 'a' && Ch <= 'z' ) ||
                    ( Ch >= '0' && Ch <= '9' ) || Ch == '_' )
            {
                K += Ch;
                Pos++;
                Ch = L[ Pos ];
            }

            VecTokens.push_back( K );
            continue;
        }

        // 그 후에, 문자로 시작하면 키워드, 매크로, 함수
        // 아니면 그외에...
        if( Ch != L'\0' )
        {
            VecTokens.emplace_back( std::wstring( 1, Ch ) );
            Pos++;
        }
    }

    return VecTokens;
}
