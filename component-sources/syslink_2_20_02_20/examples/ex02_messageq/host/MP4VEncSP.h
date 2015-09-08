//=============================================================================
//	THIS SOURCE CODE IS PROPRIETARY INFORMATION BELONGING TO INTERVIDEO, INC.
// 	ANY USE INCLUDING BUT NOT LIMITED TO COPYING OF CODE, CONCEPTS, AND/OR
//	ALGORITHMS IS PROHIBITED EXCEPT WITH EXPRESS WRITTEN PERMISSION BY THE 
//	COMPANY.
//
// 	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// 	PURPOSE.
//
// 	Copyright (c) 2004 - 2005  InterVideo Corporation.  All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _MP4VENCSP_H_
#define _MP4VENCSP_H_

#include "IviTypedef.h"
#include "IviReturn.h"
#include "bitstream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	YUV420,
	YUV422,									/// not support yet
	RGB565,									/// not support yet
	NV12,
} FRAME_TYPE;

typedef enum
{	
	Q_H263,
	Q_MPEG4
} QUANT_TYPE;

typedef struct 
{
	iviBool		userDoNotEncode;			/// a flag to indicate whether to encode this frame
	iviBool		DoNotEncode;				/// the same flag only this time is controled by rc and not by the user
	ivi8u		*pY;
	ivi8u		*pU;
	ivi8u		*pV;
	FRAME_TYPE	FrameType;					/// frame type
	ivi32u		TimeStamp;					/// current frame timestamp

    ivi32u		YStride;						///liuxu, 10/17/2013, image line stride
	ivi32u		UVStride;
	
	ivi32u		PicType;					// 0: I, 1: P, 2: B
	//MK.
	ivi32u		SrcEncodeRate;
} FRAME_STRUCT;

typedef struct  
{
	ivi32u		iTotalFrame;				/// total frames encoded up to now
	ivi32u		iTotalByte;				/// total bytes up to now
	ivi32u		iSSE;						/// PSNR of current frame
	ivi32u		iCurrQuant;				/// quant step of current frame
} ENC_STATUS;

typedef iviRet
(*PFN_MP4_GET_BUFF)(
					void	*pvGetBufContext,
					ivi8u	**ppbBuffer,  
					ivi32u	*puiNumberOfBytes 
					);

typedef iviRet
(*PFN_MP4_SEND_DATA)(
					 void	*pvSendDataContext,
					 ivi8u	*pbOutBuffer,
					 ivi32u	dwNumberOfBytes,
					 ivi32u	dwTimeStamp,
					 ivi32u	iStatus							/// entire frame bitstream, iStatus set 1, 
															/// part frame bitstream, iStatus set 0, 
					 );

typedef struct _Bitstream_
{
	//	FILE  *output;
	ivi8u *out_buff;
	//	ivi8u *out_base;
	ivi32u   m_BuffIndex;
	ivi32u   m_ulCounter;				//bit counter of the stream
	ivi32s    m_iNumOfFreeBitInWord;	//Num of available bits in the word (8/32 bit)
	ivi32u   m_32bitvalue;			//32-bit word - not used in 8-bit mode
	ivi8u *m_UCharptr32bitvalue;	//ivi8u Pointer to the last byte (base+3) of the 32-bit word - little-endian specific - not used in 8-bit mode

	// Callback functions, added by joshua for new interface function, 04/27/2005
	ivi32s					m_TimeStamp;			// time stamp of this bitstream
	PFN_MP4_GET_BUFF	pfnMP4GetBuff;
	void				*pvGetBufContext;
	PFN_MP4_SEND_DATA	pfnMP4SendData;
	void				*pvSendDataContext;
	ivi32u				m_GetBuffSize;			// 
	ivi32u				m_BuffSize;				// = m_GetBuffSize - 4
	ivi8u				m_Temp[4];				// for temporarily store 
	//end joshua
} Bitstream;


