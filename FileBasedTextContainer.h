#if !defined( FILEBASEDTEXTCONTAINER_H_INCLUDED )
#define FILEBASEDTEXTCONTAINER_H_INCLUDED

#pragma once

/*****************************************************************************

                           FileBasedTextContainer.h

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/TextContainer/FileBasedTextContainer.h#2 $

	$NoKeywords: $

*****************************************************************************/

#include "TextContainer.h"

class FileBasedTextContainer : public TextContainer  
{
public:
//	FileBasedTextContainer();
	virtual ~FileBasedTextContainer();

	// *** TextContainer overrides
	// Load a group of text in a  tab-delimited file. An id is returned.
	// The function failed if the returned value is 0. This function is
	// fairly slow.
	// NOTE: In TextContainer, the parameter is the data but here it is the
	// name of the file.
	GroupId Load( char const * pFilename );
	// *** EndTextContainer overrides
};


#endif // !defined( FILEBASEDTEXTCONTAINER_H_INCLUDED )
