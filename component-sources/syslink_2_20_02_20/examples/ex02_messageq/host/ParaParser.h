#ifndef	__PARAPARSER_H__
#define	__PARAPARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "IviTypedef.h"
#include "IviReturn.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOKEN_SIZE 4096
#define ERES_OK 0
#define ERES_PARAMS 1
#define ERES_NOOBJ 2
#define ERES_MEMORY 3
#define ERES_EOF 4
#define ERES_FORMAT 5

extern int parSkipSpace(FILE *m_fp);
extern int parSkipComment(FILE *m_fp);
extern int parGetToken(FILE *m_fp, char *pchBuf);

extern iviRet Get_MPEG4Par_File (void *pOption, FILE *pfPara);
extern iviRet ConfigureStandardPar_MPEG4(void *pOption);

extern iviRet MP4GetBuff(
						 void	*pvGetBufContext,
						 ivi8u	**ppbBuffer,  
						 ivi32u	*puiNumberOfBytes 
						 );

extern iviRet MP4SendData(
						  void	*pvSendDataContext,
						  ivi8u	*pbOutBuffer,
						  ivi32u	dwNumberOfBytes,
						  ivi32u	dwTimeStamp,		/// frame number, 0 for header
						  ivi32u	iStatus				/// entire frame bitstream, iStatus set 1, 
														  /// part frame bitstream, iStatus set 0, 
						  );

#ifdef __cplusplus
}
#endif

#endif