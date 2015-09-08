/*
 * bitstream.h
 */
#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include "IviTypedef.h"

//#define OLD_BITSTREAM

extern const ivi32u msk[33];

#if 1	
/////////////////////////////////////////////////////////////////////////////////
//JJ.Dai: New definitions fot bitstream operation 06/08

typedef struct _Ci_New_BitStream {
	ivi8u *     buf_start;
	ivi8u *     buf_curr;
	ivi32s      buf_len;
	ivi32s	bitn;
	ivi32u 	code, code_next;
} CiNewBitStream;

/* Advance (skip) several bits from the data register */
#define _CI_New_SYNCBITS(pBitStream) {										\
	if(pBitStream->bitn<0) {												\
		pBitStream->code       = pBitStream->code_next<<(-pBitStream->bitn);		\
		pBitStream->code_next  = (((ivi32u) *(pBitStream->buf_curr++))<<24);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<<16);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 8);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 0);	\
		pBitStream->bitn += 32;											\
	}																	\
	pBitStream->code |= pBitStream->code_next>>pBitStream->bitn;					\
}


/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_New_SHOWBITS(pBitStream, num, ret_val) {					\
	ret_val = pBitStream->code >> (32-(num));								\
}

/* Advance (skip) several bits from the data register */
#define _CI_New_ADVANCEBITS(pBitStream, num) {								\
	pBitStream->code <<=(num);												\
	pBitStream->bitn -= (num);												\
	_CI_New_SYNCBITS(pBitStream);			\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 32 */
#define _CI_New_GETNBIT(pBitStream, num, ret_val){					\
	_CI_New_SHOWBITS(pBitStream, num, ret_val);						\
	_CI_New_ADVANCEBITS(pBitStream, num);										\
}

/* Parsing one bit from the data register */
#define _CI_New_GET1BIT(pBitStream, ret_val) _CI_New_GETNBIT(pBitStream, 1, ret_val)

#define _CI_New_ADVANCE32BITS(pBitStream) {									\
	pBitStream->code       = pBitStream->code_next;							\
	pBitStream->code_next  = (((ivi32u) *(pBitStream->buf_curr++))<<24);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<<16);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 8);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 0);		\
	if(pBitStream->bitn>0)													\
		pBitStream->code = (pBitStream->code<<(32-pBitStream->bitn)) |			\
						(pBitStream->code_next>>pBitStream->bitn);			\
	else																\
		pBitStream->code = pBitStream->code_next;								\
}

#define _CI_New_START_BITSTREAM(pBitStream, StreamBuffer, BufLen) {			\
	pBitStream->buf_start = (ivi8u*)(StreamBuffer);								\
	pBitStream->buf_len   = (ivi32u)(BufLen);										\
	pBitStream->buf_curr  =  ((ivi8u*)((ivi32u)(StreamBuffer)&0xFFFFFFFC));\
	pBitStream->bitn      = ((ivi8u*)(((ivi32u)(StreamBuffer)&3)<<3));				\
	pBitStream->code_next  = (((ivi32u) *(pBitStream->buf_curr++))<<24);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<<16);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 8);		\
	pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 0);		\
	pBitStream->code       = pBitStream->code_next<<pBitStream->bitn;			\
	if (pBitStream->bitn)	{		\
		pBitStream->bitn = 32-pBitStream->bitn;								\
		pBitStream->code_next  = (((ivi32u) *(pBitStream->buf_curr++))<<24);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<<16);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 8);	\
		pBitStream->code_next += (((ivi32u) *(pBitStream->buf_curr++))<< 0);	\
		pBitStream->code |= pBitStream->code_next>>pBitStream->bitn;			\
	}		\
}

#define _CI_New_STOP_BITSTREAM(pBitStream, StreamBuffer) {						\
	_CI_New_SYNCBITS(pBitStream);														\
	*(StreamBuffer) = pBitStream->buf_curr-4-(pBitStream->bitn>>3);			\
}

#define _CI_New_EOS(pBitStream) (pBitStream->buf_curr >= (pBitStream->buf_start + pBitStream->buf_len + 4))

#define _CI_New_BYTE_ALIGN(pBitStream)	{										\
	_CI_New_SYNCBITS(pBitStream);														\
	pBitStream->code = pBitStream->code<<(pBitStream->bitn&0x7);					\
	pBitStream->bitn &= 0xFFFFFFF8;										\
	pBitStream->code |= pBitStream->code_next>>pBitStream->bitn;					\
}

