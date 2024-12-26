#ifndef __HDR_CMN_PATH__
#define __HDR_CMN_PATH__

#include "cmnDataType_String.hpp"

#include <string>

namespace nsCmn
{
    bool IsFileExist( const std::wstring& FilePath );

    enum TyEnFileEncoding { FILE_ENC_UNK, FILE_ENC_ANSI, FILE_ENC_UTF16LE, FILE_ENC_UTF16BE, FILE_ENC_UTF8 };
    TyEnFileEncoding DetectFileEncode( const std::wstring& FilePath );

    std::wstring NormalizeFilePath( const std::wstring& FilePath );
    std::wstring                    PathRemoveFilePart( const std::wstring& u16filePath );

} // nsCmn

#endif // __HDR_CMN_PATH__