/*
 * six_string.c
 *
 *  Created on: Aug 4, 2013
 *      Author: six
 */

#include <stdlib.h>
#include "six_bool.h"
bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void Uppercase(char *s)
{
	for(int i = 0; s[i]; i++)
    {
       s[i] = toupper(s[i]);
    }
}

void ChangeFileExt(char *s, char t[4]) //requires a filename.ext at *s, 3 chars in t, don't fuck around.
{
	char * pch;
	pch=strchr(s,'.');
	if (pch != NULL)
	{
		pch[0] = t[0];
		pch[1] = t[1];
		pch[2] = t[2];
		pch[3] = t[3];
	}
}