/////////////////////////////////////////////////////////////////////////////////
// IVI bitstream operation definitions
typedef struct _IviBitstream {
	ivi8u *     pBsBuffer;
	ivi32s      bsByteLen;
	ivi8u *     pBsCurByte;
	ivi32s      bsCurBitOffset;
} IviBitstream;

/* Old definitions */
/* Load unaligned bits to local variable*/
#define _IVI_BSTRBEGIN(dataReg, bitsLeft, pBitStream, bitOffset) {		\
	if (bitOffset) {													\
	(bitsLeft) = 8 - (bitOffset);									\
	(dataReg)  = (ivi32u)(*pBitStream) & msk[bitsLeft];				\
	++pBitStream;													\
	} else {															\
	(bitsLeft) = 0;													\
	(dataReg)  = 0;													\
	}																	\
}

/* Save bits from local variable to bitstream pointers */
#define _IVI_BSTREND(pBitStream, bitOffset, bitsLeft) {					\
	(bitOffset)   = 7 - ((bitsLeft - 1) & 0x7);							\
	(pBitStream) -= (bitsLeft + 7) >> 3;								\
}

/* Parsing several bits from data register but NOT modify bitsLeft */
#define _IVI_SHOWBITS(dataReg, bitsLeft, num, code){					\
	(code)	= ((((ivi32u)dataReg) << ( 32-(bitsLeft) ))>>( 32 - (num)));\
}

#define _IVI_SHOW12BITS(dataReg, bitsLeft, code){						\
	(code)	= ((((ivi32u)(dataReg)) << (32 - (bitsLeft))) >> 20) ;		\
}

#define _IVI_SHOW9BITS(dataReg, bitsLeft, code){						\
	(code)	= ((((ivi32u)(dataReg)) << (32 - (bitsLeft))) >> 23) ;		\
}

/* Parsing one bit from the data register */
#define _IVI_GET1BIT(dataReg, bitsLeft, code) {							\
	(bitsLeft) -= 1;													\
	(code)	= ((dataReg) >> (bitsLeft)) & 0x1;							\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 16 */
#define _IVI_GETNBIT(dataReg, bitsLeft, num, code) {					\
	(bitsLeft) -= (num);												\
	(code)	= ((dataReg) >> (bitsLeft)) & msk[num];						\
}

/* Previews 8 bits from the bitstream */
#define _IVI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));				\
	(bitsLeft) += 8;													\
	(pBitStream) += 1;													\
}

/* Previews 16 bits from the bitstream */
#define _IVI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 16) | ((ivi32u)(*(pBitStream)) << 8);		\
	(dataReg) |= (ivi32u)(*((pBitStream)+1));							\
	(bitsLeft) += 16;													\
	(pBitStream) += 2;													\
}

/* Previews 24 bits from the bitstream */
#define _IVI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));				\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream + 1));			\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream + 2));			\
	(bitsLeft) += 24;													\
	(pBitStream) += 3;													\
}

/////////////////////////////////////////////////////////////////////////////////
// CI bitstream operation definitions
/* New definitions */

/* Load unaligned bits to local variable*/
#define _CI_BSTRBEGIN(dataReg, bitsLeft, pBitStream, bitOffset) {		\
	if (bitOffset) {													\
		(bitsLeft) = 8 - (bitOffset);									\
		(dataReg)  = (ivi32u)(*pBitStream) & msk[bitsLeft];				\
		++pBitStream;													\
	} else {															\
		(bitsLeft) = 0;													\
		(dataReg)  = 0;													\
	}																	\
}

/* Save bits from local variable to bitstream pointers */
#define _CI_BSTREND(pBitStream, bitOffset, bitsLeft) {					\
	(bitOffset)   = 7 - ((bitsLeft - 1) & 0x7);							\
	(pBitStream) -= (bitsLeft + 7) >> 3;								\
}
/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_SHOWBITS(dataReg, bitsLeft, num, code){					    \
	(code)	= ((((ivi32u)(dataReg)) << (32-(bitsLeft)))>>(32 - (num))); \
}

#define _CI_SHOW12BITS(dataReg, bitsLeft, code){						\
	(code)	= ((((ivi32u)(dataReg)) << (32 - (bitsLeft))) >> 20) ;		\
}

