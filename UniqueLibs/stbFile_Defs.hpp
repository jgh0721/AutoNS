#ifndef __HDR_STB_DEFS__
#define __HDR_STB_DEFS__

#include <cstdint>

namespace nsStub
{
#pragma pack(push)
#pragma pack(1)
    const uint32_t STUB_SIG_1 = 0x73590290;
    const uint32_t STUB_SIG_2 = 0x47492605;
    const uint32_t STUB_SIG_3 = 0x77008227;

    const uint32_t STUB_SIG_4 = 0x00000000;
    const uint32_t STUB_SIG_5 = 0x00000000;
    const uint32_t STUB_SIG_6 = 0x00000000;

    /*!
     *  헤더 출현 순서
     *  TyFirstHDR
     *  TyDirectiveHDR
     *  TyScriptHDR
     *  TyFileListHDR
     *  TyFileEntry
     *  SIG
     *  BinaryData      SOLID COMPRESSION 을 사용하지 않는다면 TyFileEntry 에 나온 순서대로 파일의 이진 데이터가 나타난다. 
     */

    struct TyStringItem
    {
        uint32_t            Flags       = 0;
        uint32_t            Length      = 0;         // include null-terminate char
        char16_t            Value[ 1 ]  = { 0 };
    };

    struct TyStringDict
    {
        uint32_t            Count = 0;

        // count 수량만큼 TyStringItem 이 key-value 쌍으로 나타난다.
        // 반드시 key - value 모두 존재해야한다.
        // value 에 값을 할당하고 싶지 않을 때에는 length = 1, value 에는 L'\0' 를 할당한다
    };

    union TyFirstHDR
    {
        struct
        {
            uint32_t            Sig[ 3 ];
            uint32_t            Ver;
            uint32_t            Size;               // TyFirstHDR 를 포함한 모든 헤더 크기, 파일크기는 포함하지 않음 
            uint32_t            Flags;

            uint64_t            SplitSize;          // 분할 생성이라면 기준 분할 크기를 적는다. 확장자는 001, 002 ... 
        };

        uint8_t                 HDR[ 512 ] = { 0, };
    };

    struct TyDirectiveHDR
    {
        uint32_t            Count = 0;

        // TyStringItem 이 Key-Value 형태로 연속되어 나타난다.
    };

    struct TyScriptHDR
    {
        uint32_t            Size = 0;               //
        char16_t            Script[ 1 ] = { 0 };    // 스크립트 데이터, MakeAutoNS 에 의해 처리되어 하나의 파일로 생성된 후 해당 내용이 들어간다. 
    };

    struct TyFileListHDR
    {
        uint32_t            Count = 0;
        uint32_t            Flags = 0;

        uint64_t            Size = 0;
        uint64_t            CompSize = 0;

        // count 수량만큼 TyFileEntry 가 존재한다
    };

    enum TyEnFileEntryFlags
    {
        FILE_ENTRY_NONE = 0x0,
    };

    struct TyFileEntry
    {
        uint32_t            Id          = 0;
        uint32_t            Flags       = 0;
        uint64_t            Size        = 0;        // filesize, 원본 크기( 압축해제된 크기 )
        uint64_t            CompSize    = 0;        // 압축된 크기

        TyStringItem        Name;
        TyStringItem        SubstitueName;
    };
#pragma pack(pop)
}

#endif // __HDR_STB_DEFS__