#include "stdafx.h"
#include "W32API_PE_Defs.hpp"

namespace nsW32
{
    IMAGE_NT_HEADERS32* GetNTHeaders( uint8_t* ImageBase )
    {
        IMAGE_NT_HEADERS32* Ret = nullptr;

        do
        {
            // Get dos header
            const auto dosHeader = reinterpret_cast< PIMAGE_DOS_HEADER >( ImageBase );
            if( dosHeader->e_magic != IMAGE_DOS_SIG )
                break;

            // Get NT headers
            const auto ntHeaders = static_cast< IMAGE_NT_HEADERS32* >( Add2Ptr( ImageBase, dosHeader->e_lfanew ) );
            if( ntHeaders->Signature != IMAGE_N_T_SIG )
                break;

            // Make sure this is a supported PE format
            if( ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPT_HDR32_MAGIC &&
                ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPT_HDR64_MAGIC )
                break;

            Ret = ntHeaders;

        } while( false );

        return Ret;
    }

    RESOURCE_DIRECTORY* GetResourceDirectory( uint8_t* ImageBase, uint32_t Size, IMAGE_NT_HEADERS32* NtHeaders, uint32_t* pdwResSecVA, uint32_t* pdwSectionIndex )
    {
        RESOURCE_DIRECTORY* Result          = nullptr;
        PIMAGE_DATA_DIRECTORY dataDirectory = *GetMemberFromNtOptionalHDR( NtHeaders->OptionalHeader, DataDirectory );
        uint32_t dwNumberOfRvaAndSizes      = *GetMemberFromNtOptionalHDR( NtHeaders->OptionalHeader, NumberOfRvaAndSizes );

        do
        {
            if( dwNumberOfRvaAndSizes <= IMAGE_DIRECTORY_SEC_ENTRY_RESOURCE )
                break;

            // Get resource section virtual address
            uint32_t dwResSecVA = dataDirectory[ IMAGE_DIRECTORY_SEC_ENTRY_RESOURCE ].VirtualAddress;

            // Pointer to the sections headers array
            PIMAGE_SECTION_HEADER sectionHeadersArray = GetFirstSection( NtHeaders );

            uint32_t dwSectionIndex = UINT32_MAX;
            
            // Find resource section index in the array
            for( int i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++ )
            {
                if( dwResSecVA == sectionHeadersArray[ i ].VirtualAddress )
                {
                    // Remember resource section index
                    dwSectionIndex = i;

                    // Check for invalid resource section pointer
                    if( !sectionHeadersArray[ i ].PointerToRawData )
                        dwSectionIndex = UINT32_MAX;

                    break;
                }

                // Invalid section pointer (goes beyond the PE image)
                if( sectionHeadersArray[ i ].PointerToRawData > Size )
                {
                    dwSectionIndex = UINT32_MAX;
                    break;
                }
            }

            // No resource section...
            if( dwSectionIndex == UINT32_MAX )
                break;

            // Return extra parameters
            if( pdwSectionIndex )
                *pdwSectionIndex = dwSectionIndex;
            if( pdwResSecVA )
                *pdwResSecVA = dwResSecVA;

            // Pointer to section data, the first resource directory
            uint32_t dwResSecPtr = sectionHeadersArray[ dwSectionIndex ].PointerToRawData;
            Result = reinterpret_cast< PRESOURCE_DIRECTORY >( ImageBase + dwResSecPtr );

        } while( false );

        return Result;
    }

}
