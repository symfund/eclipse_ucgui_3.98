/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : LCDRLE4.c
Purpose     : Drawing routines for run length encoded bitmaps
              with 4bpp
---------------------------END-OF-HEADER------------------------------
*/

//#include <stddef.h>           /* needed for definition of NULL */
#include "LCD.h"
#include "GUI_Private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawBitmap_RLE4
*/
static void _DrawBitmap_RLE4(I32 x0,I32 y0,I32 xsize, I32 ysize, const U8 GUI_UNI_PTR * pPixel, const LCD_LOGPALETTE GUI_UNI_PTR* pLogPal, I32 xMag, I32 yMag)
{
	const LCD_PIXELINDEX* pTrans =NULL;
	char NoTrans = !(GUI_Context.DrawMode & LCD_DRAWMODE_TRANS);
	LCD_PIXELINDEX aColorIndex[2];
	LCD_PIXELINDEX PixelIndex;
	LCD_PIXELINDEX TransColorIndex;
	LCD_PIXELINDEX DrawColorIndex;
	I32 xi,y;
	I32 xL, yL;
	I32 DrawX0, DrawX1;
	U8 BitmapHasTrans;
	char IsMagnified = ((yMag | xMag) != 1);
	aColorIndex[0] = LCD_ACOLORINDEX[0];
	aColorIndex[1] = LCD_ACOLORINDEX[1];
	BitmapHasTrans = GUI_Context.BitmapHasTrans;
	TransColorIndex = GUI_Color2Index(GUI_Context.BitmapTransColor);
	/* Handle color translation */
	if ((pLogPal) && (pLogPal->pPalEntries)) {
		if ((pTrans = LCD_GetpPalConvTable(pLogPal)) == NULL) {
			return;
		}
	}
	/* Check if we can limit the number of lines due to clipping) */
	if (yMag == 1) {
		if (ysize > GUI_Context.ClipRect.y1 - y0 + 1){
			ysize = GUI_Context.ClipRect.y1 - y0 + 1;
		}
	}
	/* Repeat until we have reached bottom */
	for (xi=0, y = 0; y < ysize; ) {
		U8 Cmd,Data;
		Cmd= *pPixel++;
		Data = *pPixel++;
		if (Cmd) {
			DrawColorIndex = pTrans ? *(pTrans+Data) : Data;
			LCD_SetColorIndex(DrawColorIndex);
			while (Cmd) {
				I32 xi1 = xi+Cmd;
				if (xi1>=xsize){
					xi1 = xsize;
				}
				Cmd -= (xi1-xi);
				if(1 == BitmapHasTrans){
					if(TransColorIndex != DrawColorIndex){
						goto DRAW1;
					}
				}else{
DRAW1:
					if (Data || NoTrans) {  /* Skip transparent pixels */
						if (IsMagnified) {
							xL = xMag * xi + x0;
							yL = yMag * y + y0;
							LCD_FillRect(xL, yL, xL + xMag * (xi1 - xi) -1 , yL + yMag - 1);
						} else {
							if(((y + y0) >= GUI_Context.ClipRect.y0) && ((y + y0) <= GUI_Context.ClipRect.y1)){
								if((x0 + xi) > GUI_Context.ClipRect.x0){
									DrawX0 = x0 + xi;
								}else{
									DrawX0 = GUI_Context.ClipRect.x0;
								}
								if((xi1 + x0 - 1) > GUI_Context.ClipRect.x1){
									DrawX1 = GUI_Context.ClipRect.x1;
								}else{
									DrawX1 = xi1 + x0 - 1;
								}
								LCD_DrawHLine(DrawX0, y + y0, DrawX1);
							}
						}
					}
					xi =xi1;
					if (xi1==xsize) {
						y++;
						xi=0;
					}
				}
			}
		} else {
			while (Data--) {
				U8 Index = *pPixel++;
				if(1 == BitmapHasTrans){
					if(TransColorIndex != DrawColorIndex){
						goto DRAW2;
					}
				}else {
DRAW2:
					if ((Index>>4) || NoTrans) {  /* Skip transparent pixels */
						PixelIndex = pTrans ? *(pTrans+(Index>>4)) : (Index>>4);
						if (IsMagnified) {
							LCD_SetColorIndex(PixelIndex);
							xL = xMag * xi + x0;
							yL = yMag * y + y0;
							LCD_FillRect(xL, yL, xL + xMag -1 , yL + yMag - 1);
						} else {
							LCD_SetPixelIndex(x0+xi, y + y0, PixelIndex, 0xff);
						}
					}
				}
				if (++xi >= xsize) {
					xi=0; y++;
				}
				if (Data-- == 0){
					break;
				}
				if ((Index&15) || NoTrans) {  /* Skip transparent pixels */
					PixelIndex = pTrans ? *(pTrans+(Index&15)) : (Index&15);
					if (IsMagnified) {
						LCD_SetColorIndex(PixelIndex);
						xL = xMag * xi + x0;
						yL = yMag * y + y0;
						LCD_FillRect(xL, yL, xL + xMag -1 , yL + yMag - 1);
					} else {
						LCD_SetPixelIndex(x0+xi, y + y0, PixelIndex, 0xff);
					}
				}
				if (++xi >= xsize) {
					xi=0; y++;
				}
			}
		}
	}
	LCD_ACOLORINDEX[0] = aColorIndex[0];
	LCD_ACOLORINDEX[1] = aColorIndex[1];
}

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BitmapMethodsRLE4
*/
const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE4 = {
  _DrawBitmap_RLE4,
  NULL
};

/*************************** End of file ****************************/
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
