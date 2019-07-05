/*****************************************************************************

                              TextContainer.cpp

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/TextContainer/TextContainer.cpp#3 $

	$NoKeywords: $

*****************************************************************************/

#include "TextContainer.h"

#include "../Misc/Assert.h"
#include "../Misc/Etc.h"
#include <cstdio>


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TextContainer::TextContainer()
{
	// Reset the group info

	memset( m_GroupInfo, 0, sizeof m_GroupInfo );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TextContainer::~TextContainer()
{

	// Delete all the groups that we "own"

	for ( int i = 0; i < (int)elementsof( m_GroupInfo ); i++ )
	{
		if ( m_GroupInfo[ i ].m_GroupId != 0 && m_GroupInfo[ i ].m_IsOwned )
		{
			delete[] reinterpret_cast< char * >( m_GroupInfo[ i ].m_GroupId );
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TextContainer::GroupId TextContainer::Load( char * pData, int size )
{
//	assert_pointer_valid( pData, false );

	char * const	pStart	= pData;
	GroupId const	groupId	= GroupId( pData );

	// Scan the file, loading key-string pairs into the map

	char *	p	= pStart;
	while ( p < pStart + size )
	{
		char *	pTextKey;
		char *	pString;

		ScanKeyStringPair( &p, pStart + size - p, &pTextKey, &pString );

		// Insert an entry into the map

		Key	const			key	= MakeKey( pTextKey );
		TextMap::value_type	mapItem( key, MapEntry( groupId, pString ) );
		bool const			wasInserted	= m_Data.insert( mapItem ).second;

		if ( !wasInserted )
		{
			printf( "TextContainer::Load: duplicate entry\n"
					"\tKey: '%s'\n"
					"\tValue: group - %08x, string - '%s'\n"
					"\tNew value: group - %08x, string - '%s'\n",
					pTextKey,
					m_Data[ key ].m_Group, m_Data[ key ].m_pText,
					groupId, pString );
		}

	}

	return groupId;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TextContainer::GroupId TextContainer::Load( std::auto_ptr< char > pData, int size )
{
//	assert_pointer_valid( pData.get(), false );

	char * const	pStart	= pData.get();
	GroupId const	groupId	= GroupId( pData.release() );

	// Scan the file, loading key-string pairs into the map

	char *	p	= pStart;
	while ( p < pStart + size )
	{
		char *	pTextKey;
		char *	pString;

		ScanKeyStringPair( &p, pStart + size - p, &pTextKey, &pString );

		// Insert an entry into the map

		Key	const			key	= MakeKey( pTextKey );
		TextMap::value_type	mapItem( key, MapEntry( groupId, pString ) );
		bool const			wasInserted	= m_Data.insert( mapItem ).second;

		if ( !wasInserted )
		{
			printf( "TextContainer::Load: duplicate entry\n"
					"\tKey: '%s'\n"
					"\tValue: group - %08x, string - '%s'\n"
					"\tNew value: group - %08x, string - '%s'\n",
					pTextKey,
					m_Data[ key ].m_Group, m_Data[ key ].m_pText,
					groupId, pString );
		}

	}

	AddGroupInfo( groupId, true );

	return groupId;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TextContainer::Unload( GroupId id )
{
//	assert_pointer_valid( id, false );

	TextMap::iterator pEntry	= m_Data.begin();
	
	while ( pEntry != m_Data.end() )
	{
		if ( pEntry->second.m_Group == id )
		{
			pEntry = m_Data.erase( pEntry );
		}
		else
		{
			++pEntry;
		}
	}

	// If we own the group, then delete it and remove its info

	if ( GroupIsOwned( id ) )
	{
		delete[] reinterpret_cast< char * >( id );
		RemoveGroupInfo( id );
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

char const * TextContainer::Get( Key key ) const
{
	TextMap::const_iterator const	p	= m_Data.find( key );

	return ( p != m_Data.end() ) ? p->second.m_pText : 0;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TextContainer::Key TextContainer::MakeKey( char const * pTextKey )
{
	unsigned int	key	= 69069;

	while ( *pTextKey != 0 )
	{
		key *= unsigned int( *pTextKey );
		key += 1;

		++pTextKey;
	}

	return TextContainer::Key( key );
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TextContainer::ScanKeyStringPair( char * * ppData, int size, char * * ppKey, char * * ppString )
{
//	assert_pointer_valid( ppData, false );
//	assert_pointer_valid( *ppData, false );

	char * pData		= *ppData;

	// Scan the key

	*ppKey = pData;
	ScanString( &pData, *ppData + size - pData, '\t' );

	// Scan the string

	*ppString = pData;
	ScanString( &pData, *ppData + size - pData, '\r' );
	++pData;							// The string text is terminated by \r\n,
										// so skip the \n too.


	*ppData = pData;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TextContainer::ScanString( char * * ppData, int size, int delimiter )
{
//	assert_pointer_valid( ppData, false );
//	assert_pointer_valid( *ppData, false );

	char * const	pData		= *ppData;
	char *			pSource		= pData;
	char *			pDest		= pData;
	bool			isQuoted	= false;

	// If the first character of the string is a doublequote, then excel has
	// quoted the string. The first and last characters of the string are doublequote
	// and should be skipped.

	if ( *pSource == '"' )
	{
		++pSource;
		isQuoted = true;
	}

	// Scan the string removing escaped doublequotes

	while ( *pSource != delimiter )
	{
		// If the delimiter wasn't found, then abort

		if ( pSource >= pData + size )
		{
			*ppData = pSource;
			return;
		}

		// Excel replaces doublequotes with two doublequotes, skip the first one.
		
		if ( *pSource == '"' &&
			 pSource + 1 < pData + size &&
			 *(pSource+1) == '"' )
		{
			++pSource;
		}

		*pDest++ = *pSource++;
	}

	// If the string was quoted then remove the trailing quote

	if ( isQuoted && pDest > pData )
	{
		assert( *(pDest-1) == '"' );
		*(pDest-1) = 0;					// Overwrite it with a 0 to remove it
	}

	*pDest = 0;				// Terminate the text by replacing the delimiter with a 0

	*ppData = pSource + 1;	// Return the next character after the delimiter
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TextContainer::AddGroupInfo( GroupId id, bool isOwned )
{
	int i = 0;

	while ( i < (int)elementsof( m_GroupInfo ) && m_GroupInfo[ i ].m_GroupId != 0 )
	{
		++i;
	}

	assert( i < (int)elementsof( m_GroupInfo ) );

	m_GroupInfo[ i ].m_GroupId = id;
	m_GroupInfo[ i ].m_IsOwned = isOwned;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool TextContainer::GroupIsOwned( GroupId id ) const
{
	int i = 0;

	while ( i < (int)elementsof( m_GroupInfo ) && m_GroupInfo[ i ].m_GroupId != id )
	{
		++i;
	}

	assert( i < (int)elementsof( m_GroupInfo ) );

	return m_GroupInfo[ i ].m_IsOwned;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TextContainer::RemoveGroupInfo( GroupId id )
{
	int i = 0;

	while ( i < (int)elementsof( m_GroupInfo ) && m_GroupInfo[ i ].m_GroupId != id )
	{
		++i;
	}

	assert( i < (int)elementsof( m_GroupInfo ) );

	m_GroupInfo[ i ].m_GroupId = 0;
}

