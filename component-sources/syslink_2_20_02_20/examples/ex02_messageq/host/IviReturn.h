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
// 	Copyright (c) 2004 - 2006  InterVideo Corporation.  All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef __IVI_RETURN_H__
#define __IVI_RETURN_H__

/***********************************************************************  
//	Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//	        | Major |     Minor     |    General    |   Specific    |
//          | Domain|     Domain    |      Info     |     Info      |
//
//  where
//
//      Sev - is the severity code
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag, always set to 1
//
//      R - is a reserved bit, set to 0
//
//      Facility - is the facility code 12 bits
//			Major Domain - first 4 bits to indicate 'codec', 'system' or other domain
//			Minor Domain - last 8 bits to indicate specific part in Major Domain
//
//      Code - is the facility's status code 16 bits
//			General Info  - general information or errors, 8bits
//			Specific Info - specific information or errors, 8bits
*************************************************************************/

/************************************************************************
 * Define Succeeded & Failed macro
 ************************************************************************/
#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((ivi32s)(Status) >= 0)
#endif

#ifndef FAILED  
#define FAILED(Status) ((ivi32s)(Status) < 0)
#endif

/************************************************************************
 * Define Success & GENERAL Failure code
 ************************************************************************/
#define IVI_OK						0x00000000
#define IVI_FAIL					0xE0000000

/************************************************************************
 * Define Sev & C & R code, 4 bits
 ************************************************************************/
#define IVI_SEV_INFO				(0x6 << 28)
#define IVI_SEV_WARNING				(0xA << 28)
#define IVI_SEV_ERROR				(0xE << 28)

/************************************************************************
 * Define Major Domain of Facility, 4 bits
 ************************************************************************/
#define IVI_MAJOR_GENERAL			(0x0 << 24)
#define IVI_MAJOR_CODEC				(0x1 << 24)
#define IVI_MAJOR_SYSTEM			(0x2 << 24)

/************************************************************************
 * Define the facility code, 12 bits 
 ************************************************************************/
#define IVI_FAC_CODEC_GENERAL           (IVI_MAJOR_CODEC|0x00 << 16)
#define IVI_FAC_CODEC_MPEG4             (IVI_MAJOR_CODEC|0x01 << 16)
#define IVI_FAC_CODEC_AMR               (IVI_MAJOR_CODEC|0x02 << 16) 
#define IVI_FAC_CODEC_AMRWB             (IVI_MAJOR_CODEC|0x03 << 16) 
#define IVI_FAC_CODEC_BSAC              (IVI_MAJOR_CODEC|0x04 << 16) 
#define IVI_FAC_CODEC_AAC               (IVI_MAJOR_CODEC|0x05 << 16)
#define IVI_FAC_CODEC_H264              (IVI_MAJOR_CODEC|0x06 << 16)
#define IVI_FAC_CODEC_WMA               (IVI_MAJOR_CODEC|0x07 << 16)
#define IVI_FAC_CODEC_H263              (IVI_MAJOR_CODEC|0x08 << 16)

#define IVI_FAC_SYSTEM_GENERAL	(IVI_MAJOR_SYSTEM|0x00 << 16)
#define IVI_FAC_SYSTEM_MUX			(IVI_MAJOR_SYSTEM|0x01 << 16)

/************************************************************************
 * Define the prefix of return code 
 ************************************************************************/
#define IVI_ERRPRE_CODEC_GENERAL	(IVI_SEV_ERROR|IVI_FAC_CODEC_GENERAL)
#define IVI_ERRPRE_CODEC_MPEG4 		(IVI_SEV_ERROR|IVI_FAC_CODEC_MPEG4)
#define IVI_ERRPRE_CODEC_AMR   		(IVI_SEV_ERROR|IVI_FAC_CODEC_AMR)
#define IVI_ERRPRE_CODEC_AMRWB 		(IVI_SEV_ERROR|IVI_FAC_CODEC_AMRWB)
#define IVI_ERRPRE_CODEC_BSAC  		(IVI_SEV_ERROR|IVI_FAC_CODEC_BSAC)
#define IVI_ERRPRE_CODEC_AAC  		(IVI_SEV_ERROR|IVI_FAC_CODEC_AAC)
#define IVI_ERRPRE_CODEC_WMA        (IVI_SEV_ERROR|IVI_FAC_CODEC_AMR)

#define IVI_ERRPRE_CODEC_H264		(IVI_SEV_ERROR|IVI_FAC_CODEC_H264)
#define IVI_WARNING_CODEC_H264		(IVI_SEV_WARNING|IVI_FAC_CODEC_H264)
#define IVI_INFO_CODEC_H264			(IVI_SEV_INFO|IVI_FAC_CODEC_H264)
#define IVI_OK_CODEC_H264			(IVI_OK|IVI_FAC_CODEC_H264)

