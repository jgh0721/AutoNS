#include "stdafx.h"
#include "stbFile_Builder_Res.hpp"

#include "cmnString.hpp"

#include <string>

namespace nsStub
{
    namespace nsDetail
    {
        CPEResDirectory::CPEResDirectory( nsW32::IMAGE_RESOURCE_DIRECTORY* Res )
        {
            memset( &this->Res, '\0', sizeof( nsW32::IMAGE_RESOURCE_DIRECTORY ) );

            this->Res                       = *Res;
            this->Res.NumberOfIdEntries     = 0;
            this->Res.NumberOfNamedEntries  = 0;
        }

        nsW32::IMAGE_RESOURCE_DIRECTORY CPEResDirectory::GetInfo() const
        {
            return Res;
        }

        uint32_t CPEResDirectory::GetSize()
        {
            uint32_t dwSize = sizeof( nsW32::IMAGE_RESOURCE_DIRECTORY );

            for( size_t i = 0; i < ResEntryList.size(); ++i )
            {
                dwSize += sizeof( nsW32::IMAGE_RESOURCE_DIRECTORY_ENTRY );
                if( ResEntryList[ i ]->IsName() )
                    dwSize += static_cast< uint32_t >( sizeof( nsW32::IMAGE_RESOURCE_DIR_STRING_U ) + ( ResEntryList[ i ]->GetNameLength() + 1 ) * sizeof( char16_t ) );
                if( ResEntryList[ i ]->IsDataDirectory() )
                    dwSize += ResEntryList[ i ]->GetChild()->GetSize();
                else
                {
                    const uint32_t dwAligned = ResEntryList[ i ]->GetData()->GetSize();
                    dwSize += sizeof( nsW32::IMAGE_RESOURCE_DATA_ENTRY ) + nsW32::AlignToSize( dwAligned, 8 );
                }
            }

            return dwSize;
        }

        void CPEResDirectory::Destroy()
        {
            for( size_t i = 0; i < ResEntryList.size(); ++i )
            {
                if( ResEntryList[ i ]->IsDataDirectory() )
                {
                    ResEntryList[ i ]->GetChild()->Destroy();
                    delete ResEntryList[ i ]->GetChild();
                }
                else
                    delete ResEntryList[ i ]->GetData();
            }
        }

        CPEResDirectoryEntry* CPEResDirectory::GetEntry( uint32_t Idx )
        {
            if( ResEntryList.size() < Idx )
                return nullptr;

            return ResEntryList[ Idx ];
        }

        void CPEResDirectory::AddEntry( CPEResDirectoryEntry* Entry )
        {
            if( Entry->IsName() )
            {
                char16_t* szEntName = Entry->GetName();
                for( uint16_t i = 0; i < Res.NumberOfNamedEntries; i++ )
                {
                    // NOTE: On *nix, char16_t -> wchar_t convert 
                    char16_t* szName = ResEntryList[ i ]->GetName();
                    const int cmp = wcsicmp( reinterpret_cast< wchar_t* >( szName ),
                                             reinterpret_cast< wchar_t* >( szEntName ) );
                    delete[] szName;
                    if( cmp == 0 )
                    {
                        delete[] szEntName;
                        return;
                    }
                    if( cmp > 0 )
                        break;
                }
                delete[] szEntName;
                Res.NumberOfNamedEntries++;
            }
            else
            {
                for( uint16_t i = Res.NumberOfNamedEntries; i < Res.NumberOfNamedEntries + Res.NumberOfIdEntries; i++ )
                {
                    if( ResEntryList[ i ]->GetId() == Entry->GetId() )
                        return;
                    if( ResEntryList[ i ]->GetId() > Entry->GetId() )
                        break;
                }
                Res.NumberOfIdEntries++;
            }
            ResEntryList.insert( ResEntryList.begin() + i, Entry );
        }

        void CPEResDirectory::RemoveEntry( uint32_t Idx )
        {
            if( ResEntryList[ Idx ]->IsName() )
                Res.NumberOfNamedEntries--;
            else
                Res.NumberOfIdEntries--;
            delete ResEntryList[ Idx ];
            ResEntryList.erase( ResEntryList.begin() + Idx );
        }

        uint32_t CPEResDirectory::CountEntries() const
        {
            return static_cast< uint32_t >( ResEntryList.size() );
        }

        uint32_t CPEResDirectory::FindIndex( char16_t* Name )
        {
            if( IS_INTRESOURCE( Name ) )
                return FindIndex( ( uint16_t )( ULONG_PTR )Name );

            if( Name[ 0 ] == L'#' )
                return FindIndex( static_cast< uint16_t >( nsCmn::stoi( reinterpret_cast< wchar_t* >( Name + 1 ) ) ) );

            for( unsigned int i = 0; i < ResEntryList.size(); i++ )
            {
                if( !ResEntryList[ i ]->IsName() )
                    continue;
                
                const char16_t* szEntName = ResEntryList[ i ]->GetName();
                const int cmp = _wcsicmp( reinterpret_cast< wchar_t* >( Name ),
                                          reinterpret_cast< wchar_t* >( szEntName ) );
                delete[] szEntName;

                if( !cmp )
                    return i;
            }

            return UINT_MAX;
        }

