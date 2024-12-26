#ifndef __HDR_BUILDER__
#define __HDR_BUILDER__

#include "cmnDataType_String.hpp"

#include <list>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>

class ScriptBuilder
{
public:

    bool SetFile( const wchar_t* FilePath );
    bool SetStub( const std::wstring& StubPath );

    void AddIncludeDir( const wchar_t* IncludeDir );

    // NOTE: 현재 처리하고 있는 파일의 전체 경로 반환
    std::wstring                GetCurrentProcessFilePath() const;

    bool                        Write( const std::wstring& Dst );

private:

    bool                        includeFile( const std::wstring& FilePath );
    // 0 = 추가, 1 = 무시, 2 = ERROR, 3 = 해당 파일은 이미 포함된 파일
    int                         checkDirective( const std::wstring& Line );
    bool                        processMacros();

    bool                        parseInclude( const std::wstring& Line, std::wstring& FullFilePath );
    std::wstring                processMacro( const std::wstring& Line );
    std::wstring                substitueMacro( const std::wstring& Tok );
    
    // 문장을 구분자를 이용해 토큰들로 분리한다. 구분자, ' ', ',', '|' 등... 
    std::vector< std::wstring > lexer( const std::wstring& Line );

    ///////////////////////////////////////////////////////////////////////////
    /// Stub
    ///

    std::wstring                    stubFilePath;

    std::vector< std::wstring >     lines_;
    std::vector< std::wstring >     processedLines_;
    std::vector< std::wstring >     vecIncludeDirs;

    std::stack< std::wstring >      stkProcessFilePath;
    std::set< std::wstring >        setIncludedFilePath;
    std::map< std::wstring, std::wstring > mapMacroToValue;
};

#endif // __HDR_BUILDER__
