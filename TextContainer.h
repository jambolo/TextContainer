#if !defined( TEXTCONTAINER_H_INCLUDED )
#define TEXTCONTAINER_H_INCLUDED

#pragma once

/*****************************************************************************

                               TextContainer.h

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/TextContainer/TextContainer.h#2 $

	$NoKeywords: $

*****************************************************************************/


#include <map>
#include <memory>

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class TextContainer  
{
public:

	// Key type for accessing text
	typedef unsigned int	Key;

	// Text group id type
	typedef unsigned int	GroupId;

	TextContainer();
	virtual ~TextContainer();

	// Load a group of text in a raw tab-delimited format. An id is returned.
	// The function failed if the returned value is 0. This function is
	// fairly slow. The data pointed to by pData is modified.
	GroupId Load( char * pData, int size );

	// Load a group of text in a raw tab-delimited format. An id is returned.
	// The function failed if the returned value is 0. This function is
	// fairly slow. The data pointed to by pData is "owned" by this class.
	// NOTE: The address of the data is passed as an auto_ptr but the data
	// must have been allocated with new[].
	GroupId Load( std::auto_ptr< char > pData, int size );

	// Unload a previously loaded group of text with the given id. This
	// function is fairly slow.
	void Unload( GroupId id );

	// Return a pointer to a string given a key. The key can be in the form of
	// a string or a Key (a value derived from the string form). 0 is
	// returned if the key is not found.
	char const * Get( Key key ) const;
	char const * Get( char const * pTextKey ) const;
	char const * operator[]( Key key ) const;
	char const * operator[]( char const * pTextKey ) const;

	// Create a Key from a string
	static Key MakeKey( char const * pTextKey );

protected:

	struct MapEntry
	{
		MapEntry( GroupId id = 0, char * pText = 0 ) : m_Group( id ), m_pText( pText ) {}

		GroupId	m_Group;	// Group ID
		char *	m_pText;	// String value
	};

	typedef std::map< Key, MapEntry > TextMap;

	// The data
	TextMap		m_Data;

private:

	// Scan the data for a key-string pair
	static void ScanKeyStringPair( char * * ppData, int size, char * * ppKey, char * * ppString );

	// Scan the data for a delimited string
	static void ScanString( char * * ppData, int size, int delimiter );

	struct GroupInfo
	{
		GroupId	m_GroupId;
		bool	m_IsOwned;
	};

	// Save the group info for a group
	void AddGroupInfo( GroupId id, bool isOwned );

	// Return the "owned" status of a group
	bool GroupIsOwned( GroupId id ) const;

	// Remove the group info for a group
	void RemoveGroupInfo( GroupId id );

	GroupInfo	m_GroupInfo[ 16 ];		// Group info for 16 groups
};



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline char const * TextContainer::Get( char const * pTextKey ) const
{
	return Get( MakeKey( pTextKey ) );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline char const * TextContainer::operator[]( Key key ) const
{
	return Get( key );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline char const * TextContainer::operator[]( char const * pTextKey ) const
{
	return Get( pTextKey );
}


#endif // !defined( TEXTCONTAINER_H_INCLUDED )
