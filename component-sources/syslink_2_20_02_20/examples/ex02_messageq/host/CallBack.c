#include <stdlib.h>
#include <stdio.h>
#include "IviTypedef.h"
#include "IviReturn.h"


unsigned char DATA_GET[4096];

iviBool MP4GetBuff(
				void	*pvGetBufContext,
				ivi8u	**ppbBuffer,  
				ivi32u	*puiNumberOfBytes 
				)
{
	// may use ping-pong buffer
	*ppbBuffer = DATA_GET;
	*puiNumberOfBytes = sizeof(DATA_GET);
	
	// 0 represent unsuccess!
	return iviTrue;
}

iviBool MP4SendData(
				 void	*pvSendDataContext,
				 ivi8u	*pbOutBuffer,
				 ivi32u	dwNumberOfBytes,
				 ivi32u	dwTimeStamp,		/// frame number, 0 for header
				 ivi32u	iStatus				/// entire frame bitstream, iStatus set 1, 
											/// part frame bitstream, iStatus set 0, 
				 )
{
	if(pvSendDataContext)
		fwrite(pbOutBuffer, sizeof(char), dwNumberOfBytes, (FILE *)pvSendDataContext);
	
	return iviTrue;
}