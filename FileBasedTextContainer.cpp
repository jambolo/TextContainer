/*****************************************************************************

                          FileBasedTextContainer.cpp

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/TextContainer/FileBasedTextContainer.cpp#2 $

	$NoKeywords: $

*****************************************************************************/

#include "FileBasedTextContainer.h"




/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

FileBasedTextContainer::~FileBasedTextContainer()
{
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

FileBasedTextContainer::GroupId FileBasedTextContainer::Load( char const * pFilename )
{
	char *			pData		= 0;					// Pointer to loaded data
	int				size		= 0;					// Size of loaded data

	// Load the file into memory

	FILE *	fp	= fopen( pFilename, "rb" );
	if ( !fp )
	{
		printf( "Unable to open '%s' for reading.\n", pFilename );
		return 0;
	}

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	pData = new char[ size + 1 ];
	if ( pData == 0 )
	{
		fclose( fp );
		throw std::bad_alloc();
	}

	fread( pData, size, 1, fp );
	fclose( fp );

	pData[ size ] = 0;				// Guarantee that the last string is terminated

	// Load the TextContainer giving it ownership of the data

	return TextContainer::Load( std::auto_ptr< char >( pData ), size );
}
