#pragma once
#include <Windows.h>
#include <time.h>
#include <stdlib.h>

#define LID_STATE						3
#define BLOCK_YNUM						16
#define BLOCK_XNUM						16
#define MINE_NUM						40
#define DIR_NUM							8
#define BLOCK_LID						0
#define BLOCK_FLAG						1
#define BLOCK_UNCERTAIN					2
#define BLOCK_REDMINE					3
#define BLOCK_WRONGMARK					4
#define BLOCK_MINE						5
#define BLOCK_UNLIDEDQUNCERTAIN			6
#define BLOCK_NUM8						7	
#define BLOCK_NUM7						8
#define BLOCK_NUM6						9
#define BLOCK_NUM5						10
#define BLOCK_NUM4						11
#define BLOCK_NUM3						12
#define BLOCK_NUM2						13
#define BLOCK_NUM1						14
#define BLOCK_NONE						15
#define BMP_NUM							16

#define WI_CXPADDING					8
#define WI_CYPADDING					50
#define UNIT_LEN						16
#define WI_CXCLIENT						289
#define WI_CYCLIENT						368
#define WI_CBOTTOMPADDING				8
#define INFO_WINDOW_Y					38
const dir[][2] = {
	{ -1, 0 },{ -1, -1 },{ 0, -1 },{ 1, -1 },
	{ 1, 0 },{ 1, 1 },{ 0, 1 },{ -1, 1 }
};
#define INBOND(A, B, C) ((B) >= (A)) && ((B) < (C))
#define UNLID(A)		((A) | 0x80)
#define LID(A)			((A) & 0x7f)
#define LIDWORD(X)				(((X) >> 4) & 0x03)
#define UNLIDWORD(X)			((X) & 0x0f)
#define ISLIDDED(X)				(!((X) & 0x80))
#define NUM_DEC(X)				((((X) & 0x0f) + 1) | ((X) & 0xf0))
#define NUM_ASC(X)				((((X) & 0x0f) - 1) | ((X) & 0xf0))