#define IVI_INFO_CODEC_H263         (IVI_SEV_INFO|IVI_FAC_CODEC_H263)
#define IVI_INFO_CODEC_MPEG4        (IVI_SEV_INFO|IVI_FAC_CODEC_MPEG4)

#define IVI_ERRPRE_SYSTEM_GENERAL	(IVI_SEV_ERROR|IVI_FAC_SYSTEM_GENERAL)
#define IVI_ERRPRE_SYSTEM_MUX		(IVI_SEV_ERROR|IVI_FAC_SYSTEM_MUX)

/************************************************************************
 * Define General Error of Code, 8 bits 
 * Usage: ( ERR_PREFIX | GEN_ERROR )
 ************************************************************************/
#define IVI_GENERR_UNDEFINED		(0x00 << 8)	/* undefined or unsupport error */
#define IVI_GENERR_INPUTERROR		(0x01 << 8)	/* input is error */
#define IVI_GENERR_NOMEMORY			(0x02 << 8)	/* no memory, failed of malloc */

#define IVI_GENERR_SYNCNOTFOUND		(0x03 << 8) /* Sync code not found in data buffer */
#define IVI_GENERR_ALIGNMENT		(0x04 << 8) /* Memory alignment condition not met */
#define IVI_GENERR_NOFREEFRAME		(0x05 << 8) /* no free frame buffer in buffer queue */ 

/************************************************************************
 * Define Specific Error of Code, 8 bits 
 * Usage: Use it directly            
 ************************************************************************/
/************************************************************************ 
 * MPEG-4 Decoder specific Error 
 ************************************************************************/
#define IVI_ERROR_MPEG4_ALIGNMENT		(IVI_ERRPRE_CODEC_MPEG4|IVI_GENERR_ALIGNMENT)
#define IVI_ERROR_MPEG4_SYNCNOTFOUND	(IVI_ERRPRE_CODEC_MPEG4|IVI_GENERR_SYNCNOTFOUND)
/************************************************************************ 
 * H263  Decoder specific Return code 
 ************************************************************************/
#define IVI_INFO_H263_MB_STUFFING       (IVI_INFO_CODEC_H263|0x01)
/************************************************************************ 
 * MPEG4 Decoder specific Return code 
 ************************************************************************/
#define IVI_INFO_MPEG4_MB_STUFFING      (IVI_INFO_CODEC_MPEG4|0x01)

/************************************************************************ 
 * H264 Base Line Decoder specific Return code 
 ************************************************************************/
// #define IVI_ERROR_H264_NOTSUPPORTED		(IVI_ERRPRE_CODEC_H264|IVI_GENERR_UNDEFINED)
// #define IVI_ERROR_H264_ALIGNMENT		(IVI_ERRPRE_CODEC_H264|IVI_GENERR_ALIGNMENT)
// #define IVI_ERROR_H264_SYNCNOTFOUND		(IVI_ERRPRE_CODEC_H264|IVI_GENERR_SYNCNOTFOUND)
// 
// #define IVI_INFO_H264_NOTDEFINEDNALUTYPE	(IVI_INFO_CODEC_H264|0x01)
// 
// #define IVI_INFO_H264_STREAMERROR_LEVEL_3	(IVI_INFO_CODEC_H264|0x10)	
// #define IVI_INFO_H264_STREAMERROR_LEVEL_2	(IVI_INFO_CODEC_H264|0x11)	
// #define IVI_INFO_H264_STREAMERROR_LEVEL_1	(IVI_INFO_CODEC_H264|0x12)	
// #define IVI_INFO_H264_STREAMERROR_LEVEL_0	(IVI_INFO_CODEC_H264|0x13)	
// 
// #define IVI_OK_H264_FRAMECOMPLETE			(IVI_OK_CODEC_H264|0x0)
// #define IVI_OK_H264_SPSCOMPLETE				(IVI_OK_CODEC_H264|0x02)
// #define IVI_OK_H264_PPSCOMPLETE				(IVI_OK_CODEC_H264|0x03)
// #define IVI_OK_H264_SEICOMPLETE				(IVI_OK_CODEC_H264|0x04)

/************************************************************************ 
 * AMR BSAC Decoder specific Error, Added by jerry 
 ************************************************************************/
#define IVI_ERROR_AMR_OPEN				(IVI_ERRPRE_CODEC_AMR|0x01)
#define IVI_ERROR_AMR_CLOSE				(IVI_ERRPRE_CODEC_AMR|0x02)
#define IVI_ERROR_AMR_ENCODE			(IVI_ERRPRE_CODEC_AMR|0x03)
#define IVI_ERROR_AMR_DECODE			(IVI_ERRPRE_CODEC_AMR|0x04)

