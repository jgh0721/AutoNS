#ifndef __HDR_TYPE_STRING__
#define __HDR_TYPE_STRING__

#include <windows.h>
#include <limits.h>

class WString
{
public:
    WString() = default;
    WString( int TextLength );                          // Constructor with pre-allocation
    WString( const wchar_t* Str );
    WString( const WString& Str );
    ~WString();

    unsigned int                Length() const;
    // 버퍼를 WString 에서 관리하도록 설정합니다. 소멸자에서 free 를 이용해 해제합니다. 
    void                        SetBuffer( const wchar_t* Text, int TextLength = -1 );

    WString	                    Left( int nSize ) const;						// left-most characters
    WString	                    Right( int nSize ) const;						// right-most characters
    WString	                    Mid( int nStart, int nSize = INT_MAX ) const;	// middle characters

    const wchar_t*              Ptr() const;

	WString&                    operator=( const WString& Rhs );		
    WString		                operator+( const WString& Rhs ) const;	
    WString&                    operator+=( const WString& Rhs );

private:
    void                        clear();

    static const int NULL_CHAR_SIZE = sizeof( wchar_t );

    // from UNICODE_STRING
    unsigned int                Length_ = 0;            // Bytes
    unsigned int                MaximumLength = 0;      // Allocated
    wchar_t*                    Buffer = nullptr;       // 
};

//
//class AString
//{
//public:
//	// Functions
//	~AString() { delete m_szText; }				// Destructor (inline)
//
//	void		erase( void );					// Erase entire string
//	void		erase( int nStart, int nEnd );	// Erase a range from the string
//	void		erase( int nStart );				// Erase a from start to end
//
//	const char* c_str( void ) const { return m_szText; }	// Return C style string
//	void		tolower( void ) { CharLower( m_szText ); }	// Convert string to lowercase
//	void		toupper( void ) { CharUpper( m_szText ); }	// Convert string to uppercase
//	void		assign( const AString& sStr,
//						int nStart, int nEnd );				// Assign chars from one str to another
//
//	int			find_first_not_of( const char* szInput ) const;							// Find first char not in input
//	int			find_last_not_of( const char* szInput ) const;							// Find last char not in input
//	int			find_first_of( const char* szInput ) const;								// Find first char in input
//	int			find_last_of( const char* szInput ) const;								// Find last char in input
//	int			find_str( const char* szInput, bool bCaseSense, int nOccurance = 1 ) const;	// Find given string in input
//
//	void		strip_leading( const char* szInput );		// Strip leading chars
//	void		strip_trailing( const char* szInput );	// Strip trailing chars
//
//	// Properties
//	int			length( void ) const { return m_length; }	// Return string length
//	bool		empty( void ) const;						// Tests if string empty
//
//	// Substrings
//	AString		left( int nSize ) const;						// left-most characters
//	AString		right( int nSize ) const;						// right-most characters
//	AString		mid( int nStart, int nSize = INT_MAX ) const;	// middle characters
//
//	// Overloads
//	char& operator[]( int nIndex );					// Overloaded []
//	char		operator[]( int nIndex ) const;			// Overloaded [] for const
//	AString& operator=( const char* szOp2 );			// Overloaded = for C string
//	AString& operator=( const char ch );				// Overloaded = for a single char
//	AString& operator+=( const char* szOp2 );			// Overloaded += for C string (append)
//	AString& operator+=( const char ch );				// Overloaded += for a single char (append)
//	AString		operator+( const char ch ) const;			// Overloaded + for a single char (append)
//
//	friend bool	operator==( const AString& sOp1, const AString& sOp2 );	// Overloaded == for AStrings (case sensitive)
//	friend bool	operator!=( const AString& sOp1, const AString& sOp2 );	// Overloaded != for AStrings (case sensitive)
//	friend bool	operator <( const AString& sOp1, const AString& sOp2 );	// Overloaded < for AStrings (case sensitive)
//	friend bool	operator<=( const AString& sOp1, const AString& sOp2 );	// Overloaded <= for AStrings (case sensitive)
//	friend bool	operator >( const AString& sOp1, const AString& sOp2 );	// Overloaded > for AStrings (case sensitive)
//	friend bool	operator>=( const AString& sOp1, const AString& sOp2 );	// Overloaded >= for AStrings (case sensitive)
//};


#endif // __HDR_TYPE_STRING__