#define _CI_SHOW9BITS(dataReg, bitsLeft, code){						\
	(code)	= ((((ivi32u)(dataReg)) << (32 - (bitsLeft))) >> 23) ;		\
}

/* Parsing one bit from the data register */
#define _CI_GET1BIT(dataReg, bitsLeft, code) {							\
	(bitsLeft) -= 1;													\
	(code)	= (((dataReg) >> (bitsLeft)) & 0x1);							\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 16 */
#define _CI_GETNBIT(dataReg, bitsLeft, num, code) {					\
	(bitsLeft) -= (num);												\
	(code)	= (((dataReg) >> (bitsLeft)) & (msk[num]));					\
}

/* Advance (skip) several bits from the data register */
#define _CI_ADVANCEBITS(bitsLeft, num) {								\
	(bitsLeft) -= (num);												\
}

/* Previews 8 bits from the bitstream */
#define _CI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));				\
	(bitsLeft) += 8;													\
	(pBitStream) += 1;													\
}

/* Previews 16 bits from the bitstream */
#define _CI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 16) | ((ivi32u)(*(pBitStream)) << 8);		\
	(dataReg) |= (ivi32u)(*((pBitStream)+1));							\
	(bitsLeft) += 16;													\
	(pBitStream) += 2;													\
}

/* Previews 24 bits from the bitstream */
#define _CI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));				\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 1));			\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 2));			\
	(bitsLeft) += 24;													\
	(pBitStream) += 3;													\
}

/* Previews 32 bits from the bitstream */
#define _CI_PREVIEW32BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	(dataReg) = (ivi32u)(*(pBitStream));								\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 1));			\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 2));			\
	(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 3));			\
	(bitsLeft) += 32;													\
	(pBitStream) += 4;													\
}

/* Guarantees at least 8 bits from the bitstream */
#define _CI_GUARANTEE8BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if((bitsLeft)<8) {													\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));			\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 1));		\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 2));		\
		(bitsLeft) += 24;												\
		(pBitStream) += 3;												\
	}																	\
}

/* Guarantees at least 9 bits from the bitstream */
#define _CI_GUARANTEE9BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if((bitsLeft)<=8) {													\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));			\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 1));		\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*((pBitStream) + 2));		\
		(bitsLeft) += 24;												\
		(pBitStream) += 3;												\
	}																	\
}

/* Guarantees at least 16 bits from the bitstream */
#define _CI_GUARANTEE16BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if((bitsLeft)<16) {													\
		(dataReg) = ((dataReg) << 16) | ((ivi32u)(*(pBitStream)) << 8);	\
		(dataReg) |= (ivi32u)(*((pBitStream)+1));						\
		(bitsLeft) += 16;												\
		(pBitStream) += 2;												\
	}																	\
}

/* Guarantees at least 17 bits from the bitstream */
#define _CI_GUARANTEE17BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if((bitsLeft)<=16) {												\
		(dataReg) = ((dataReg) << 16) | ((ivi32u)(*(pBitStream)) << 8);	\
		(dataReg) |= (ivi32u)(*((pBitStream)+1));						\
		(bitsLeft) += 16;												\
		(pBitStream) += 2;												\
	}																	\
}

/* Guarantees at least 24 bits from the bitstream */
#define _CI_GUARANTEE24BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	while((bitsLeft)<24) {												\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));			\
		(bitsLeft) += 8;												\
		(pBitStream) += 1;												\
	}																	\
}

/* Guarantees at least 25 bits from the bitstream */
#define _CI_GUARANTEE25BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	while((bitsLeft)<=24) {												\
		(dataReg) = ((dataReg) << 8) | (ivi32u)(*(pBitStream));			\
		(bitsLeft) += 8;												\
		(pBitStream) += 1;												\
	}																	\
}

#define _CI_LOAD_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)		\
	unsigned char *pBsCurByte = pMp2Img->buf_curr;						\
	unsigned int  dataReg     = pMp2Img->code;							\
	unsigned int  bitsLeft    = pMp2Img->bitn;							\

#define _CI_SAVE_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft) {	\
	pMp2Img->buf_curr = pBsCurByte;										\
	pMp2Img->code     = dataReg;										\
	pMp2Img->bitn     = bitsLeft;										\
}

