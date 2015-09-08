/******************************************************************************
//  Description:    Parser MPEG-4 encoder parameter
//	Function list:
//		GetPar_MPEG4
//
******************************************************************************/
#include "IviTypedef.h"
#include "IviReturn.h"
#include "ParaParser.h"
#include "MP4VEncSP.h"
#include "stdio.h"
#include "stdlib.h"

#define RECTANGULAR 0
#define linux//liuxu, 8/22/2013.

/***************************************************************************
// Name:             GetPar_MPEG4
// Description:      Get parameters from input PAR file to OpenOptions structure
//
// Input Arguments:
//      pfPara		 Pointer to the input parameter file stream buffer
// Output Arguments:
//      pOpenOption  Pointer to the updated input OpenOptions structure
//
//	Returns:	
//		1 - IVI_OK
//		0 - IVI_FAIL
*****************************************************************************/
iviRet Get_MPEG4Par_File (void *pOption, FILE *pfPara)
{
	/* all the parameters to the encoder*/
	FILE *m_fp, *compressedfile=NULL;
	int output = 0;
	
	int  uiFrmWidth, uiFrmHeight;

	char *pchRawDataFileName, *pchCompressedDataFileName=NULL, *pchReconDataFileName=NULL, *pchInputDir, *pchOutputDir;
	char pchSrcRaw[200], pchDstCompressed[200], pchDstRecon[200];

	char pchBuf[TOKEN_SIZE], pchName[TOKEN_SIZE];
	
	int  rguiBitsBudget;
	int	 rgdInputLength;
	int  rgdFrameFrequency;	
	int	 iEncodeRate;
	FRAME_TYPE  ColorFormat;
	int	 bRateControl;
	int  iLen, er;
	char *pchEnd;
	int  EncodeQuality;
	MP4VEncSP_OpenOptions *pOpenOption;

	pOpenOption = (MP4VEncSP_OpenOptions *)pOption;
	
	//Set Default Value
	pchInputDir = NULL;
	pchOutputDir = NULL;
	pchRawDataFileName = NULL;
	pchCompressedDataFileName = NULL;
	rgdFrameFrequency = 30;
	iEncodeRate = 1;
	uiFrmWidth = 176;
	uiFrmHeight = 144;
	rguiBitsBudget = 300000;
	bRateControl = 0;
	ColorFormat = NV12;//liuxu, 8/21/2013, change the default to NV12. 
	
	er = ERES_OK;
	
	m_fp = pfPara;
	while (!(parGetToken(m_fp, pchBuf)==ERES_EOF)){

		strcpy(pchName, pchBuf);
//		iInd = -1;

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if(pchBuf[0]!='=')
			er = ERES_FORMAT;

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if (strcmp(pchName, "Source.Directory") == 0){				
			iLen = strlen(pchBuf) + 1;
			pchInputDir = (char *)malloc(iLen*sizeof(char));
			memcpy(pchInputDir, pchBuf, iLen);

		}else if(strcmp(pchName, "Source.RawDataFile") == 0){
			iLen = strlen(pchBuf) + 1;
			pchRawDataFileName = (char *)malloc(iLen*sizeof(char));
			memcpy(pchRawDataFileName, pchBuf, iLen);

		}else if(strcmp(pchName, "Source.Length") == 0){
			rgdInputLength = strtol(pchBuf, &pchEnd, 10);

		}else if(strcmp(pchName, "Output.Directory") == 0){
			iLen = strlen(pchBuf) + 1;
			pchOutputDir = (char *)malloc(iLen*sizeof(char));
			memcpy(pchOutputDir, pchBuf, iLen);

		}else if(strcmp(pchName, "Output.CompressedBitstream") == 0){
			iLen = strlen(pchBuf) + 1;
			pchCompressedDataFileName = (char *)malloc(iLen*sizeof(char));
			memcpy(pchCompressedDataFileName, pchBuf, iLen);
			output = 1;

		}else if(strcmp(pchName, "Output.ReconstructedDataFile") == 0){
			iLen = strlen(pchBuf) + 1;
			pchReconDataFileName = (char *)malloc(iLen*sizeof(char));
			memcpy(pchReconDataFileName, pchBuf, iLen);

		}else if(strcmp(pchName, "Source.FrameRate") == 0){
			rgdFrameFrequency = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "Source.EncodeRate") == 0){
			iEncodeRate = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "Source.Width") == 0){
			uiFrmWidth = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "Source.Height") == 0){
			uiFrmHeight = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "RateControl.BitRate") == 0){
			rguiBitsBudget = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "RateControl.Enable") == 0){
			bRateControl = strtol(pchBuf, &pchEnd, 10);
		}else if(strcmp(pchName, "Source.ColorFormat") == 0){
			if(strcmp(pchBuf, "YUV420")==0)
				ColorFormat = YUV420; 
			else if(strcmp(pchBuf, "RGB565")==0)
				ColorFormat  = RGB565;     
			else if(strcmp(pchBuf, "YUV422")==0)
				ColorFormat  = YUV422;  
		    else if(strcmp(pchBuf, "NV12")==0)
				ColorFormat = NV12;   
			else {
				printf("Not supported Color Format!\n");
				er = ERES_FORMAT;
			}
		}else if(strcmp(pchName, "Output.EncodeQuality") == 0){
			EncodeQuality = strtol(pchBuf, &pchEnd, 10);
		}else{
			er = ERES_FORMAT;
		}
	};

	if(er!=ERES_OK) {
		printf("error %d at some line of parameter file\n", er );
		return IVI_FAIL;
	}
	

	/* set pInfo values */

	pOpenOption->iFrameWidth  = uiFrmWidth;
	pOpenOption->iFrameHeight = uiFrmHeight;	
	pOpenOption->iSrcFrmRate = rgdFrameFrequency;
	pOpenOption->iSrcEncodeRate = iEncodeRate;