#define IVI_ERROR_AMR_DEC_INIT			(IVI_ERRPRE_CODEC_AMR|0x05)
#define IVI_ERROR_AMR_DEC_BUFF			(IVI_ERRPRE_CODEC_AMR|0x06)
#define IVI_ERROR_AMR_DEC_NODATA		(IVI_ERRPRE_CODEC_AMR|0x07)
#define IVI_ERROR_AMR_DEC_UNPACK		(IVI_ERRPRE_CODEC_AMR|0x08)
#define IVI_ERROR_AMR_DEC_FAIL			(IVI_ERRPRE_CODEC_AMR|0x09)

#define IVI_ERROR_AMR_ENC_INIT			(IVI_ERRPRE_CODEC_AMR|0x0A)
#define IVI_ERROR_AMR_ENC_BUFF			(IVI_ERRPRE_CODEC_AMR|0x0B)
#define IVI_ERROR_AMR_ENC_NODATA		(IVI_ERRPRE_CODEC_AMR|0x0C)
#define IVI_ERROR_AMR_ENC_READ			(IVI_ERRPRE_CODEC_AMR|0x0D)
#define IVI_ERROR_AMR_ENC_FAIL			(IVI_ERRPRE_CODEC_AMR|0x0E)

/************************************************************************ 
 * BSAC Dec specific Error, Added by jerry 
 ************************************************************************/
#define IVI_ERROR_BSAC_OPEN				(IVI_ERRPRE_CODEC_BSAC|0x01)
#define IVI_ERROR_BSAC_CLOSE			(IVI_ERRPRE_CODEC_BSAC|0x02)
#define IVI_ERROR_BSAC_DECODE			(IVI_ERRPRE_CODEC_BSAC|0x03)

#define IVI_ERROR_BSAC_DEC_INIT			(IVI_ERRPRE_CODEC_BSAC|0x04)
#define IVI_ERROR_BSAC_DEC_BUFF			(IVI_ERRPRE_CODEC_BSAC|0x05)
#define IVI_ERROR_BSAC_DEC_NODATA		(IVI_ERRPRE_CODEC_BSAC|0x06)
#define IVI_ERROR_BSAC_DEC_UNPACK		(IVI_ERRPRE_CODEC_BSAC|0x07)
#define IVI_ERROR_BSAC_DEC_FAIL			(IVI_ERRPRE_CODEC_BSAC|0x08)

/************************************************************************ 
 * AMR-WB Dec specific Error, Added by jerry 
 ************************************************************************/
#define IVI_ERROR_AMRWB_OPEN			(IVI_ERRPRE_CODEC_AMRWB|0x01)
#define IVI_ERROR_AMRWB_CLOSE			(IVI_ERRPRE_CODEC_AMRWB|0x02)
#define IVI_ERROR_AMRWB_DECODE			(IVI_ERRPRE_CODEC_AMRWB|0x03)

#define IVI_ERROR_AMRWB_DEC_INIT		(IVI_ERRPRE_CODEC_AMRWB|0x04)
#define IVI_ERROR_AMRWB_DEC_BUFF		(IVI_ERRPRE_CODEC_AMRWB|0x05)
#define IVI_ERROR_AMRWB_DEC_NODATA		(IVI_ERRPRE_CODEC_AMRWB|0x06)
#define IVI_ERROR_AMRWB_DEC_UNPACK		(IVI_ERRPRE_CODEC_AMRWB|0x07)
#define IVI_ERROR_AMRWB_DEC_FAIL		(IVI_ERRPRE_CODEC_AMRWB|0x08)

/************************************************************************ 
 * AAC Codec specific Error 
 ************************************************************************/
#define IVI_ERROR_AAC_DEC_TNS			(IVI_ERRPRE_CODEC_AAC|0x01)
#define IVI_ERROR_AAC_DEC_SYNC			(IVI_ERRPRE_CODEC_AAC|0x02)
#define IVI_ERROR_AAC_DEC_FREQ			(IVI_ERRPRE_CODEC_AAC|0x03)
#define IVI_ERROR_AAC_DEC_MISALIGNMENT	(IVI_ERRPRE_CODEC_AAC|0x04)
#define IVI_ERROR_AAC_DEC_BS_END		(IVI_ERRPRE_CODEC_AAC|0x05)
#define IVI_ERROR_AAC_DEC_UNKNOWN		(IVI_ERRPRE_CODEC_AAC|0x06)

#endif // __IVI_RETURN_H_

