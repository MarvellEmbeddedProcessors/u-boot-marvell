/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "lib_utils.h"
#include <stdlib.h>
#include "mv_os.h"
#include "mvUart.h"
#include "printf.h"

/*******************************************************************************
 * implementation of used ctype functions
 *******************************************************************************/

/*******************************************************************************
* isprint
*
* DESCRIPTION:
* Checks whether c is a printable character
* A printable character is a character that occupies a printing position on a display
*
*******************************************************************************/
int isprint(int c)
{
	return c >= ' ' && c <= '~';
}

/*******************************************************************************
* isblank1
*
* DESCRIPTION:
* Checks whether c is a blank character
* the blank characters are space and horizontal tab
*
*******************************************************************************/
int isblank1(char c)
{
	return (c >= 9 && c <= 13) || c == 32;
}

/*******************************************************************************
* isxdigit
*
* DESCRIPTION:
* Checks whether c is a hexdecimal digit character
* Hexadecimal digits are any of: 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F
*
*******************************************************************************/
int isxdigit(int c)
{
	if (c >= '0' && c <= '9')
		return 1;

	if (c >= 'A' && c <= 'F')
		return 1;

	if (c >= 'a' && c <= 'f')
		return 1;

	return 0;
}

/*******************************************************************************
* isdigit
*
* DESCRIPTION:
* Checks whether c is a decimal digit character
* Decimal digits are any of: 0 1 2 3 4 5 6 7 8 9
*
*******************************************************************************/
int isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return 1;

	return 0;
}

/*******************************************************************************
* islower
*
* DESCRIPTION:
* Checks whether c is a lowercase letter
*
*******************************************************************************/
int islower(int c)
{
	if (c >= 'a' && c <= 'z')
		return 1;

	return 0;
}

/*******************************************************************************
* toupper
*
* DESCRIPTION:
* Returns the uppercase equivalent of c
*
*******************************************************************************/
int toupper(int c)
{
	if (islower(c))
		return 'A' + (c - 'a');
	else
		return c;
}

/*******************************************************************************
 * implementation of used string functions
 *******************************************************************************/

/* strchr */
char *strchr(const char *s, int c)
{
/* Scan s for the character.  When this loop is finished,
 s will either point to the end of the string or the
 character we were looking for.  */
	while (*s != '\0' && *s != (char)c)
		s++;
	return (*s == (char)c) ? (char *) s : NULL;
}

/* strlen */
int strlen(const char *s)
{
	const char *p = s;
	/* Loop over the data in s.  */
	while (*p != '\0')
		p++;
	return (int)(p - s);
}

/* strcmp */
int strcmp(const char *s1, const char *s2)
{
	int __res;
	while (1) {
		__res = *s1 - *s2++;
		if (__res != 0 || !*s1++)
			break;
	}

	return __res;
}

/* strncmp */
int strncmp(const char *s1, const char *s2, int n)
{
	unsigned char uc1, uc2;
	/* Nothing to compare?  Return zero.  */
	if (n == 0)
		return 0;

	/* Loop, comparing bytes.  */
	while (n-- > 0 && *s1 == *s2) {
		/* If we've run out of bytes or hit a null, return zero
		since we already know *s1 == *s2.  */
		if (n == 0 || *s1 == '\0')
			return 0;

		s1++;
		s2++;
	}
	uc1 = (*(unsigned char *) s1);
	uc2 = (*(unsigned char *) s2);
	return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}
/* strcpy */
char *strcpy(char *s1, const char *s2)
{
	char *dst = s1;
	const char *src = s2;
	/* Do the copying in a loop.  */
	while ((*dst++ = *src++) != '\0')
		;               /* The body of this loop is left empty. */
	/* Return the destination string.  */
	return s1;
}

/*******************************************************************************
 * implementation of other utils
 *******************************************************************************/
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
	unsigned long result = 0, value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base)
			base = 8;
	}
	if (!base)
		base = 10;

	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

char console_buffer[CONFIG_SYS_CBSIZE + 1];
/*******************************************************************************
* readline
*
* DESCRIPTION:
* prints 'disp' and reads characters from UART until newline character(ACSII 13)
* received and writes the characters to 'console_buffer'
*
* INPUT:
*	disp	- string to display.
*
* RETURN:
*	Number of read characters, not including newline character
*
*******************************************************************************/
int readline(const char *disp)
{
	char c;
	int curr = 0;
	mvPrintf(disp);

	while (curr <= CONFIG_SYS_CBSIZE) {
		c = mvUartGetc();
		if (c == 13) {
			console_buffer[curr] = 0;
			putstring("\n");
			return curr;
		}
		mvUartPutc(c);
		console_buffer[curr++] = c;
	}
	console_buffer[CONFIG_SYS_CBSIZE] = 0;
	return curr;
}

