﻿#include "stdafx.h"
#include "cmnDataType_String.hpp"

#pragma warning( disable: 4996 )

WString::WString( int TextLength )
{
    Length_ = TextLength * sizeof(wchar_t);
    MaximumLength = ( ( Length_ + NULL_CHAR_SIZE ) + 7 ) & 0xfffffff8;	// Multiple of 8
    Buffer = ( wchar_t* )malloc( MaximumLength );
    memset( Buffer, '\0', MaximumLength );
} 

WString::WString( const wchar_t* Str )
{
    size_t Len = wcslen( Str );
    if( Len == 0 )
        return;

    Length_       = static_cast< unsigned int >( Len * sizeof( wchar_t ) );
    MaximumLength = Length_ + NULL_CHAR_SIZE;
    Buffer        = static_cast< wchar_t* >( malloc( MaximumLength ) );
    memcpy( Buffer, Str, Length_ );
}

WString::WString( const WString& Str )
{
    Length_         = Str.Length_;
    MaximumLength   = Str.MaximumLength;
    if( MaximumLength == 0 )
        return;

    Buffer = static_cast< wchar_t* >( malloc( MaximumLength ) );
    wcscpy_s( Buffer, Length_ / 2 + 1, Str.Buffer );
}

WString::~WString()
{
    clear();
}

unsigned int WString::Length() const
{
    if( Length_ == 0 ) return 0;
    return Length_ / 2;
}

void WString::SetBuffer( const wchar_t* Text, int TextLength )
{
    clear();

    Length_       = static_cast< unsigned int >( ( TextLength < 0 ? wcslen( Text ) : TextLength ) * sizeof( wchar_t ) );
    MaximumLength = Length_ + NULL_CHAR_SIZE;
    Buffer        = const_cast<wchar_t*>( Text );
}

const wchar_t* WString::Ptr() const
{
    return Buffer;
}

WString& WString::operator=( const WString& Rhs )
{
    if( this == &Rhs )
        return *this;

    Length_ = Rhs.Length_;

    if( MaximumLength < ( Length_ + NULL_CHAR_SIZE ) )
    {
        free( Buffer );
        MaximumLength = ( ( Length_ + NULL_CHAR_SIZE + 1 ) + 7 ) & 0xfffffff8;	// Multiple of 8
        Buffer = (wchar_t*)malloc( MaximumLength );
        memset( Buffer, '\0', MaximumLength );
    }

    wcscpy( Buffer, Rhs.Buffer );

    return *this;								// Return this object that generated the call

} // operator=()

WString WString::operator+( const WString& Rhs ) const
{
    WString retval( Length() + Rhs.Length() );

    wcscpy( retval.Buffer, Buffer );
    wcscat( retval.Buffer, Rhs.Buffer );
    return retval;

} // operator+()

WString& WString::operator+=( const WString& Rhs )
{
    if( this == &Rhs )
    {
        // duplicate current contents and store.
        *this = *this + *this;
        return *this;
    }

    Length_ += Rhs.Length();

    if( MaximumLength < ( Length_ + NULL_CHAR_SIZE ) )
    {
        MaximumLength = ( ( ( Length_ << 1 ) + 1 ) + 7 ) & 0xfffffff8;	// Leave room to grow
        wchar_t* wsz = ( wchar_t* )malloc( MaximumLength );
        memset( wsz, '\0', MaximumLength );
        wcscpy( wsz, Buffer );
        free( Buffer );
        Buffer = wsz;
    }

    wcscat( Buffer, Rhs.Buffer );

    return *this;								// Return this object that generated the call

} // operator+=()


void WString::clear()
{
    Length_ = 0;
    MaximumLength = 0;
    free( Buffer );
    Buffer = nullptr;
}