#define _CI_SHOW32BITS(dataReg, ret_val) (ret_val) = (dataReg);

#define _CI_ADVANCE32BITS(bitsLeft) (bitsLeft) -= 32;

#define _CI_START_BITSTREAM(pMp2Img, StreamBuffer, Buflen) {			\
	pMp2Img->buf_start = pMp2Img->buf_curr  = *(StreamBuffer);			\
	pMp2Img->buf_len   = BufLen;										\
}

#define _CI_STOP_BITSTREAM(pMp2Img, StreamBuffer) {					\
	*(StreamBuffer) = pMp2Img->buf_curr - ((pMp2Img->bitn)>>3);			\
	pMp2Img->bitn = 0;													\
}

#define _CI_EOS(pBsCurByte, StreamBuffer, Buflen) ((pBsCurByte) >= ((StreamBuffer) + (Buflen)))

#define _CI_BYTE_ALIGN(pMp2Img) pMp2Img->bitn &= 0xFFFFFFF8;

#elif 0

/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_SHOWBITS(dataReg, bitsLeft, num, ret_val){					\
	ret_val = (pMp2Img->code << ( 32-pMp2Img->bitn ))>>( 32 - (num) );	\
}

/* Parsing one bit from the data register */
#define _CI_GET1BIT(dataReg, bitsLeft, ret_val) {						\
	pMp2Img->bitn--;													\
	ret_val = (pMp2Img->code >> pMp2Img->bitn) & 0x1;					\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 16 */
#define _CI_GETNBIT(dataReg, bitsLeft, num, ret_val) {					\
	pMp2Img->bitn -= (num);												\
	ret_val = (pMp2Img->code >> pMp2Img->bitn) & msk[num];				\
}

/* Advance (skip) several bits from the data register */
#define _CI_ADVANCEBITS(bitsLeft, num) {								\
	pMp2Img->bitn -= (num);												\
}

/* Previews 8 bits from the bitstream */
#define _CI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	pMp2Img->code = (pMp2Img->code << 8) |								\
					(ivi32u)(*pMp2Img->buf_curr);						\
	pMp2Img->bitn += 8;													\
	pMp2Img->buf_curr += 1;												\
}

/* Previews 16 bits from the bitstream */
#define _CI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	pMp2Img->code = (pMp2Img->code << 16) |								\
					((ivi32u)(*(pMp2Img->buf_curr+0)) << 8) |			\
					((ivi32u)(*(pMp2Img->buf_curr+1)));					\
	pMp2Img->bitn += 16;												\
	pMp2Img->buf_curr += 2;												\
}

/* Previews 24 bits from the bitstream */
#define _CI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	pMp2Img->code = (pMp2Img->code << 24) |								\
					((ivi32u)(*(pMp2Img->buf_curr+0)) << 16) |			\
					((ivi32u)(*(pMp2Img->buf_curr+1)) << 8) |			\
					((ivi32u)(*(pMp2Img->buf_curr+2)));					\
	pMp2Img->bitn += 24;												\
	pMp2Img->buf_curr += 3;												\
}

/* Previews 32 bits from the bitstream */
#define _CI_PREVIEW32BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	pMp2Img->code = ((ivi32u)(*(pMp2Img->buf_curr+0)) << 24) |			\
					((ivi32u)(*(pMp2Img->buf_curr+1)) << 16) |			\
					((ivi32u)(*(pMp2Img->buf_curr+2)) << 8) |			\
					((ivi32u)(*(pMp2Img->buf_curr+3)));					\
	pMp2Img->bitn += 32;												\
	pMp2Img->buf_curr += 4;												\
}

/* Guarantees at least 8 bits from the bitstream */
#define _CI_GUARANTEE8BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if(pMp2Img->bitn<8) {												\
		pMp2Img->code = (pMp2Img->code << 24) |							\
						((ivi32u)(*(pMp2Img->buf_curr+0)) << 16) |		\
						((ivi32u)(*(pMp2Img->buf_curr+1)) << 8) |		\
						((ivi32u)(*(pMp2Img->buf_curr+2)));				\
		pMp2Img->bitn += 24;											\
		pMp2Img->buf_curr += 3;											\
	}																	\
}

