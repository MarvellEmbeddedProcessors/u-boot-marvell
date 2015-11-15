/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mv_hws_strtol.c
*
* DESCRIPTION:
*     strtol(), strtoul implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

static int todigit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    return -1;
}

static unsigned long _strtol(
    const char *nptr,
    char **endptr,
    int base,
    int *neg
)
{
    const char *s = nptr;
    unsigned long acc;
    int any, c;

    *neg = 0;

    /* skip white space */
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\12')
        s++;
    /* pick up leading +/- sign if any. */
    if (*s == '-') {
        *neg=1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    if ((base == 0 || base == 16) &&
        *s == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if ((base == 0 || base == 2) &&
        *s == '0' && (s[1] == 'b' || s[1] == 'B')) {
        s += 2;
        base = 2;
    }
    if (base == 0)
        base = (*s == '0') ? 8 : 10;
    /* convert */
    for (acc = 0, any = 0; *s; s++) {
        c = todigit(*s);
        if (c < 0 || c >= base)
            break;
        any = 1;
        acc *= base;
        acc += c;
    }
    if (endptr)
        *endptr = (char *)(any ? s : nptr);
    return acc;
}

long hws_strtol(
    const char *nptr,
    char **endptr,
    int base
)
{
    int neg = 0;
    long val;
    val = _strtol(nptr, endptr, base, &neg);
    return neg ? -val : val;
}

unsigned long hws_strtoul(
    const char *nptr,
    char **endptr,
    int base
)
{
    int neg = 0;
    return _strtol(nptr, endptr, base, &neg);
}

