/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* Io.h
*
* DESCRIPTION:
*       Operating System wrapper. Input/output facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __Ioh
#define __Ioh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include "gtGenTypes.h"

/************* Typedefs *******************************************************/

typedef GT_U32 GT_FILEP;

/************* Functions ******************************************************/
typedef int (*OS_BIND_STDOUT_FUNC_PTR)(void* userPtr, const char* buffer, int length);


/*******************************************************************************
* osBindStdOut
*
* DESCRIPTION:
*       Bind Stdout to handler
*
* INPUTS:
*       writeFunction - function to call for output
*       userPtr       - first parameter to pass to write function
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osBindStdOut(
    IN OS_BIND_STDOUT_FUNC_PTR writeFunction,
    IN void* userPtr
);

/*******************************************************************************
* osStdin
*
* DESCRIPTION:
*       Returns file pointer to standard input console.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       File pointer to standard input console.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_FILEP osStdin(void);


/*******************************************************************************
* osStdout
*
* DESCRIPTION:
*       Returns file pointer to standard output console.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       File pointer to standard output console.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_FILEP osStdout(void);


/*******************************************************************************
* osFileno
*
* DESCRIPTION:
*       This routine returns the file descriptor associated with a 
*       specified stream
*
* INPUTS:
*       fp - stream
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The file descriptor, or -1 if an error occurs
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 osFileno
(
    GT_FILEP fp
);


/*******************************************************************************
* osLogFdSet
*
* DESCRIPTION:
*       This routine changes the file descriptor where messages from osLogMsg()
*       are written, allowing the log device to be changed from the default.
*
* INPUTS:
*       fd - file descriptor to use as logging device 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osLogFdSet
(
    IN GT_32  fd
);


/*******************************************************************************
* osLogMsg
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream through a task.
*
* INPUTS:
*       format  - format string to write
*       arg1    - first of six required args for fmt 
*       arg2    - Sec of six required args for fmt 
*       arg3    - 3 of six required args for fmt 
*       arg4    - 4 of six required args for fmt 
*       arg5    - 5 of six required args for fmt 
*       arg6    - 6 of six required args for fmt 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 osLogMsg
(
    IN char*        format,     
    IN GT_U32       arg1, /* first of six required args for fmt */
    IN GT_U32       arg2,
    IN GT_U32       arg3,
    IN GT_U32       arg4,
    IN GT_U32       arg5,
    IN GT_U32       arg6
);


/*******************************************************************************
* osSprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osSprintf(char * buffer, const char* format, ...);

/*******************************************************************************
* osFprintf
*
* DESCRIPTION:
*       Write a formatted string to the pointed stream.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osFprintf
(
    IN const GT_FILEP theFp,
    const char* theFormat,
    ...
);

/*******************************************************************************
* osGets
*
* DESCRIPTION:
*       Reads characters from the standard input stream into the array
*       'buffer' until end-of-file is encountered or a new-line is read.
*       Any new-line character is discarded, and a null character is written
*       immediately after the last character read into the array.
*
* INPUTS:
*       buffer  - pointer to buffer to write to
*
* OUTPUTS:
*       buffer  - buffer with read data
*
* RETURNS:
*       A pointer to 'buffer', or a null pointer if end-of-file is
*       encountered and no characters have been read, or there is a read error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
char * osGets(char * buffer);

/*******************************************************************************
* osFgets
*
* DESCRIPTION:
*       Reads characters from the file pointer input stream into the array
*       'buffer' until end-of-file is encountered or a new-line is read or 
*       number of characters (size) read.
*       Any new-line character is discarded, and a null character is written
*       immediately after the last character read into the array.
*
* INPUTS:
*       theBuffer   - pointer to buffer to write to
*       theSize     - maximum number of characters to read
*       theFp       - file pointer to read from 
*
* OUTPUTS:
*       theBuffer  - buffer with read data
*
* RETURNS:
*       A pointer to 'theBuffer', or a null pointer if end-of-file is
*       encountered and no characters have been read, or there is a read error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
char * osFgets (char * theBuffer, GT_U32 theSize, GT_FILEP theFp);

/*******************************************************************************
* osFopen
*
* DESCRIPTION:
*       Open a file. The mode argument is used just as in the 
*       fopen( ) function. 
*
* INPUTS:
*       filename    - filename
*       theMode     - open mode 
*
* OUTPUTS:
*                 - file pointer
*
* RETURNS:
*       A pointer to a stream, or a null pointer if an error occurs, with 
*       errno set to indicate the error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_FILEP osFopen 
(
    IN GT_U8        *filename,
    IN const char   *theMode
);

/*******************************************************************************
* osFclose
*
* DESCRIPTION:
*       Close a file. 
*
* INPUTS:
*       theFp   - the file pointer to close
*
* OUTPUTS:
*       None.        
*
* RETURNS:
*       GT_OK    - on success,
*       GT_FAIL  - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osFclose
(
    IN GT_FILEP theFp
);

/*******************************************************************************
* osFflush
*
* DESCRIPTION:
*       Flushes a stream.
*
* INPUTS:
*       theFp   - the file pointer to close
*
* OUTPUTS:
*       None.        
*
* RETURNS:
*       GT_OK    - on success,
*       GT_FAIL  - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osFflush
(
    IN GT_FILEP theFp
);

/*******************************************************************************
* osFdopen
*
* DESCRIPTION:
*       This routine opens the file specified by the file descriptor fd and
*       associates a stream with it. The mode argument is used just as in the
*       fopen( ) function. 
*
* INPUTS:
*       theFd       - file descriptor
*       theMode     - open mode 
*
* OUTPUTS:
*                 - file pointer
*
* RETURNS:
*       A pointer to a stream, or a null pointer if an error occurs, with 
*       errno set to indicate the error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_FILEP osFdopen 
(
    IN GT_U32       theFd, 
    IN const char   *theMode
);

/*******************************************************************************
* osIsSpace
*
* DESCRIPTION:
*       This routine tests whether a character is one of the standard 
*       white-space characters, as follows: 
*
*           space  "\0" 
*           horizontal tab  \t 
*           vertical tab  \v 
*           carriage return  \r 
*           new-line  \n 
*           form-feed  \f 
*
* INPUTS:
*       c           - character
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Non-zero if and only if c is a space, tab, carriage return, new-line, 
*       or form-feed character. 
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osIsSpace
(
    IN char c
);

#ifdef __cplusplus
}
#endif

#endif  /* __Ioh */
/* Do Not Add Anything Below This Line */