/* Guarantees at least 9 bits from the bitstream */
#define _CI_GUARANTEE9BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if(pMp2Img->bitn<=8) {												\
		pMp2Img->code = (pMp2Img->code << 24) |							\
						((ivi32u)(*(pMp2Img->buf_curr+0)) << 16) |		\
						((ivi32u)(*(pMp2Img->buf_curr+1)) << 8) |		\
						((ivi32u)(*(pMp2Img->buf_curr+2)));				\
		pMp2Img->bitn += 24;											\
		pMp2Img->buf_curr += 3;											\
	}																	\
}

/* Guarantees at least 16 bits from the bitstream */
#define _CI_GUARANTEE16BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if(pMp2Img->bitn<16) {												\
		pMp2Img->code = (pMp2Img->code << 16) |							\
						((ivi32u)(*(pMp2Img->buf_curr+0)) << 8) |		\
						((ivi32u)(*(pMp2Img->buf_curr+1)));				\
		pMp2Img->bitn += 16;											\
		pMp2Img->buf_curr += 2;											\
	}																	\
}

/* Guarantees at least 17 bits from the bitstream */
#define _CI_GUARANTEE17BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	if(pMp2Img->bitn<=16) {												\
		pMp2Img->code = (pMp2Img->code << 16) |							\
						((ivi32u)(*(pMp2Img->buf_curr+0)) << 8) |		\
						((ivi32u)(*(pMp2Img->buf_curr+1)));				\
		pMp2Img->bitn += 16;											\
		pMp2Img->buf_curr += 2;											\
	}																	\
}

/* Guarantees at least 24 bits from the bitstream */
#define _CI_GUARANTEE24BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	while(pMp2Img->bitn<24) {											\
		pMp2Img->code = (pMp2Img->code << 8) |							\
						(ivi32u)(*pMp2Img->buf_curr);					\
		pMp2Img->bitn += 8;												\
		pMp2Img->buf_curr += 1;											\
	}																	\
}

/* Guarantees at least 25 bits from the bitstream */
#define _CI_GUARANTEE25BITS(pBitStream, dataReg, bitsLeft)				\
{																		\
	while(pMp2Img->bitn<=24) {											\
		pMp2Img->code = (pMp2Img->code << 8) |							\
						(ivi32u)(*pMp2Img->buf_curr);					\
		pMp2Img->bitn += 8;												\
		pMp2Img->buf_curr += 1;											\
	}																	\
}

#define _CI_LOAD_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SAVE_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SHOW32BITS(dataReg, ret_val) ret_val = pMp2Img->code;

#define _CI_ADVANCE32BITS(bitsLeft) pMp2Img->bitn -= 32;

#define _CI_START_BITSTREAM(pMp2Img, StreamBuffer, Buflen) {			\
	pMp2Img->buf_start = pMp2Img->buf_curr  = *(StreamBuffer);			\
	pMp2Img->buf_len   = BufLen;										\
}

#define _CI_STOP_BITSTREAM(pMp2Img, StreamBuffer) {						\
	*(StreamBuffer) = pMp2Img->buf_curr;								\
	*(StreamBuffer) = pMp2Img->buf_curr - (pMp2Img->bitn>>3);			\
	pMp2Img->bitn = 0;													\
}

#define _CI_EOS(pBsCurByte, StreamBuffer, Buflen) (pMp2Img->buf_curr >= (pMp2Img->buf_start + pMp2Img->buf_len))

#define _CI_BYTE_ALIGN(pMp2Img) pMp2Img->bitn &= 0xFFFFFFF8;

#elif 0

/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_SHOWBITS(dataReg, bitsLeft, num, ret_val){					\
	int l_bitcount;														\
	l_bitcount = pMp2Img->bitn - (num);									\
	if(l_bitcount<0) {													\
		ret_val = (pMp2Img->code << (-l_bitcount)) |					\
				  (pMp2Img->code_next >> (32+l_bitcount));				\
	}																	\
	else																\
	{																	\
		ret_val = pMp2Img->code >> l_bitcount;							\
	}																	\
	ret_val &= msk[num];												\
}

/* Parsing one bit from the data register */
#define _CI_GET1BIT(dataReg, bitsLeft, ret_val){						\
	pMp2Img->bitn--;													\
	if(pMp2Img->bitn<0) {												\
		ret_val = (pMp2Img->code_next >> 31);							\
		pMp2Img->code       = pMp2Img->code_next;						\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
	}																	\
	else																\
	{																	\
		ret_val = (pMp2Img->code >> pMp2Img->bitn)&1;					\
	}																	\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 16 */
