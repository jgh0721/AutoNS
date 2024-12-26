#ifndef __HDR_CMN_WIN32_PE_IMAGE__
#define __HDR_CMN_WIN32_PE_IMAGE__

#include <stdint.h>

#ifndef FlagOn
#define FlagOn(_F,_SF)        ((_F) & (_SF))
#endif

#ifndef BooleanFlagOn
#define BooleanFlagOn(F,SF)   ((BOOLEAN)(((F) & (SF)) != 0))
#endif

#ifndef SetFlag
#define SetFlag(_F,_SF)       ((_F) |= (_SF))
#endif

#ifndef ClearFlag
#define ClearFlag(_F,_SF)     ((_F) &= ~(_SF))
#endif

#ifndef Add2Ptr
#define Add2Ptr(P,I) ((void*)((unsigned char*)(P) + (I)))
#endif

namespace nsW32
{
    // Size 의 수를 Alignment 의 배수로 반올림한다
    template< typename T >
    T AlignToSize( const T Size, int Alignment = 512 )
    {
        return ( Size + ( Alignment - 1 ) ) & ~( Alignment - 1 );
    }
    
#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) ((((uintptr_t)(_r)) >> 16) == 0)
#endif

#ifndef GetFieldOffset
#define GetFieldOffset(type, field)    ((int)(intptr_t)&(((type *)0)->field))
#endif

#ifndef GetMemberFromNtOptionalHDR
#define GetMemberFromNtOptionalHDR( Optional, Member ) \
    ( ( (Optional).Magic == nsW32::IMAGE_NT_OPT_HDR32_MAGIC) ? \
      &((nsW32::PIMAGE_OPTIONAL_HEADER32)&(Optional))->Member : \
      &((nsW32::PIMAGE_OPTIONAL_HEADER64)&(Optional))->Member \
    )
#endif

#ifndef GetFirstSection
#define GetFirstSection( NtHDR ) ((nsW32::PIMAGE_SECTION_HEADER)               \
    ((uintptr_t)(NtHDR) +                                               \
     GetFieldOffset( nsW32::IMAGE_NT_HEADERS32, OptionalHeader ) +        \
     ((NtHDR))->FileHeader.SizeOfOptionalHeader                         \
    ))
#endif

    constexpr uint16_t IMAGE_DOS_SIG    = 0x5A4D;       // MZ
    constexpr uint16_t IMAGE_OS2_SIG    = 0x454E;       // NE
    constexpr uint16_t IMAGE_OS2_SIG_LE = 0x454C;       // LE
    constexpr uint16_t IMAGE_VXD_SIG    = 0x454C;       // LE
    constexpr uint32_t IMAGE_N_T_SIG    = 0x00004550;   // PE00