/*! encoder's open options */
typedef struct MP4VEncSP_OpenOptions_
{
	/* User input from parameter file */
	ivi8s InputRawDataFileName[200];
	ivi8s OutputBitstreamFileName[200];
	ivi8s OutputReconstructedFileName[200];
	
	ivi32u iFrameWidth;						/// multiple of 16, greater than 0 recommend higher than or equal to 112. default: 176.
	ivi32u iFrameHeight;						/// multiple of 16, greater than 0. recommend higher than or equal to 96. default: 144.

	ivi32s  bRCEnable;							/// 2-> RateControl with skip frames Enable  1 -> Ratecontrol Enable  0 -> RateControl Disable
	ivi32u iRCChoice;							/// Ratecontrol choice 
	ivi32u iSrcFrmRate;
	ivi32u iSrcEncodeRate;
	ivi32u iRawdataLength;
	//ivi32f DisplayFrameRate;					/// [1..60]  default: 15.

	FRAME_TYPE  iColorFormat;
	ivi32u dwBitRate;							// unit: kbps, not support yet

	ivi32u iEncodeQuality;					// 0 for minimum quality & maximum speed
											// 1 for medium quality & medium speed
											// 2 for maximum quality & minimum speed

	ivi32s iThreshold_4mv;					// Recommanded -- 2000
											// Optional	   -- 2500	
											// Optional	   -- 3000	
											// Optional	   -- 3500
	
	ivi32s iThresholdType_me;				//Integer MV early exit type
											// 0 -- the average of all the previously encoded MBs
											// 1 -- the minimal of the neighbour (Left, top, topright)
	
	ivi32s iThresholdDelta_me;				// Just effective when  iThresholdType_me == 1
											// 0 -- Threshold_me
											// 1 -- 1.1 * Threshold_me
											// 2 -- 1.2 * Threshold_me
											// 3 -- 1.3 * Threshold_me
											// 4 -- 1.4 * Threshold_me
											// 5 -- 1.5 * Threshold_me
	

	/* User input in sample code */

	//For VOL Level
	ivi32s	 bShortHeaderEnable;
	/* 2 Option: 1 -> fill frame edge Enable 0 -> do not fill frame edge
	** used in quality==1
	*/
	ivi32s	 bFillEdgeEnable;
	
	ivi32s  bObjectLayerIdentifier;
	ivi32s  iVolVerID;
	ivi32s  iVolPriority;
	ivi32s  bVolRandomAccess;
	ivi32s  iVideoObjectTypeID;				// Video Object Type Identification 
	ivi32s	 iAspectRatioInfo;
	ivi32s	 bVolControlPara;
	ivi32s	 iVideoObjectLayerShape;
	ivi32s	 bFixedVopRate;
	
	ivi32s	 iClockRate;						// Clock Rate
	ivi32u iNumBitsTimeIncr;					// Number of Bits for Time Increment
	ivi32s  bComplexEstDisable;				// Complexity Estimation Disable -> 1 Complexity Estimation Enable -> 0
	
	ivi32s	 bOBMC_Disable;
	ivi32s	 bInterlaced;
	ivi32s	 bSpriteEnable;
	ivi32s	 bNot8Bit;
	ivi32s	 bScalability;
	ivi32s	 bQuantType;						// H.263 Method or MPEG-4 Method
	ivi32s  bIntraQMatrixDefault;				// Use Default Intra Quantization Matrix
	ivi32s  bInterQMatrixDefault;				// Use Default Inter Quantization Matrix
	ivi8u *IntraQMatrix;					// User configured Intra Quantization Matrix
	ivi8u *InterQMatrix;					// User configured Inter Quantization Matrix

	ivi32s	 bResyncDisable;					// Error resillience Resync mode disable or not
	ivi32s	 bDataPartitioned;					// Error resillience Data Partition mode enable or not
	ivi32s  bReverseVLC;						// Error resillience Reversible Varied Length Coding enable or not
	ivi32s  iVideoPacketLength;				// Video packet length, bit unit
	ivi32s  bAdapativeIntraRefreshEnable;		// Adapative Intra Macroblock Refreshment Enable
	ivi32s  iAdapativeIntraRefreshRate;		// the number of VOPs in which all macroblocks refreshed a round

	ivi32s	 bQuarterPixel;
	
	// For VOP Level
	ivi32s	 iForwardFCode;
	ivi32s	 iBackwardFCode;	
	ivi32s	 bDoubleIEnable;
	ivi32s	 bCIREnable;						//Unknown Features  Haihua
	ivi32u iCIRRate;							//Unknown Features  Haihua
	ivi32s	 bSMCEnable;						//Unknown Features  Haihua
	ivi32u iIntraDcVlcThr;					// Intra DC VLC Threshold
	ivi32s  bRoundingCtrlEnable;				// Rounding Control Enable -> 1 Rounding Control Disable -> 1
	ivi32s  iRoundingCtrlInit;					// Initial Rounding Control Value 
	
	ivi32u iKeyFrameInterval;				/// [1..30]  [highest bitrate .. default] [fastest encoding .. default] I-Frame interval.
	
	
	ivi32u iQuantStep_I;						/// [1..31]  [Best .. Worst] quant scale of I frame. default: 8.
	ivi32u iQuantStep_P;						/// [1..31]  [Best .. Worst] quant scale of P frame. default: =iQpOffset_I
	
	ivi32u dwSearchRange;						// mv search range, not support yet

	// For short header only
	ivi32s iTemporalReference;				
	ivi32s	bZeroBit;
	ivi32s bSplitScreenIndicator;
	ivi32s	bDocumentCameraIndicator;
	ivi32s bFullPictureFreezeRelease;

	// Callback functions
	PFN_MP4_GET_BUFF	pfnMP4GetBuff;
	void				*pvGetBufContext;
	PFN_MP4_SEND_DATA	pfnMP4SendData;
	void				*pvSendDataContext;

}MP4VEncSP_OpenOptions;

