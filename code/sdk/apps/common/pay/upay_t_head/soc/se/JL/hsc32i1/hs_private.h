/*
 * @Description:
 * @Author: HJY
 * @Date: 2022-11-17 15:09:17
 * @LastEditTime: 2022-11-21 19:15:37
 * @LastEditors: HJY
 */
#ifndef	__HS_PRIVATE_H__
#define	__HS_PRIVATE_H__


extern unsigned char g_HSI2CBuf[128];


//CRC单字节计算——循环左移，先移最高bit
extern unsigned short LeftShift_CRC(unsigned char byValue, unsigned short *lpwCrc);
extern void ComputeCRC(unsigned char *Data, unsigned short Length, unsigned char *pbyOut);
extern void lmemset(unsigned char *pBuf, unsigned char byVal, unsigned short wLen);
extern void lmemcpy(unsigned char *pDst, unsigned char *pSrc, unsigned short wLen);
extern unsigned char lmemcmp(unsigned char *pBuf1, unsigned char *pBuf2, unsigned short wLen);


extern void HS_IIC_Init(void);
extern void HS_IIC_Send_Cmd(unsigned char *pData, unsigned short wLen);
extern unsigned char HS_IIC_Recv_Resp(unsigned char *pData, unsigned short wLen);

#endif