//
/////////////////////////////////////////////////////////////////////////////////
//// Constructor()
//// Default constructor
/////////////////////////////////////////////////////////////////////////////////
//
//AString::AString() : m_length( 0 ), m_allocated( 8 )
//{
//    m_szText = new char[ 8 ];
//    m_szText[ 0 ] = '\0';
//
//} // AString()
//
/////////////////////////////////////////////////////////////////////////////////
//// Constructor()
//// With pre-allocation of the string size to speed up multiple += ops
/////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Copy Constructor()
/////////////////////////////////////////////////////////////////////////////////
//
//AString::AString( const AString& sSource )
//{
//    m_length = sSource.m_length;
//    m_allocated = ( ( m_length + 1 ) + 7 ) & 0xfffffff8;	// Multiple of 8
//    m_szText = new char[ m_allocated ];
//    strcpy( m_szText, sSource.m_szText );
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator[]()
/////////////////////////////////////////////////////////////////////////////////
//
//char& AString::operator[]( int nIndex )
//{
//    if( nIndex >= 0 && nIndex < m_length )
//        return m_szText[ nIndex ];
//    else
//        return m_szText[ m_length ];				// return \0
//
//} // operator[]()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator[]() for const
/////////////////////////////////////////////////////////////////////////////////
//
//char AString::operator[]( int nIndex ) const
//{
//    if( nIndex >= 0 && nIndex < m_length )
//        return m_szText[ nIndex ];
//    else
//        return m_szText[ m_length ];				// return \0
//
//} // operator[]()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator=() for AStrings
/////////////////////////////////////////////////////////////////////////////////
//