#ifdef linux
	sprintf (pchSrcRaw, "%s/%s", pchInputDir, pchRawDataFileName);
	sprintf (pchDstCompressed, "%s/%s", pchOutputDir, pchCompressedDataFileName);
#else
	sprintf (pchSrcRaw, "%s\\%s", pchInputDir, pchRawDataFileName);
	sprintf (pchDstCompressed, "%s\\%s", pchOutputDir, pchCompressedDataFileName);
#endif	
	strcpy(pOpenOption->InputRawDataFileName, pchSrcRaw);

	pOpenOption->bRCEnable = bRateControl;
	pOpenOption->dwBitRate = rguiBitsBudget;
	
	pOpenOption->iColorFormat = ColorFormat;
	pOpenOption->iEncodeQuality = EncodeQuality;

	pOpenOption->iRawdataLength = rgdInputLength;

	if(output)
	{
		if ( *pchCompressedDataFileName != 0 ) {
			strcpy(pOpenOption->OutputBitstreamFileName, pchDstCompressed);
		} else {
			strcpy(pOpenOption->OutputBitstreamFileName, pchCompressedDataFileName);
		}
		
		if ((compressedfile = fopen((const char*)(pOpenOption->OutputBitstreamFileName),"wb")) == NULL)
		{
			printf("cannot open output file, file: %s has problems\n", pOpenOption->OutputBitstreamFileName);
			return IVI_FAIL;
		} 
	}

	{

		pOpenOption->pvSendDataContext = (void **)compressedfile;
		pOpenOption->pvGetBufContext   = (void **)compressedfile;

		pOpenOption->pfnMP4GetBuff			= MP4GetBuff;
		pOpenOption->pfnMP4SendData			= MP4SendData;

	}

	pOpenOption->iThreshold_4mv = 2000; 	// Recommanded -- 2000
											// Optional	   -- 2500	
											// Optional	   -- 3000	
											// Optional	   -- 3500
	
	pOpenOption->iThresholdType_me = 0;		//Integer MV early exit type
											// 0 -- the average of all the previously encoded MBs
											// 1 -- the minimal of the neighbour (Left, top, topright)	
#ifdef _EPZS
		pOpenOption->iThresholdType_me = 1;
