#ifndef __HDR_SCRIPT__
#define __HDR_SCRIPT__

#include <map>
#include <string>
#include <vector>

// 지정한 파일이 스크립트 실행 파일인지 확인
bool CheckImage( const std::wstring& FilePath );

class CScript
{
public:

    CScript( wchar_t* lpCMDLine );

    bool Prepare();

private:

    std::wstring                currentFilePath_;

    struct FileInfo
    {
        uint32_t Id = 0;
        uint32_t DiskId = 0;        // 분할 압축일 경우, 해당 파일이 들어가는 디스크 번호, 확장자는 001, 002... 
        uint32_t Flags = 0;

        uint64_t Size = 0;
        uint64_t CompSize = 0;

        std::wstring SrcName;
        std::wstring SubstitueName;
    };

    std::wstring SrcScriptData;

};

#endif // __HDR_SCRIPT__