//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator=() for C strings
/////////////////////////////////////////////////////////////////////////////////
//
//AString& AString::operator=( const char* szOp2 )
//{
//    m_length = ( int )strlen( szOp2 );
//
//    if( m_allocated < ( m_length + 1 ) )
//    {
//        delete[] m_szText;
//        m_allocated = ( ( m_length + 1 ) + 7 ) & 0xfffffff8;	// Multiple of 8
//        m_szText = new char[ m_allocated ];
//    }
//
//    strcpy( m_szText, szOp2 );
//
//    return *this;								// Return this object that generated the call
//
//} // operator=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator=() for a single char
/////////////////////////////////////////////////////////////////////////////////
//
//AString& AString::operator=( const char ch )
//{
//    if( m_allocated < 8 )
//    {
//        delete[] m_szText;
//        m_allocated = 8;
//        m_szText = new char[ 8 ];
//    }
//
//    m_length = 1;
//    m_szText[ 0 ] = ch;
//    m_szText[ 1 ] = '\0';
//
//    return *this;								// Return this object that generated the call
//
//} // operator=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator+=() for AStrings
/////////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator+=() for C strings
/////////////////////////////////////////////////////////////////////////////////
//
//AString& AString::operator+=( const char* szOp2 )
//{
//    m_length += ( int )strlen( szOp2 );
//
//    if( m_allocated < ( m_length + 1 ) )
//    {
//        m_allocated = ( ( ( m_length << 1 ) + 1 ) + 7 ) & 0xfffffff8;	// Leave room to grow
//        char* szTemp = new char[ m_allocated ];
//        strcpy( szTemp, m_szText );
//        delete[] m_szText;
//        m_szText = szTemp;
//    }
//
//    strcat( m_szText, szOp2 );
//
//    return *this;								// Return this object that generated the call
//
//} // operator+=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator+=() for a single char
/////////////////////////////////////////////////////////////////////////////////
//
//AString& AString::operator+=( const char ch )
//{
//    m_length += 1;
//
//    if( m_allocated < ( m_length + 1 ) )
//    {
//        m_allocated = ( ( ( m_length << 1 ) + 1 ) + 7 ) & 0xfffffff8;	// Leave room to grow
//        char* szTemp = new char[ m_allocated ];
//        strcpy( szTemp, m_szText );
//        delete[] m_szText;
//        m_szText = szTemp;
//    }
//
//    m_szText[ m_length - 1 ] = ch;
//    m_szText[ m_length ] = '\0';
//
//    return *this;								// Return this object that generated the call
//
//} // operator+=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator+() for AStrings
/////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator+() for a single char
/////////////////////////////////////////////////////////////////////////////////
//
//AString AString::operator+( const char ch ) const
//{
//    AString retval( m_length + 2 );
//
//    strcpy( retval.m_szText, m_szText );
//
//    retval.m_szText[ m_length ] = ch;
//    retval.m_szText[ m_length + 1 ] = '\0';
//    retval.m_length = m_length + 1;
//    return retval;
//} // operator+()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator==() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator==( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) )
//        return false;
//    else
//        return true;
//
//} // operator==()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator!=() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator!=( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) )
//        return true;
//    else
//        return false;
//
//} // operator!=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator <() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator <( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) < 0 )
//        return true;
//    else
//        return false;
//
//} // operator <()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator<=() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator<=( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) <= 0 )
//        return true;
//    else
//        return false;
//
//} // operator<=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator >() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator >( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) > 0 )
//        return true;
//    else
//        return false;
//
//} // operator >()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// Overloaded operator>=() for AStrings
////
//// CASE SENSITIVE
/////////////////////////////////////////////////////////////////////////////////
//
//bool operator>=( const AString& sOp1, const AString& sOp2 )
//{
//    if( strcmp( sOp1.m_szText, sOp2.m_szText ) >= 0 )
//        return true;
//    else
//        return false;
//
//} // operator>=()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// empty()
//// Returns true if a string is empty (first char is \0)
/////////////////////////////////////////////////////////////////////////////////
//
//bool AString::empty( void ) const
//{
//    if( m_szText[ 0 ] == '\0' )
//        return true;
//    else
//        return false;
//
//} // empty()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// erase()
//// Erase an entire string (and release memory)
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::erase( void )
//{
//    delete[] m_szText;
//
//    m_length = 0;
//    m_allocated = 8;
//    m_szText = new char[ m_allocated ];
//    m_szText[ 0 ] = '\0';
//
//} // erase()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// erase()
//// Erase a range from a string (erase from nStart to (but not including) nEnd)
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::erase( int nStart, int nEnd )
//{
//    if( nStart < 0 || nStart >= m_length || nEnd < 0 || nEnd > m_length || nEnd <= nStart )
//        return;
//
//    while( m_szText[ nEnd ] != '\0' )
//        m_szText[ nStart++ ] = m_szText[ nEnd++ ];
//
//    m_szText[ nStart ] = '\0';							// Terminate
//
//    m_length = ( int )strlen( m_szText );
//
//} // erase()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// erase()
//// Erase a range from a string (erase from nStart to end of string)
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::erase( int nStart )
//{
//    if( nStart < 0 || nStart >= m_length )
//        return;
//
//    m_szText[ nStart ] = '\0';
//    m_length = ( int )strlen( m_szText );
//
//} // erase()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// assign()
//// Assign a string with a range from another string
//// (copy from nStart to (but not including) nEnd)
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::assign( const AString& sStr, int nStart, int nEnd )
//{
//    int Len = nEnd - nStart;					// Number of elements to copy
//
//    // Valid ranges?
//    if( nStart < 0 || nStart >= sStr.m_length || nEnd < 0 || nEnd > sStr.m_length || nEnd <= nStart )
//    {
//        // Bad range, reset this string to empty and return
//        erase();
//        return;
//    }
//
//    // Does the existing string have enough space?
//    if( m_allocated < ( Len + 1 ) )
//    {
//        m_allocated = ( ( Len + 1 ) + 7 ) & 0xfffffff8;		// Multiple of 8
//        delete[] m_szText;
//        m_szText = new char[ m_allocated ];
//    }
//
//    m_length = Len;
//
//    int i = 0;
//    int j = nStart;
//    while( j < nEnd )
//        m_szText[ i++ ] = sStr.m_szText[ j++ ];
//
//    m_szText[ i ] = '\0';							// Terminate
//
//} // assign()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// find_first_not_of()
//// Returns first element doesn't match the input (or returns AString.end() )
/////////////////////////////////////////////////////////////////////////////////
//
//int AString::find_first_not_of( const char* szInput ) const
//{
//    int	j, nMatches;
//    size_t			nCount = m_length;
//    int	i = 0;
//    while( nCount > 0 )
//    {
//        --nCount;
//
//        j = 0;
//        nMatches = 0;
//        while( szInput[ j ] != '\0' )
//        {
//            if( m_szText[ i ] == szInput[ j ] )
//                ++nMatches;
//
//            ++j;
//        }
//
//        // Did all the characters match?
//        if( nMatches == 0 )
//            return i;							// This is the first pos not to match
//
//        ++i;
//    }
//
//    return i;									// i = AString.end()
//
//} // find_first_not_of()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// find_last_not_of()
//// Returns first element doesn't match the input (or returns AString.size() i.e.
//// index of \0)
/////////////////////////////////////////////////////////////////////////////////
//
//int AString::find_last_not_of( const char* szInput ) const
//{
//    int		j, nMatches;
//    int		nCount = m_length;
//    int		i = m_length - 1;
//
//    while( nCount > 0 )
//    {
//        --nCount;
//
//        j = 0;
//        nMatches = 0;
//        while( szInput[ j ] != '\0' )
//        {
//            if( m_szText[ i ] == szInput[ j ] )
//                ++nMatches;
//
//            ++j;
//        }
//
//        // Did all the characters match?
//        if( nMatches == 0 )
//            return i;							// This is the first pos not to match
//
//        --i;
//    }
//
//    return m_length;							// i = index of \0
//
//} // find_last_not_of()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// find_first_of()
//// Returns first element to match any character in the input (or returns AString.end() )
/////////////////////////////////////////////////////////////////////////////////
//
//int AString::find_first_of( const char* szInput ) const
//{
//    int		j;
//    int		nCount = m_length;
//    int		i = 0;
//
//    while( nCount > 0 )
//    {
//        --nCount;
//
//        j = 0;
//        while( szInput[ j ] != '\0' )
//        {
//            if( m_szText[ i ] == szInput[ j ] )
//                return i;
//
//            ++j;
//        }
//
//        ++i;
//    }
//
//    return i;									// i = AString.end()
//
//} // find_first_of()
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//// find_last_of()
//// Returns last element to match any character in the input (or returns index of
//// \0)
/////////////////////////////////////////////////////////////////////////////////
//
//int AString::find_last_of( const char* szInput ) const
//{
//    int		j;
//    int		nCount = m_length;
//    int		i = m_length - 1;
//
//    while( nCount > 0 )
//    {
//        --nCount;
//
//        j = 0;
//        while( szInput[ j ] != '\0' )
//        {
//            if( m_szText[ i ] == szInput[ j ] )
//                return i;
//
//            ++j;
//        }
//
//        --i;
//    }
//
//    return m_length;							// i = index of \0
//
//} // find_last_of()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// find_str()
//// Returns first element to match the input string (or returns index of \0 = size() )
/////////////////////////////////////////////////////////////////////////////////
//
//int AString::find_str( const char* szInput, bool bCaseSense, int nOccurance ) const
//{
//    int		i, nMatches;
//    int		nInputLen = ( int )strlen( szInput );
//
//    if( nOccurance > 0 )
//    {
//        i = 0;
//        nMatches = 0;
//        while( nMatches < nOccurance )
//        {
//            for( ; i < m_length; ++i )
//            {
//                if( bCaseSense )
//                {
//                    if( strncmp( m_szText + i, szInput, nInputLen ) == 0 ) // if the characters starting at m_szText[i] matches szInput
//                    {
//                        ++nMatches;
//                        break;
//                    }
//                }
//                else
//                {
//                    if( strnicmp( m_szText + i, szInput, nInputLen ) == 0 ) // if the characters starting at m_szText[i] matches szInput
//                    {
//                        ++nMatches;
//                        break;
//                    }
//                }
//            }
//            if( i >= m_length ) // not found
//                break;
//            else if( nMatches < nOccurance )
//                ++i;	// skipped increment
//        }
//    }
//    else if( nOccurance < 0 )
//    {
//        i = m_length - nInputLen;
//        nMatches = 0;
//        while( nMatches < -nOccurance )
//        {
//            for( ; i >= 0; --i )
//            {
//                if( bCaseSense )
//                {
//                    if( strncmp( m_szText + i, szInput, nInputLen ) == 0 ) // if the characters starting at m_szText[i] matches szInput
//                    {
//                        ++nMatches;
//                        break;
//                    }
//                }
//                else
//                {
//                    if( strnicmp( m_szText + i, szInput, nInputLen ) == 0 ) // if the characters starting at m_szText[i] matches szInput
//                    {
//                        ++nMatches;
//                        break;
//                    }
//                }
//            }
//            if( i < 0 ) // not found
//                break;
//            else if( nMatches < -nOccurance )
//                --i;	// skipped decrement
//        }
//    }
//    else
//        return m_length;
//
//    return i;									// i = index of \0
//
//} // find_str()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// strip_leading()
//// Strips leading characters that match the input
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::strip_leading( const char* szInput )
//{
//    erase( 0, find_first_not_of( szInput ) );
//
//} // strip_leading()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// strip_trailing()
//// Strips trailing characters that match the input
/////////////////////////////////////////////////////////////////////////////////
//
//void AString::strip_trailing( const char* szInput )
//{
//    int nPos = find_last_not_of( szInput );
//    if( nPos == m_length )						// =end when ALL matched
//        erase();
//    else
//        erase( nPos + 1 );
//
//} // strip_leading()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// left()
//// returns the left-most number of characters in a new AString
/////////////////////////////////////////////////////////////////////////////////
//
//AString AString::left( int size ) const
//{
//    if( size < 1 )
//        // return size is less than 1
//        return AString( "" );
//
//    AString retval( size );
//    int count;
//    char* pcDest = retval.m_szText, * pcSource = m_szText;
//
//    // copy size characters from source to dest
//    for( count = 0; count < size && *pcSource != '\0'; ++count )
//        // copy character from Source to Dest and then increment the pointers.
//        *( pcDest++ ) = *( pcSource++ );
//
//    // Add null character at end of string
//    *pcDest = '\0';
//    if( count < size )		// source string was smaller than size
//        retval.m_length = count + 1;	// count is the number of characters copied
//    return retval;
//
//} // left()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// mid()
//// returns middle characters in a new AString
/////////////////////////////////////////////////////////////////////////////////
//
//AString AString::mid( int start, int size /* = INT_MAX */ ) const
//{
//    if( size<1 || start > m_length )
//        // return size is less than 1 OR starts after the last character, return empty string
//        return AString( "" );
//
//    // only allocate to the end of the string
//    if( size > m_length - start )
//        size = m_length - start;
//
//    int count;
//    AString retval( size );
//    char* pcDest = retval.m_szText, * pcSource = m_szText + start;
//
//    // copy size characters from source to dest
//    for( count = 0; count < size && *pcSource != '\0'; ++count )
//        // copy character from Source to Dest and then increment the pointers.
//        *( pcDest++ ) = *( pcSource++ );
//
//    // Add null character at end of string
//    *pcDest = '\0';
//    if( count < size )		// source string was smaller than size
//        retval.m_length = count + 1;	// count is the number of characters copied
//    return retval;
//
//} // mid()
//
//
/////////////////////////////////////////////////////////////////////////////////
//// right()
//// returns the right-most number of characters in a new AString
/////////////////////////////////////////////////////////////////////////////////
//
//AString AString::right( int size ) const
//{
//    if( size < 1 )
//        // return size is less than 1
//        return AString( "" );
//
//    AString retval( size );
//    int count = m_length - size;
//    // initial use of count is to figure out where to start copying
//    if( count < 0 )
//        // would start copying before the start of the string, so set to beginning of string
//        count = 0;
//    char* pcDest = retval.m_szText, * pcSource = m_szText + count;
//
//    // copy size characters from source to dest
//    for( count = 0; count < size && *pcSource != '\0'; ++count )
//        // copy character from Source to Dest and then increment the pointers.
//        *( pcDest++ ) = *( pcSource++ );
//
//    // Add null character at end of string
//    *pcDest = '\0';
//    if( count < size )		// source string was smaller than size
//        retval.m_length = count + 1;	// count is the number of characters copied
//    return retval;
//
//} // right()