        uint32_t CPEResDirectory::FindIndex( uint16_t Id )
        {
            for( unsigned int i = 0; i < ResEntryList.size(); i++ )
            {
                if( ResEntryList[ i ]->IsName() )
                    continue;

                if( Id == ResEntryList[ i ]->GetId() )
                    return i;
            }

            return UINT_MAX;
        }
    } // nsDetail

    void CPEResEditor::SetImage( uint8_t* Buffer, size_t Size )
    {
        img_.clear();
        img_.resize( Size );
        memcpy( img_.data(), Buffer, Size );

        scan();
    }

    void CPEResEditor::SetImage( const std::vector<uint8_t>& Image )
    {
        img_ = Image;

        scan();
    }

    void CPEResEditor::scan()
    {
        imgNtHDR_ = nsW32::GetNTHeaders( img_.data() );
        if( imgNtHDR_ == nullptr )
            return;

        imgNtHDRCRC_ = GetMemberFromNtOptionalHDR( imgNtHDR_->OptionalHeader, CheckSum );
        // 수정 후 기록을 위해 CRC 값을 초기화한다
        *imgNtHDRCRC_ = 0;

        const auto root = nsW32::GetResourceDirectory( img_.data(), static_cast< DWORD >( img_.size() ),
                                                       imgNtHDR_,
                                                       &dwResourceSectionVA_,
                                                       &dwResourceSectionIndex_ );
        if( root == nullptr )
            return;

        imgResDir_ = scanDirectory( root, root );
    }

    nsDetail::CPEResDirectory* CPEResEditor::scanDirectory( nsW32::RESOURCE_DIRECTORY* Root, nsW32::RESOURCE_DIRECTORY* Scan )
    {
        nsDetail::CPEResDirectory* Ret = nullptr;

        if( Root == nullptr || Scan == nullptr )
            return Ret;

        Ret = new nsDetail::CPEResDirectory( reinterpret_cast< nsW32::IMAGE_RESOURCE_DIRECTORY* >( Scan ) );

        WCHAR* szName = nullptr;
        nsW32::PIMAGE_RESOURCE_DATA_ENTRY rde = NULL;

        // Go through all entries of this resource directory
        const int entries = Scan->Header.NumberOfNamedEntries + Scan->Header.NumberOfIdEntries;

        for( int i = 0; i < entries; i++ )
        {
            nsW32::IMAGE_RESOURCE_DIRECTORY_ENTRY rd = Scan->Entries[ i ];

            // If this entry points to data entry get a pointer to it
            if( !rd.DataIsDirectory )
                rde = nsW32::PIMAGE_RESOURCE_DATA_ENTRY( rd.OffsetToData + ( uint8_t* )Root );

            // If this entry has a name, translate it from Unicode
            if( rd.NameIsString )
            {
                const auto rds = static_cast< nsW32::PIMAGE_RESOURCE_DIR_STRING_U >( Add2Ptr( Root, rd.NameOffset ) );

                size_t nameSize = rds->Length;
                szName = new WCHAR[ nameSize + 1 ];
                memset( szName, '\0', ( nameSize + 1 ) * sizeof( char16_t ) );
                memcpy( szName, rds->NameString, nameSize * sizeof( char16_t ) );
                szName[ nameSize ] = 0;
            }
            // Else, set the name to this entry's id
            else
                szName = MAKEINTRESOURCEW( Scan->Entries[ i ].Id );

            if( rd.DataIsDirectory )
            {
                Ret->AddEntry(
                  new nsDetail::CPEResDirectoryEntry(
                      szName,
                      scanDirectory(
                          Root,
                          nsW32::PRESOURCE_DIRECTORY( rd.OffsetToDirectory + ( LPBYTE )Root )
                      )
                  )
                );
            }
            else
            {
                LPBYTE pbData = ( LPBYTE )Root + rde->OffsetToData - _dwResourceSectionVA;
                DWORD dwOffset = DWORD( pbData - &_image[ 0 ] );

                if( dwOffset > DWORD( _image.size() ) )
                {
                    // Invalid resource entry data pointer, possibly compressed resources;

                    delete Ret;
                    Ret = nullptr;
                    break;
                }

                Ret->AddEntry( new nsDetail::CPEResDirectoryEntry( szName, new nsDetail::CPEResDataEntry( pbData,
                                                                                                          rde->Size,
                                                                                                          rde->CodePage,
                                                                                                          dwOffset
                )
                )
                );
            }

            // Delete the dynamicly allocated name if it is a name and not an id
            if( !IS_INTRESOURCE( szName ) )
                delete[] szName;

            szName = nullptr;
        } // for

        return Ret;
    }
} // nsStub