#endif

	pOpenOption->iThresholdDelta_me = 4;	// Just effective when  iThresholdType_me == 1
											// 0 -- Threshold_me
											// 1 -- 1.1 * Threshold_me
											// 2 -- 1.2 * Threshold_me
											// 3 -- 1.3 * Threshold_me
											// 4 -- 1.4 * Threshold_me
											// 5 -- 1.5 * Threshold_me		

	free(pchInputDir);
	free(pchOutputDir);
	free(pchRawDataFileName);
	if(pchCompressedDataFileName)
		free(pchCompressedDataFileName);
	if(pchReconDataFileName)
		free(pchReconDataFileName);


	return IVI_OK;
}

/***************************************************************************
// Name:             ConfigureStandardPar_MPEG4
// Description:      Configure MPEG-4 standard compatible parameters in sample code
//
// Output Arguments:
//      pOpenOption  Pointer to the updated input OpenOptions structure
//
//	Returns:	
//		1 - IVI_OK
//		0 - IVI_FAIL
*****************************************************************************/
iviRet ConfigureStandardPar_MPEG4(void *pOption)
{

	MP4VEncSP_OpenOptions *pOpenOption;

	pOpenOption = (MP4VEncSP_OpenOptions *)pOption;

	pOpenOption->bObjectLayerIdentifier = 0;
		/* 2 Option: 1 -> version identification and priority is specified for the visual object layer
					 0 -> no version identification or priority needs to be specified
		*/

	pOpenOption->iVolVerID = 2;			
		/* 2 Option: 1 -> Version 1; 2 -> Version 2 */

	pOpenOption->iVolPriority = 1;
		/* 7 Option: 1 ~ 7; 
					 1 - Highest Priority
					 7 - Lowest Priority
		*/

	pOpenOption->bVolRandomAccess = 0;
		/* 2 Option: 1 -> every VOP in this VOL is individually decodable
					 0 -> any of the VOPs in the VOL are non-intra coded
		*/

	pOpenOption->iVideoObjectTypeID = 0x04;		//Potential problem, Main Profile? Haihua
		/* Video Object Type Identification 
			Reserved						0x00
			Simple Object Type				0x01	(Supported)
			Simple Scalable Object Type		0x02	(Not Supported)
			Core Object Type				0x03	(Not Supported)
			Main Object Type				0x04	(Not Supported)
			N-bit Object Type				0x05	(Not Supported)
			Basic Anim.2D Texture			0x06	(Not Supported)
			Anim.2D Mesh					0x07	(Not Supported)
			Simple Face						0x08	(Not Supported)
			Still Scalable Texture			0x09	(Not Supported)
			Advanced Real Time Simple		0x0a	(Not Supported)
			Core Scalable					0x0b	(Not Supported)
			Advanced Coding Efficiency		0x0c	(Not Supported)
			Advanced Scalable Texture		0x0d	(Not Supported)
			Simple FBA						0x0e	(Not Supported)
			Reserved						0x0f ~ 0xff
		*/


	pOpenOption->iAspectRatioInfo = 1;

	pOpenOption->bVolControlPara = 0;
		/* 2 Option:  1 -> Control parameters in following syntax, 0 -> No control parameters in stream */

	pOpenOption->iVideoObjectLayerShape = RECTANGULAR;
	/*
		RECTANGULAR 0
		BINARY 1
		BINARY_ONLY 2
		GRAYSCALE 3  
	*/

	pOpenOption->bFixedVopRate = 1;
		

	pOpenOption->bQuantType = Q_H263;
		/* 2 Option:	Q_H263, Q_MPEG4 */

	pOpenOption->bIntraQMatrixDefault = 1;
		/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pOpenOption->bInterQMatrixDefault = 1;
		/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pOpenOption->IntraQMatrix = NULL;
		/* Customer Input Intra Quantization Maxtrix, 64 sub-values */

	pOpenOption->InterQMatrix = NULL;
		/* Customer Input Inter Quantization Maxtrix, 64 sub-values */

	pOpenOption->iKeyFrameInterval = 30;
		/* Positive integer indicates number of P-frame between two neighbouring I-frames, 
		   -1 indicates just one I-frame in starting point, no more I-frame */
	
	pOpenOption->iIntraDcVlcThr = 0;
		/* 8 Option:  0, 1, 2, 3, 4, 5, 6, 7 */

	if (pOpenOption->bRCEnable) {
		pOpenOption->iRCChoice = 1;
	}
	/* only one Option support now: 1 */
	
	pOpenOption->iQuantStep_I = 12;
	//pOpenOption->iQuantStep_I = 20;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of I-VOP,
		   else it indicates the QP of all I-VOPs
		*/

	pOpenOption->iQuantStep_P = 12;
	//pOpenOption->iQuantStep_P = 22;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of P-VOP,
		   else it indicates the QP of all P-VOPs
		*/

	pOpenOption->iRoundingCtrlInit = 1;
		/* 2 Option: 0, 1 */

	
	pOpenOption->bComplexEstDisable = 1;
		/* 2 Option: 1 -> Disable; 0 -> Enable; */

	pOpenOption->bResyncDisable = 1;
		/* 2 Option: 1 -> Disable; 0 -> Enable; */	
	pOpenOption->bDataPartitioned = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */	
	pOpenOption->iVideoPacketLength = 500;
		/* Posititve Integer indicates the target number of bit in packet */	

	pOpenOption->bReverseVLC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pOpenOption->bScalability = 0;
		/* 2 Option: 1 -> Scalable mode Enable; 0 -> Scalable mode Disable; */

	pOpenOption->bAdapativeIntraRefreshEnable = 0;
		/* 2 Option: 1 -> Adapative Intra Macroblock Refreshment Enable 
					 0 -> Adapative Intra Macroblock Refreshment Disable
		*/
    pOpenOption->iAdapativeIntraRefreshRate = 0;
		/* Positive Integer indicates the number of VOPs in which all macroblocks refreshed a round */

	
	pOpenOption->bAdapativeIntraRefreshEnable = 0;	
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	
	pOpenOption->bShortHeaderEnable = 0;
		/* 2 Option: 1 -> Shortheader Enable (H263 baseline) 0 -> Normal MPEG-4 */

	pOpenOption->bFillEdgeEnable = 1;
	/* 2 Option: 1 -> fill frame edge Enable 0 -> do not fill frame edge
	** used in quality==1
	*/

	if ( pOpenOption->bShortHeaderEnable ) {
		
		pOpenOption->bRoundingCtrlEnable = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
	}else
	{
		pOpenOption->bRoundingCtrlEnable = 1;
	}

	if ( pOpenOption->bShortHeaderEnable ) {
		pOpenOption->iTemporalReference = 0;
		pOpenOption->bZeroBit			= 0;
		pOpenOption->bSplitScreenIndicator		=0;
		pOpenOption->bDocumentCameraIndicator	=0;
		pOpenOption->bFullPictureFreezeRelease	=0;
		
	}

	pOpenOption->dwSearchRange = 15;
		/* Positive Integer indicates the motion estimation search range, pixel unit */

	pOpenOption->bOBMC_Disable = 1;
		/* 2 Option: 1 -> OBMC mode disabled  0 -> OBMC mode enabled */

	pOpenOption->bInterlaced	= 0;
		/* 2 Option: 1 -> Interlace mode enabled  0 -> Interlace mode disabled */

	pOpenOption->bSpriteEnable	= 0;
		/* 2 Option: 1 -> Sprite mode enabled  0 -> Sprite mode disabled */

	pOpenOption->bNot8Bit		= 0;
		/* 2 Option: 1 -> Not 8 bit in each pixel input 0 -> Always 8 bit in pixel input */

	pOpenOption->iForwardFCode = 1;
		/* Positive integer indicate forward f code in forward motion vector coding */

	pOpenOption->iBackwardFCode = 1;
		/* Positive integer indicate backward f code in backward motion vector coding */

	pOpenOption->bQuarterPixel = 0;
		/* 2 Option: 1 -> Enable Quarter Pixel in motion estimation  0 -> Disabled */

	pOpenOption->bDoubleIEnable = 0;
	/* 2 Option: 1 -> Enable double I frame in on GOP  0 -> Disabled */

	pOpenOption->bCIREnable = 0;
	/* 2 Option: 1 -> Enable CIR  0 -> Disabled */

	pOpenOption->iCIRRate = 0;
	/* Positive value indicates ... */

	pOpenOption->bSMCEnable = 0;
	/* 2 Option: 1 -> Enable SMC  0 -> Disabled */


		

	return IVI_OK;

}
