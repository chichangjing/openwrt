
#ifndef _BITMAP_H_
#define _BITMAP_H_

typedef struct tagBITMAP
{
  unsigned long part0;     /* Least Significant part */
} BITMAP_T;

#define BitmapClear(BitmapPtr)                      { (BitmapPtr)->part0 = 0; }
#define BitmapSetAllBits(BitmapPtr)                 { (BitmapPtr)->part0 = 0xF; }
#define BitmapClearBits(BitmapPtr,BitmapBitsPtr)    { (BitmapPtr)->part0 &= ~((BitmapBitsPtr)->part0); }
#define BitmapSetBits(BitmapPtr,BitmapBitsPtr)      { (BitmapPtr)->part0 |= ((BitmapBitsPtr)->part0); }
#define BitmapOr(ResultPtr,BitmapPtr1,BitmapPtr2)   { (ResultPtr)->part0 = (BitmapPtr1)->part0 | (BitmapPtr2)->part0; }
#define BitmapAnd(ResultPtr,BitmapPtr1,BitmapPtr2)  { (ResultPtr)->part0 = (BitmapPtr1)->part0 & (BitmapPtr2)->part0; }
#define BitmapNot(ResultPtr,BitmapPtr)              { (ResultPtr)->part0 = ~((BitmapPtr)->part0); }

/* Return zero if identical */
#define BitmapCmp(BitmapPtr1,BitmapPtr2)            ((BitmapPtr1)->part0 != (BitmapPtr2)->part0) 
#define BitmapIsZero(BitmapPtr)                     (!(BitmapPtr)->part0)
#define BitmapIsAllOnes(BitmapPtr)                  ((BitmapPtr)->part0 == 0xF)
#define BitmapGetBit(BitmapPtr,Bit)                 ((BitmapPtr)->part0 & (1 << (Bit)))

/* Bit range [0 .. 63] */
#define BitmapSetBit(BitmapPtr,Bit)                 {(BitmapPtr)->part0 |= (1 << (Bit)); }
#define BitmapClearBit(BitmapPtr,Bit)               (BitmapPtr)->part0 &= ~(1 << (Bit));
#define BitmapCopy(BitmapDstPtr,BitmapSrcPtr)       { (BitmapDstPtr)->part0 = (BitmapSrcPtr)->part0; }
#define BitmapXor(ResultPtr,BitmapPtr1,BitmapPtr2)  { (ResultPtr)->part0 = (BitmapPtr1)->part0 ^ (BitmapPtr2)->part0; }

#endif /* _BITMAP_H_ */



