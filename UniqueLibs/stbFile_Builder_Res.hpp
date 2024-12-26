#ifndef __HDR_STB_BUILDER_RES__
#define __HDR_STB_BUILDER_RES__

#include <vector>

#include "win32/W32API_PE_Defs.hpp"

// based on NSIS PE Parser
namespace nsStub
{
    namespace nsDetail
    {
        class CPEResDirectory;
        class CPEResDirectoryEntry;
        class CPEResDataEntry;

        class CPEResDirectory
        {
        public:
            CPEResDirectory( nsW32::IMAGE_RESOURCE_DIRECTORY* Res );
            virtual ~CPEResDirectory() = default;

            nsW32::IMAGE_RESOURCE_DIRECTORY         GetInfo() const;
            // 해당 리소스 디렉토리의 크기 반환( 자식 포함 )
            uint32_t                                GetSize();
            // 해당 리소스 디렉토리 삭제( 자식 포함 )
            void                                    Destroy();

            // 지정한 위치의 항목 반환, 없다면 nullptr 
            CPEResDirectoryEntry*                   GetEntry( uint32_t Idx );
            // 리소스 디렉토리에 항목을 추가한다.
            // 항목이 오름차순으로 유지되도록 정렬을 수행한다. ( PE 스펙 )
            void                                    AddEntry( CPEResDirectoryEntry* Entry );
            void                                    RemoveEntry( uint32_t Idx );
            // 리소스 디렉토리에 포함된 항목의 수를 반환한다. 
            uint32_t                                CountEntries() const;
            // 지정한 이름을 가진 항목의 색인 반환
            // 이름은 문자열 또는 Id 일 수 있음.  예). AAA, #1( 첫번째 항목 ), MAKEINTRESOURCE
            // 찾을 수 없으면 UINT_MAX 반환
            uint32_t                                FindIndex( char16_t* Name );
            // 지정한 Id 를 가진항목의 색인 반환
            // 찾을 수 없으면 UINT_MAX 반환
            uint32_t                                FindIndex( uint16_t Id );

            // 리소스 섹션을 다시생성할 때 사용함
            // 메모리 포인터 값이 기록됨( 버퍼 = 리소스 섹션 시작위치 )
            // 멤버변수 = 리소스 섹션 시작위치로부터 해당 항목이 기록된 위치의 포인터
            uint64_t                                m_ulWrittenAt = 0;
        private:

            nsW32::IMAGE_RESOURCE_DIRECTORY         Res;
            std::vector< CPEResDirectoryEntry* >    ResEntryList;
        };


        class CPEResDirectoryEntry
        {
        public:
            CPEResDirectoryEntry( WCHAR* Name, CPEResDirectory* Child );
            CPEResDirectoryEntry( WCHAR* Name, CPEResDataEntry* Data );
            virtual ~CPEResDirectoryEntry();

            uint16_t                GetId() const;

            bool                    IsName() const { return HasName; }
            // 반드시 반환된 포인터를 해제해야한다
            WCHAR* GetName() const;
            size_t                  GetNameLength() const;

            bool                    IsDataDirectory() const { return Value.IsDataDirectory; }
            CPEResDirectory* GetChild();
            CPEResDataEntry* GetData();

            ULONG_PTR               m_ulWrittenAt = 0;
        private:

            uint16_t                Id = 0;
            WCHAR* Name = nullptr;
            bool                    HasName = false;

            struct
            {
                bool                IsDataDirectory = false;
                union
                {
                    CPEResDirectory* Child;
                    CPEResDataEntry* Data;
                };
            } Value;
        };

        class CPEResDataEntry
        {
        public:
            CPEResDataEntry( uint8_t* Data, uint32_t Size, uint32_t CodePage = 0, uint32_t Offset = uint32_t( -1 ) );
            ~CPEResDataEntry();

            uint8_t*            GetData() { return Data; };
            void                SetData( uint8_t* Data, uint32_t Size );
            void                SetData( uint8_t* Data, uint32_t Size, uint32_t CodePage );

            uint32_t            GetSize() const { return Size; }
            uint32_t            GetCodePage() const { return CodePage; }
            uint32_t            GetOffset() const { return Offset; }

            uint64_t            m_ulWrittenAt = 0;

        private:

