/*
	libcharguess	-	Guess the encoding/charset of a string
    Copyright (C) 2003  Stephane Corbe <noubi@users.sourceforge.net>
	Based on Mozilla sources

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "universal.h"
#include "string.h"

nsUniversalDetector* det;

extern "C" int CharGuessInit(void)
{
	det = new nsUniversalDetector;
	if (det)
		return 0;
	else
		return -1;
}

extern "C" const char* GuessChardet(const char *str)
{
	const char* tmp;

	if (!det)
	{
		CharGuessInit();
		return NULL;
	}
	det->Reset();
	det->HandleData(str, strlen(str));
	det->DataEnd();
	tmp = det->GetCharset();

	return tmp;
}

extern "C" int CharGuessDone(void)
{
	if (det)
		delete det;

    return 0;
}