#define _CI_GETNBIT(dataReg, bitsLeft, num, ret_val){					\
	pMp2Img->bitn -= (num);												\
	if(pMp2Img->bitn<0) {												\
		ret_val = (pMp2Img->code << (-pMp2Img->bitn)) |					\
				  (pMp2Img->code_next >> (32+pMp2Img->bitn));			\
		pMp2Img->code       = pMp2Img->code_next;						\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
	}																	\
	else																\
	{																	\
		ret_val = pMp2Img->code >> pMp2Img->bitn;						\
	}																	\
	ret_val &= msk[num];												\
}

/* Advance (skip) several bits from the data register */
#define _CI_ADVANCEBITS(bitsLeft, num) {								\
	pMp2Img->bitn -= (num);												\
	if(pMp2Img->bitn<0) {												\
		pMp2Img->code       = pMp2Img->code_next;						\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
	}																	\
}

/* Previews 8 bits from the bitstream */
#define _CI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)				\

/* Previews 16 bits from the bitstream */
#define _CI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft)				\

/* Previews 24 bits from the bitstream */
#define _CI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft)				\

/* Previews 32 bits from the bitstream */
#define _CI_PREVIEW32BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 8 bits from the bitstream */
#define _CI_GUARANTEE8BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 9 bits from the bitstream */
#define _CI_GUARANTEE9BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 16 bits from the bitstream */
#define _CI_GUARANTEE16BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 17 bits from the bitstream */
#define _CI_GUARANTEE17BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 24 bits from the bitstream */
#define _CI_GUARANTEE24BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 25 bits from the bitstream */
#define _CI_GUARANTEE25BITS(pBitStream, dataReg, bitsLeft)				\

#define _CI_LOAD_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)		\
	unsigned char *pBsCurByte = 0;										\
	unsigned int  dataReg = 0;											\
	unsigned int  bitsLeft = 0;											\

#define _CI_SAVE_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SHOW32BITS(dataReg, ret_val) {								\
	if(pMp2Img->bitn==0)												\
		ret_val = pMp2Img->code_next;									\
	else																\
	{																	\
		ret_val = (pMp2Img->code << (32-pMp2Img->bitn)) |				\
				  (pMp2Img->code_next >> pMp2Img->bitn);				\
	}																	\
}

#define _CI_ADVANCE32BITS(bitsLeft) {									\
	pMp2Img->code       = pMp2Img->code_next;							\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
}

#define _CI_START_BITSTREAM(pMp2Img, StreamBuffer, Buflen) {			\
	pMp2Img->buf_start = *(StreamBuffer);								\
	pMp2Img->buf_len   = BufLen;										\
	pMp2Img->buf_curr  = (unsigned char *) (((int) *(StreamBuffer))&0xFFFFFFFC);\
	pMp2Img->bitn      = ((4-(((int) *(StreamBuffer))&3))&3)<<3;		\
	if(pMp2Img->bitn) {													\
		pMp2Img->code  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
		pMp2Img->code += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
		pMp2Img->code += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
		pMp2Img->code += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
	}																	\
	else {																\
		pMp2Img->code  = 0;												\
	}																	\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
}

#define _CI_STOP_BITSTREAM(pMp2Img, StreamBuffer) {					\
	*(StreamBuffer) = pMp2Img->buf_curr-4-(pMp2Img->bitn>>3);			\
}

#define _CI_EOS(pBsCurByte, StreamBuffer, Buflen) (pMp2Img->buf_curr >= (pMp2Img->buf_start + pMp2Img->buf_len + 4))

#define _CI_BYTE_ALIGN(pMp2Img) pMp2Img->bitn &= 0xFFFFFFF8;

#elif 0

/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_SHOWBITS(dataReg, bitsLeft, num, ret_val) {					\
	ret_val = pMp2Img->code >> (32-(num));								\
}

/* Advance (skip) several bits from the data register */
#define _CI_ADVANCEBITS(bitsLeft, num) {								\
	pMp2Img->bitn -= (num);												\
	if(pMp2Img->bitn<0) {												\
		pMp2Img->code       = pMp2Img->code_next<<(-pMp2Img->bitn);		\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
	}																	\
	else {																\
		pMp2Img->code = pMp2Img->code<<(num);							\
	}																	\
	pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;					\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 32 */