#pragma pack(push)
#pragma pack(2)
    typedef struct _IMAGE_DOS_HEADER
    {
        // DOS .EXE header
        uint16_t e_magic;                     // Magic number
        uint16_t e_cblp;                      // Bytes on last page of file
        uint16_t e_cp;                        // Pages in file
        uint16_t e_crlc;                      // Relocations
        uint16_t e_cparhdr;                   // Size of header in paragraphs
        uint16_t e_minalloc;                  // Minimum extra paragraphs needed
        uint16_t e_maxalloc;                  // Maximum extra paragraphs needed
        uint16_t e_ss;                        // Initial (relative) SS value
        uint16_t e_sp;                        // Initial SP value
        uint16_t e_csum;                      // Checksum
        uint16_t e_ip;                        // Initial IP value
        uint16_t e_cs;                        // Initial (relative) CS value
        uint16_t e_lfarlc;                    // File address of relocation table
        uint16_t e_ovno;                      // Overlay number
        uint16_t e_res[ 4 ];                  // Reserved words
        uint16_t e_oemid;                     // OEM identifier (for e_oeminfo)
        uint16_t e_oeminfo;                   // OEM information; e_oemid specific
        uint16_t e_res2[ 10 ];                // Reserved words
        int32_t  e_lfanew;                    // File address of new exe header
    } IMAGE_DOS_HEADER, * PIMAGE_DOS_HEADER;

    typedef struct _IMAGE_OS2_HEADER
    {      // OS/2 .EXE header
        uint16_t   ne_magic;                    // Magic number
        int8_t     ne_ver;                      // Version number
        int8_t     ne_rev;                      // Revision number
        uint16_t   ne_enttab;                   // Offset of Entry Table
        uint16_t   ne_cbenttab;                 // Number of bytes in Entry Table
        int32_t    ne_crc;                      // Checksum of whole file
        uint16_t   ne_flags;                    // Flag word
        uint16_t   ne_autodata;                 // Automatic data segment number
        uint16_t   ne_heap;                     // Initial heap allocation
        uint16_t   ne_stack;                    // Initial stack allocation
        int32_t    ne_csip;                     // Initial CS:IP setting
        int32_t    ne_sssp;                     // Initial SS:SP setting
        uint16_t   ne_cseg;                     // Count of file segments
        uint16_t   ne_cmod;                     // Entries in Module Reference Table
        uint16_t   ne_cbnrestab;                // Size of non-resident name table
        uint16_t   ne_segtab;                   // Offset of Segment Table
        uint16_t   ne_rsrctab;                  // Offset of Resource Table
        uint16_t   ne_restab;                   // Offset of resident name table
        uint16_t   ne_modtab;                   // Offset of Module Reference Table
        uint16_t   ne_imptab;                   // Offset of Imported Names Table
        int32_t    ne_nrestab;                  // Offset of Non-resident Names Table
        uint16_t   ne_cmovent;                  // Count of movable entries
        uint16_t   ne_align;                    // Segment alignment shift count
        uint16_t   ne_cres;                     // Count of resource segments
        uint8_t    ne_exetyp;                   // Target Operating system
        uint8_t    ne_flagsothers;              // Other .EXE flags
        uint16_t   ne_pretthunks;               // offset to return thunks
        uint16_t   ne_psegrefbytes;             // offset to segment ref. bytes
        uint16_t   ne_swaparea;                 // Minimum code swap area size
        uint16_t   ne_expver;                   // Expected Windows version number
    } IMAGE_OS2_HEADER, * PIMAGE_OS2_HEADER;

    typedef struct _PE_IMAGE_VXD_HEADER
    {      // Windows VXD header
        uint16_t   e32_magic;                   // Magic number
        uint8_t    e32_border;                  // The byte ordering for the VXD
        uint8_t    e32_worder;                  // The word ordering for the VXD
        uint32_t   e32_level;                   // The EXE format level for now = 0
        uint16_t   e32_cpu;                     // The CPU type
        uint16_t   e32_os;                      // The OS type
        uint32_t   e32_ver;                     // Module version
        uint32_t   e32_mflags;                  // Module flags
        uint32_t   e32_mpages;                  // Module # pages
        uint32_t   e32_startobj;                // Object # for instruction pointer
        uint32_t   e32_eip;                     // Extended instruction pointer
        uint32_t   e32_stackobj;                // Object # for stack pointer
        uint32_t   e32_esp;                     // Extended stack pointer
        uint32_t   e32_pagesize;                // VXD page size
        uint32_t   e32_lastpagesize;            // Last page size in VXD
        uint32_t   e32_fixupsize;               // Fixup section size
        uint32_t   e32_fixupsum;                // Fixup section checksum
        uint32_t   e32_ldrsize;                 // Loader section size
        uint32_t   e32_ldrsum;                  // Loader section checksum
        uint32_t   e32_objtab;                  // Object table offset
        uint32_t   e32_objcnt;                  // Number of objects in module
        uint32_t   e32_objmap;                  // Object page map offset
        uint32_t   e32_itermap;                 // Object iterated data map offset
        uint32_t   e32_rsrctab;                 // Offset of Resource Table
        uint32_t   e32_rsrccnt;                 // Number of resource entries
        uint32_t   e32_restab;                  // Offset of resident name table
        uint32_t   e32_enttab;                  // Offset of Entry Table
        uint32_t   e32_dirtab;                  // Offset of Module Directive Table
        uint32_t   e32_dircnt;                  // Number of module directives
        uint32_t   e32_fpagetab;                // Offset of Fixup Page Table
        uint32_t   e32_frectab;                 // Offset of Fixup Record Table
        uint32_t   e32_impmod;                  // Offset of Import Module Name Table
        uint32_t   e32_impmodcnt;               // Number of entries in Import Module Name Table
        uint32_t   e32_impproc;                 // Offset of Import Procedure Name Table
        uint32_t   e32_pagesum;                 // Offset of Per-Page Checksum Table
        uint32_t   e32_datapage;                // Offset of Enumerated Data Pages
        uint32_t   e32_preload;                 // Number of preload pages
        uint32_t   e32_nrestab;                 // Offset of Non-resident Names Table
        uint32_t   e32_cbnrestab;               // Size of Non-resident Name Table
        uint32_t   e32_nressum;                 // Non-resident Name Table Checksum
        uint32_t   e32_autodata;                // Object # for automatic data object
        uint32_t   e32_debuginfo;               // Offset of the debugging information
        uint32_t   e32_debuglen;                // The length of the debugging info. in bytes
        uint32_t   e32_instpreload;             // Number of instance pages in preload section of VXD file
        uint32_t   e32_instdemand;              // Number of instance pages in demand load section of VXD file
        uint32_t   e32_heapsize;                // Size of heap - for 16-bit apps
        uint8_t    e32_res3[ 12 ];              // Reserved words
        uint32_t   e32_winresoff;
        uint32_t   e32_winreslen;
        uint16_t   e32_devid;                   // Device ID for VxD
        uint16_t   e32_ddkver;                  // DDK version for VxD
    } IMAGE_VXD_HEADER, *PIMAGE_VXD_HEADER;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(4)
    typedef struct _IMAGE_FILE_HEADER
    {
        uint16_t   Machine;
        uint16_t   NumberOfSections;
        uint32_t   TimeDateStamp;
        uint32_t   PointerToSymbolTable;
        uint32_t   NumberOfSymbols;
        uint16_t   SizeOfOptionalHeader;
        uint16_t   Characteristics;
    } IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;

    typedef struct _IMAGE_DATA_DIRECTORY
    {
        uint32_t  VirtualAddress;
        uint32_t  Size;
    } IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

    constexpr int PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;

    typedef struct _IMAGE_OPTIONAL_HEADER
    {
        // Standard fields.

        uint16_t   Magic;
        uint8_t    MajorLinkerVersion;
        uint8_t    MinorLinkerVersion;
        uint32_t   SizeOfCode;
        uint32_t   SizeOfInitializedData;
        uint32_t   SizeOfUninitializedData;
        uint32_t   AddressOfEntryPoint;
        uint32_t   BaseOfCode;
        uint32_t   BaseOfData;

        // NT additional fields.

        uint32_t   ImageBase;
        uint32_t   SectionAlignment;
        uint32_t   FileAlignment;
        uint16_t   MajorOperatingSystemVersion;
        uint16_t   MinorOperatingSystemVersion;
        uint16_t   MajorImageVersion;
        uint16_t   MinorImageVersion;
        uint16_t   MajorSubsystemVersion;
        uint16_t   MinorSubsystemVersion;
        uint32_t   Win32VersionValue;
        uint32_t   SizeOfImage;
        uint32_t   SizeOfHeaders;
        uint32_t   CheckSum;
        uint16_t   Subsystem;
        uint16_t   DllCharacteristics;
        uint32_t   SizeOfStackReserve;
        uint32_t   SizeOfStackCommit;
        uint32_t   SizeOfHeapReserve;
        uint32_t   SizeOfHeapCommit;
        uint32_t   LoaderFlags;
        uint32_t   NumberOfRvaAndSizes;
        IMAGE_DATA_DIRECTORY DataDirectory[ PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES ];
    } IMAGE_OPTIONAL_HEADER32, * PIMAGE_OPTIONAL_HEADER32;

    typedef struct _IMAGE_OPTIONAL_HEADER64
    {
        uint16_t        Magic;
        uint8_t         MajorLinkerVersion;
        uint8_t         MinorLinkerVersion;
        uint32_t       SizeOfCode;
        uint32_t       SizeOfInitializedData;
        uint32_t       SizeOfUninitializedData;
        uint32_t       AddressOfEntryPoint;
        uint32_t       BaseOfCode;
        uint64_t   ImageBase;
        uint32_t       SectionAlignment;
        uint32_t       FileAlignment;
        uint16_t        MajorOperatingSystemVersion;
        uint16_t        MinorOperatingSystemVersion;
        uint16_t        MajorImageVersion;
        uint16_t        MinorImageVersion;
        uint16_t        MajorSubsystemVersion;
        uint16_t        MinorSubsystemVersion;
        uint32_t       Win32VersionValue;
        uint32_t       SizeOfImage;
        uint32_t       SizeOfHeaders;
        uint32_t       CheckSum;
        uint16_t        Subsystem;
        uint16_t        DllCharacteristics;
        uint64_t   SizeOfStackReserve;
        uint64_t   SizeOfStackCommit;
        uint64_t   SizeOfHeapReserve;
        uint64_t   SizeOfHeapCommit;
        uint32_t       LoaderFlags;
        uint32_t       NumberOfRvaAndSizes;
        IMAGE_DATA_DIRECTORY DataDirectory[ PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES ];
    } IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;

    constexpr uint16_t IMAGE_NT_OPT_HDR32_MAGIC = 0x10b;
    constexpr uint16_t IMAGE_NT_OPT_HDR64_MAGIC = 0x20b;
    constexpr uint16_t IMAGE_ROM_OPT_HDR_MAGIC  = 0x107;

    // Directory Entries

    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_EXPORT         =  0;   // Export Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_IMPORT         =  1;   // Import Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_RESOURCE       =  2;   // Resource Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_EXCEPTION      =  3;   // Exception Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_SECURITY       =  4;   // Security Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_BASERELOC      =  5;   // Base Relocation Table
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_DEBUG          =  6;   // Debug Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_COPYRIGHT      =  7;   // (X86 usage)
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_ARCHITECTURE   =  7;   // Architecture Specific Data
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_GLOBALPTR      =  8;   // RVA of GP
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_TLS            =  9;   // TLS Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_LOAD_CONFIG    = 10;   // Load Configuration Directory
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_BOUND_IMPORT   = 11;   // Bound Import Directory in headers
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_IAT            = 12;   // Import Address Table
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_DELAY_IMPORT   = 13;   // Delay Load Import Descriptors
    constexpr int32_t IMAGE_DIRECTORY_SEC_ENTRY_COM_DESCRIPTOR = 14;   // COM Runtime descriptor

    typedef struct _IMAGE_NT_HEADERS
    {
        uint32_t                Signature;
        IMAGE_FILE_HEADER       FileHeader;
        IMAGE_OPTIONAL_HEADER32 OptionalHeader;
    } IMAGE_NT_HEADERS32, * PIMAGE_NT_HEADERS32;

    typedef struct _IMAGE_NT_HEADERS64
    {
        uint32_t                Signature;
        IMAGE_FILE_HEADER       FileHeader;
        IMAGE_OPTIONAL_HEADER64 OptionalHeader;
    } IMAGE_NT_HEADERS64, * PIMAGE_NT_HEADERS64;

    constexpr int PE_IMAGE_SIZEOF_SHORT_NAME = 8;

    typedef struct _IMAGE_SECTION_HEADER
    {
        uint8_t    Name[ PE_IMAGE_SIZEOF_SHORT_NAME ];
        union
        {
            uint32_t   PhysicalAddress;
            uint32_t   VirtualSize;
        } Misc;
        uint32_t   VirtualAddress;
        uint32_t   SizeOfRawData;
        uint32_t   PointerToRawData;
        uint32_t   PointerToRelocations;
        uint32_t   PointerToLinenumbers;
        uint16_t    NumberOfRelocations;
        uint16_t    NumberOfLinenumbers;
        uint32_t   Characteristics;
    } IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;

    constexpr int PE_IMAGE_SIZEOF_SECTION_HEADER = sizeof( IMAGE_SECTION_HEADER );
    static_assert( PE_IMAGE_SIZEOF_SECTION_HEADER == 40 );

    typedef struct _IMAGE_RESOURCE_DIRECTORY
    {
        uint32_t   Characteristics;
        uint32_t   TimeDateStamp;
        uint16_t   MajorVersion;
        uint16_t   MinorVersion;
        uint16_t   NumberOfNamedEntries;
        uint16_t   NumberOfIdEntries;
        //  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
    } IMAGE_RESOURCE_DIRECTORY, * PIMAGE_RESOURCE_DIRECTORY;

    typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY
    {
        union
        {
            struct
            {
                uint32_t NameOffset : 31;
                uint32_t NameIsString : 1;
            };
            uint32_t   Name;
            uint16_t   Id;
        };
        union
        {
            uint32_t   OffsetToData;
            struct
            {
                uint32_t   OffsetToDirectory : 31;
                uint32_t   DataIsDirectory : 1;
            };
        };
    } IMAGE_RESOURCE_DIRECTORY_ENTRY, * PIMAGE_RESOURCE_DIRECTORY_ENTRY;

    typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING
    {
        uint16_t Length;
        char8_t  NameString[ 1 ];
    } IMAGE_RESOURCE_DIRECTORY_STRING, * PIMAGE_RESOURCE_DIRECTORY_STRING;

    
    typedef struct _IMAGE_RESOURCE_DIR_STRING_U
    {
        uint16_t  Length;
        char16_t  NameString[ 1 ];
    } IMAGE_RESOURCE_DIR_STRING_U, * PIMAGE_RESOURCE_DIR_STRING_U;

    typedef struct _IMAGE_RESOURCE_DATA_ENTRY
    {
        uint32_t   OffsetToData;
        uint32_t   Size;
        uint32_t   CodePage;
        uint32_t   Reserved;
    } IMAGE_RESOURCE_DATA_ENTRY, * PIMAGE_RESOURCE_DATA_ENTRY;

    // Resource directory with entries
    typedef struct RESOURCE_DIRECTORY
    {
        IMAGE_RESOURCE_DIRECTORY Header;
        IMAGE_RESOURCE_DIRECTORY_ENTRY Entries[ 1 ];
    } RESOURCE_DIRECTORY, * PRESOURCE_DIRECTORY;
#pragma pack(pop)

    ///////////////////////////////////////////////////////////////////////////
    ///

    IMAGE_NT_HEADERS32* GetNTHeaders( uint8_t* ImageBase );
    RESOURCE_DIRECTORY* GetResourceDirectory( uint8_t* ImageBase, uint32_t Size, IMAGE_NT_HEADERS32* NtHeaders, uint32_t* pdwResSecVA = nullptr, uint32_t* pdwSectionIndex = nullptr );

} // nsW32

#endif // __HDR_CMN_WIN32_PE_IMAGE__