            uint8_t*            Data = nullptr;
            uint32_t            Size = 0;
            uint32_t            CodePage = 0;
            uint32_t            Offset = 0;
        };

    } // nsDetail
    
    class CPEResEditor
    {
    public:
        void                        SetImage( uint8_t* Buffer, size_t Size );
        void                        SetImage( const std::vector< uint8_t >& Image );

        std::vector<uint8_t>        GetResource( char16_t* Type, uint16_t Name, uint16_t Language );
        int                         GetResourceSize( char16_t* Type, uint16_t Name, uint16_t Language );
        uint32_t                    GetResourceOffset( char16_t* Type, uint16_t Name, uint16_t Language );
        std::vector< uint16_t >     GetResourceIdList( char16_t* Type, uint16_t Name );

        bool                        UpdResource( char16_t* Type, uint16_t Name, uint16_t Language, const std::vector< uint8_t >& Buffer );

        /*!
         * @brief 편집기에 반영된 리소스 변경사항 들을 적용한 Stub 를 반환한다
        */
        std::vector<uint8_t>        SaveTo();

    private:
        void                        scan();
        // Scan 으로 지정한 리소스 디렉토리 하부를 검색하여 클래스 구조로 생성하여 반환한다
        nsDetail::CPEResDirectory*  scanDirectory( nsW32::RESOURCE_DIRECTORY* Root, nsW32::RESOURCE_DIRECTORY* Scan );

        // Returns a copy of the requested resource
        // Returns 0 if the requested resource can't be found
        std::vector<uint8_t>    getResource( WCHAR* Type, WCHAR* Name, LANGID Language );
        // Returns the size of the requested resource
        // Returns -1 if the requested resource can't be found
        int                     getResourceSize( WCHAR* Type, WCHAR* Name, LANGID Language );
        // Returns the offset of the requested resource in the original PE
        // Returns -1 if the requested resource can't be found
        DWORD                   getResourceOffset( WCHAR* Type, WCHAR* Name, LANGID Language );

        // Adds/Replaces/Removes a resource.
        // If Buffer is empty UpdateResource removes the resource.
        bool                    updResource( WCHAR* Type, WCHAR* Name, LANGID Language, const std::vector< uint8_t >& Buffer );
        /*!
         * @brief 해당 버퍼에 리소스 섹션을 기록한다
         *
         * 반드시 해당 버퍼는 모든 리소스 섹션을 기록하기에 충분히 커야한다
         * This function writes into a given place in memory (pbRsrcSec) the edited resource section
        */
        void                    writeRsrcSecTo( uint8_t* Buffer );
        void                    setOffsets( nsDetail::CPEResDirectory* RsrcDir, ULONG_PTR From );
        uint32_t                patchRVAs( std::vector< uint8_t>& Src, std::vector< uint8_t>& Dst,
                                           IMAGE_NT_HEADERS* SrcNtHDR, IMAGE_NT_HEADERS* DstNtHR,
                                           uint32_t Delta, uint32_t RVADelta );

        uint32_t                    calcSizeOfImage( PVOID ImageBase );
        uint32_t                    calcSizeOfHeaders( PVOID ImageBase );

        IMAGE_SECTION_HEADER*       findSecHDRByRVA( PVOID ImgBase, uint32_t RVA, uint32_t* SecIndex = NULL );
        IMAGE_SECTION_HEADER*       findSecHDRByOffset( PVOID ImgBase, uint32_t Offset );

        // from reactos
        IMAGE_NT_HEADERS*           get_nt_header( void* base, uint32_t mapping_size );
        IMAGE_SECTION_HEADER*       get_section_header( void* base, uint32_t mapping_size, uint32_t* num_sections );
        IMAGE_SECTION_HEADER*       get_resource_section( void* base, uint32_t mapping_size );
        uint32_t                    get_init_data_size( void* base, uint32_t mapping_size );
        void                        res_write_padding( uint8_t* res_base, uint32_t Size );

        std::vector< uint8_t >      img_;
        nsW32::IMAGE_NT_HEADERS32*  imgNtHDR_ = nullptr;
        uint32_t*                   imgNtHDRCRC_ = nullptr;
        uint32_t                    dwResourceSectionIndex_ = 0;
        uint32_t                    dwResourceSectionVA_ = 0;
        nsDetail::CPEResDirectory*  imgResDir_ = nullptr;
    };
}

#endif // __HDR_STB_BUILDER_RES__