#define _CI_GETNBIT(dataReg, bitsLeft, num, ret_val){					\
	_CI_SHOWBITS(dataReg, bitsLeft, num, ret_val)						\
	_CI_ADVANCEBITS(bitsLeft, num)										\
}

/* Parsing one bit from the data register */
#define _CI_GET1BIT(dataReg, bitsLeft, ret_val) _CI_GETNBIT(dataReg, bitsLeft, 1, ret_val)

/* Previews 8 bits from the bitstream */
#define _CI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)					\

/* Previews 16 bits from the bitstream */
#define _CI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft)				\

/* Previews 24 bits from the bitstream */
#define _CI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft)				\

/* Previews 32 bits from the bitstream */
#define _CI_PREVIEW32BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 8 bits from the bitstream */
#define _CI_GUARANTEE8BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 9 bits from the bitstream */
#define _CI_GUARANTEE9BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 16 bits from the bitstream */
#define _CI_GUARANTEE16BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 17 bits from the bitstream */
#define _CI_GUARANTEE17BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 24 bits from the bitstream */
#define _CI_GUARANTEE24BITS(pBitStream, dataReg, bitsLeft)				\

/* Guarantees at least 25 bits from the bitstream */
#define _CI_GUARANTEE25BITS(pBitStream, dataReg, bitsLeft)				\

#define _CI_LOAD_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)		\
	unsigned char *pBsCurByte = 0;										\
	unsigned int  dataReg = 0;											\
	unsigned int  bitsLeft = 0;											\

#define _CI_SAVE_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SHOW32BITS(dataReg, ret_val) {								\
	ret_val = pMp2Img->code;											\
}

#define _CI_ADVANCE32BITS(bitsLeft) {									\
	pMp2Img->code       = pMp2Img->code_next;							\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
	if(pMp2Img->bitn>0)													\
		pMp2Img->code = (pMp2Img->code<<(32-pMp2Img->bitn)) |			\
						(pMp2Img->code_next>>pMp2Img->bitn);			\
	else																\
		pMp2Img->code = pMp2Img->code_next;								\
}

#define _CI_START_BITSTREAM(pMp2Img, StreamBuffer, Buflen) {			\
	pMp2Img->buf_start = *(StreamBuffer);								\
	pMp2Img->buf_len   = BufLen;										\
	pMp2Img->buf_curr  = (unsigned char *) (((int) *(StreamBuffer))&0xFFFFFFFC);\
	pMp2Img->bitn      = (((int) *(StreamBuffer))&3)<<3;				\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
	pMp2Img->code       = pMp2Img->code_next<<pMp2Img->bitn;			\
	if(pMp2Img->bitn) {													\
		pMp2Img->bitn = 32-pMp2Img->bitn;								\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;				\
	}																	\
}

#define _CI_STOP_BITSTREAM(pMp2Img, StreamBuffer) {						\
	*(StreamBuffer) = pMp2Img->buf_curr-4-(pMp2Img->bitn>>3);			\
}

#define _CI_EOS(pBsCurByte, StreamBuffer, Buflen) (pMp2Img->buf_curr >= (pMp2Img->buf_start + pMp2Img->buf_len + 4))

#define _CI_BYTE_ALIGN(pMp2Img)	{										\
	pMp2Img->code = pMp2Img->code<<(pMp2Img->bitn&0x7);					\
	pMp2Img->bitn &= 0xFFFFFFF8;										\
	pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;					\
}

#else

/* Parsing several bits from data register but NOT modify bitsLeft */
#define _CI_SHOWBITS(dataReg, bitsLeft, num, ret_val) {					\
	ret_val = pMp2Img->code >> (32-(num));								\
}

/* Advance (skip) several bits from the data register */
#define _CI_ADVANCEBITS(bitsLeft, num) {								\
	pMp2Img->code <<=(num);												\
	pMp2Img->bitn -= (num);												\
}

/* Advance (skip) several bits from the data register */
#define _CI_SYNCBITS(bitsLeft) {										\
	if(pMp2Img->bitn<0) {												\
		pMp2Img->code       = pMp2Img->code_next<<(-pMp2Img->bitn);		\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
	}																	\
	pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;					\
}

