/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.6  1997/01/27 00:32:33  ldp
    Polish

    Revision 1.5  1996/12/10 14:00:16  aros
    Moved #include into first column to allow makedepend to see it.

    Revision 1.4  1996/10/24 22:51:46  aros
    Use proper Amiga datatypes (eg: ULONG not unsigned long)

    Revision 1.3  1996/10/24 15:51:39  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.2  1996/09/13 17:16:35  digulla
    Removed the macro TOUPPER. Use this library function instead

    Revision 1.1  1996/08/31 12:58:13  aros
    Merged in/modified for FreeBSD.


    Desc:
    Lang: english
*/
#include <exec/types.h>
#include <aros/libcall.h>
#include "utility_intern.h"

/*****************************************************************************

    NAME */
#include <proto/utility.h>

	AROS_LH1I(UBYTE, ToUpper,

/*  SYNOPSIS */
	AROS_LHA(ULONG, character, D0),

/*  LOCATION */
	struct UtilityBase *, UtilityBase, 29, Utility)

/*  FUNCTION
	Convert a character to uppercase

    INPUTS
	character   - The character that you want changed.

    RESULT
	The uppercase version of that character.

    NOTES
	Currently only works for ASCII characters. Would not be difficult
	to adapt for other character sets (Unicode for example).

	This function is patched by the locale.library, so you should be
	prepared for different results when running under different
	languages.

    EXAMPLE
	STRPTR string; UBYTE chr;

	\* Convert a string to uppercase *\
	while( chr = *string )
	{
	    *string = ToUpper( chr );
	    string++;
	}

    BUGS

    SEE ALSO
	utility/ToLower()

    INTERNALS
	This function is patched by locale.library.

    HISTORY
	29-10-95    digulla automatically created from
			    utility_lib.fd and clib/utility_protos.h
	10-08-96    iaint   Created from tolower.c from AROSdev15
*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return
    (
	(character >= 'a' && character <= 'z')
	|| (character >= 0xE0
	    && character <= 0xEE
	    && character != 0xE7)
	? character - 0x20
	: character
    );

    AROS_LIBFUNC_EXIT
} /* ToLower */