/*! The MP4VEncSP_Create creates MP4 SP Encoder object.
  \param pCoDec [out] CoDec Object.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_Create(
    void **ppEncoder
    );

typedef iviRet (*MP4VEncSP_CreateFcn)(void **ppEncoder);

/*! The MP4VEncSP_Release releases MP4 SP Encoder object.
  \param pCoDec [in] CoDec Object.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_Release(
    void *pEncoder
    );

typedef iviRet (*MP4VEncSP_ReleaseFcn)(void *pEncoder);


/*! The MP4VEncSP_Open prepares MP4 SP Encoder to start.
  \param pEncoder [in] MP4VEncSP Object.
  \param pOptions [in] MP4VEncSP_OpenOptions encode options.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_Open(
    void *pEncoder,
	const MP4VEncSP_OpenOptions *pOptions
	);

typedef iviRet (*MP4VEncSP_OpenFcn)(void *pEncoder, const MP4VEncSP_OpenOptions *pOptions);

/*! The MP4VEncSP_Close returns resources MP4VEncSP Encoder used.
  \param pEncoder [in] MP4VEncSP Object.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_Close(
    void *pEncoder
	);

typedef iviRet (*MP4VEncSP_CloseFcn)(void *pEncoder);


/*! The MP4VEncSP_EncodeFrame encodes Frame in Y,U,V input buffer.
  \param pEncoder [in] MP4VEncSP Object.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_EncodeFrame(
	void *pEncoder,
	FRAME_STRUCT *pInputFrame
    );
typedef iviRet (*MP4VEncSP_EncodeFrameFcn)(void *pEncoder, FRAME_STRUCT *pInputFrame);

/*! The MP4VEncSP_GetStatus retrieves Encoder status.
  \param pEncoder [in] MP4VEncSP Object.
  \param ENC_STATUS [out] Encoder status Structure.
  \return Returns an iviRet value.
*/
iviRet
MP4VEncSP_GetStatus(
	void *pEncoder,
	ENC_STATUS *EncStatus,
	iviBool NotEncoded
	);
typedef iviRet (*MP4VEncSP_GetStatusFcn)(void *pEncoder, ENC_STATUS *EncStatus,iviBool NotEncoded);



#ifdef __cplusplus
}
#endif

#endif // _MP4VENCSP_H_