/* Parsing several bits from the data register the range of bits number is from 1 to 32 */
#define _CI_GETNBIT(dataReg, bitsLeft, num, ret_val){					\
	_CI_SHOWBITS(dataReg, bitsLeft, num, ret_val)						\
	_CI_ADVANCEBITS(bitsLeft, num)										\
}

/* Parsing one bit from the data register */
#define _CI_GET1BIT(dataReg, bitsLeft, ret_val) _CI_GETNBIT(dataReg, bitsLeft, 1, ret_val)

/* Previews 8 bits from the bitstream */
#define _CI_PREVIEW8BITS(pBitStream, dataReg, bitsLeft)	_CI_SYNCBITS(bitsLeft)

/* Previews 16 bits from the bitstream */
#define _CI_PREVIEW16BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Previews 24 bits from the bitstream */
#define _CI_PREVIEW24BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Previews 32 bits from the bitstream */
#define _CI_PREVIEW32BITS(pBitStream, dataReg, bitsLeft) {				\
	if(pMp2Img->bitn==(-32)) {											\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->bitn += 32;											\
		pMp2Img->code = pMp2Img->code_next;								\
	}																	\
	else {																\
		_CI_SYNCBITS(0)													\
	}																	\
}

/* Guarantees at least 8 bits from the bitstream */
#define _CI_GUARANTEE8BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Guarantees at least 9 bits from the bitstream */
#define _CI_GUARANTEE9BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Guarantees at least 16 bits from the bitstream */
#define _CI_GUARANTEE16BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Guarantees at least 17 bits from the bitstream */
#define _CI_GUARANTEE17BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Guarantees at least 24 bits from the bitstream */
#define _CI_GUARANTEE24BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

/* Guarantees at least 25 bits from the bitstream */
#define _CI_GUARANTEE25BITS(pBitStream, dataReg, bitsLeft) _CI_SYNCBITS(bitsLeft)

#define _CI_LOAD_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SAVE_BS_PARAMS(pMp2Img, pBsCurByte, dataReg, bitsLeft)

#define _CI_SHOW32BITS(dataReg, ret_val) {								\
	ret_val = pMp2Img->code;											\
}

#define _CI_ADVANCE32BITS(bitsLeft) {									\
	pMp2Img->code       = pMp2Img->code_next;							\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
	if(pMp2Img->bitn>0)													\
		pMp2Img->code = (pMp2Img->code<<(32-pMp2Img->bitn)) |			\
						(pMp2Img->code_next>>pMp2Img->bitn);			\
	else																\
		pMp2Img->code = pMp2Img->code_next;								\
}

#define _CI_START_BITSTREAM(pMp2Img, StreamBuffer, Buflen) {			\
	pMp2Img->buf_start = *(StreamBuffer);								\
	pMp2Img->buf_len   = BufLen;										\
	pMp2Img->buf_curr  = (unsigned char *) (((int) *(StreamBuffer))&0xFFFFFFFC);\
	pMp2Img->bitn      = (((int) *(StreamBuffer))&3)<<3;				\
	pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);		\
	pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);		\
	pMp2Img->code       = pMp2Img->code_next<<pMp2Img->bitn;			\
	if(pMp2Img->bitn) {													\
		pMp2Img->bitn = 32-pMp2Img->bitn;								\
		pMp2Img->code_next  = (((ivi32u) *(pMp2Img->buf_curr++))<<24);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<<16);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 8);	\
		pMp2Img->code_next += (((ivi32u) *(pMp2Img->buf_curr++))<< 0);	\
		pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;				\
	}																	\
}

#define _CI_STOP_BITSTREAM(pMp2Img, StreamBuffer) {						\
	_CI_SYNCBITS(0)														\
	*(StreamBuffer) = pMp2Img->buf_curr-4-(pMp2Img->bitn>>3);			\
}

#define _CI_EOS(pBsCurByte, StreamBuffer, Buflen) (pMp2Img->buf_curr >= (pMp2Img->buf_start + pMp2Img->buf_len + 4))

#define _CI_BYTE_ALIGN(pMp2Img)	{										\
	_CI_SYNCBITS(0)														\
	pMp2Img->code = pMp2Img->code<<(pMp2Img->bitn&0x7);					\
	pMp2Img->bitn &= 0xFFFFFFF8;										\
	pMp2Img->code |= pMp2Img->code_next>>pMp2Img->bitn;					\
}

#endif


#endif /* _BITSTREAM_H_ */
