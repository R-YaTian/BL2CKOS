/*---------------------------------------------------------------------------------

	sprite.h -- definitions for DS sprites

	Copyright (C) 2007
		Liran Nuna	(LiraNuna)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
	must not claim that you wrote the original software. If you use
	this software in a product, an acknowledgment in the product
	documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
	distribution.

---------------------------------------------------------------------------------*/
/*! \file sprite.h
    \brief nds sprite functionality.
*/

#ifndef _libnds_sprite_h_
#define _libnds_sprite_h_

#ifndef ARM9
#error Sprites are only available on the ARM9
#endif

#include "nds/ndstypes.h"
#include "nds/arm9/video.h"
#include "nds/memory.h"
#include "nds/system.h"

// Sprite control defines

// Attribute 0 consists of 8 bits of Y plus the following flags:
#define ATTR0_NORMAL			(0<<8)
#define ATTR0_ROTSCALE			(1<<8)
#define ATTR0_DISABLED			(2<<8)
#define ATTR0_ROTSCALE_DOUBLE	(3<<8)

#define ATTR0_TYPE_NORMAL		(0<<10)
#define ATTR0_TYPE_BLENDED		(1<<10)
#define ATTR0_TYPE_WINDOWED		(2<<10)
#define ATTR0_BMP				(3<<10)

#define ATTR0_MOSAIC			(1<<12)

#define ATTR0_COLOR_16		(0<<13) //16 color in tile mode...16 bit in bitmap mode
#define ATTR0_COLOR_256		(1<<13)

#define ATTR0_SQUARE		(0<<14)
#define ATTR0_WIDE			(1<<14)
#define ATTR0_TALL			(2<<14)

#define OBJ_Y(m)			((m)&0x00ff)

// Atribute 1 consists of 9 bits of X plus the following flags:
#define ATTR1_ROTDATA(n)      ((n)<<9)  // note: overlaps with flip flags
#define ATTR1_FLIP_X          (1<<12)
#define ATTR1_FLIP_Y          (1<<13)
#define ATTR1_SIZE_8          (0<<14)
#define ATTR1_SIZE_16         (1<<14)
#define ATTR1_SIZE_32         (2<<14)
#define ATTR1_SIZE_64         (3<<14)

#define OBJ_X(m)			((m)&0x01ff)

// Atribute 2 consists of the following:
#define ATTR2_PRIORITY(n)     ((n)<<10)
#define ATTR2_PALETTE(n)      ((n)<<12)
#define ATTR2_ALPHA(n)		  ((n)<<12)

/** \brief The blending mode of the sprite */
typedef enum
{
	OBJMODE_NORMAL,		/**< No special mode is on - Normal sprite state. */
	OBJMODE_BLENDED,	/**< Color blending is on - Sprite can use HW blending features. */
	OBJMODE_WINDOWED,	/**< Sprite can be seen only inside the sprite window. */
	OBJMODE_BITMAP,		/**< Sprite is not using tiles - per pixel image data. */

} ObjBlendMode;

/** \brief The shape of the sprite */
typedef enum {
	OBJSHAPE_SQUARE,	/**< Sprite shape is NxN (Height == Width). */
	OBJSHAPE_WIDE,		/**< Sprite shape is NxM with N > M (Height < Width). */
	OBJSHAPE_TALL,		/**< Sprite shape is NxM with N < M (Height > Width). */
	OBJSHAPE_FORBIDDEN,	/**< Sprite shape is undefined. */
} ObjShape;

/** \brief The size of the sprite */
typedef enum {
	OBJSIZE_8,		/**< Major sprite size is 8px. */
	OBJSIZE_16,		/**< Major sprite size is 16px. */
	OBJSIZE_32,		/**< Major sprite size is 32px. */
	OBJSIZE_64,		/**< Major sprite size is 64px. */
} ObjSize;

/** \brief The color mode of the sprite */
typedef enum {
	OBJCOLOR_16,		/**< sprite has 16 colors. */
	OBJCOLOR_256,		/**< sprite has 256 colors. */
} ObjColMode;

/** \brief The priority of the sprite */
typedef enum {
	OBJPRIORITY_0,		/**< sprite priority level 0 - highest. */
	OBJPRIORITY_1,		/**< sprite priority level 1. */
	OBJPRIORITY_2,		/**< sprite priority level 2. */
	OBJPRIORITY_3,		/**< sprite priority level 3 - lowest. */
} ObjPriority;

//! A bitfield of sprite attribute goodness...ugly to look at but not so bad to use.
typedef union SpriteEntry
{
	struct
	{
		struct
		{
			u16 y							:8;	/**< Sprite Y position. */
			union
			{
				struct
				{
					u8 						:1;
					bool isHidden 			:1;	/**< Sprite is hidden (isRotoscale cleared). */
					u8						:6;
				};
				struct
				{
					bool isRotateScale		:1;	/**< Sprite uses affine parameters if set. */
					bool isSizeDouble		:1;	/**< Sprite bounds is doubled (isRotoscale set). */
					ObjBlendMode blendMode	:2;	/**< Sprite object mode. */
					bool isMosaic			:1;	/**< Enables mosaic effect if set. */
					ObjColMode colorMode	:1;	/**< Sprite color mode. */
					ObjShape shape			:2;	/**< Sprite shape. */
				};
			};
		};

		union {
			struct {
				u16 x						:9;	/**< Sprite X position. */
				u8 							:7;
			};
			struct {
				u8							:8;
				union {
					struct {
						u8					:4;
						bool hFlip			:1; /**< Flip sprite horizontally (isRotoscale cleared). */
						bool vFlip			:1; /**< Flip sprite vertically (isRotoscale cleared).*/
						u8					:2;
					};
					struct {
						u8					:1;
						u8 rotationIndex	:5; /**< Affine parameter number to use (isRotoscale set). */
						ObjSize size		:2; /**< Sprite size. */
					};
				};
			};
		};

		struct
		{
			u16 gfxIndex					:10;/**< Upper-left tile index. */
			ObjPriority priority			:2;	/**< Sprite priority. */
			u8 palette						:4;	/**< Sprite palette to use in paletted color modes. */
		};

		u16 attribute3;							/* Unused! Four of those are used as a sprite rotation matrice */
	};

	struct {
		uint16 attribute[3];
		uint16 filler;
	};

} SpriteEntry, * pSpriteEntry;



//! A sprite rotation entry.
typedef struct SpriteRotation
{
	uint16 filler1[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 hdx;			/**< The change in x per horizontal pixel */

	uint16 filler2[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 vdx;			/**< The change in y per horizontal pixel */

	uint16 filler3[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 hdy;			/**< The change in x per vertical pixel */

	uint16 filler4[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 vdy;			/**< The change in y per vertical pixel */
} SpriteRotation, * pSpriteRotation;


//! maximum number of sprites per engine available.
#define SPRITE_COUNT 128
//! maximum number of affine matrices per engine available.
#define MATRIX_COUNT 32


//is this union still used?
typedef union OAMTable {
	SpriteEntry oamBuffer[SPRITE_COUNT];
	SpriteRotation matrixBuffer[MATRIX_COUNT];
} OAMTable;



//! Enumerates all sizes supported by the 2D engine.
typedef enum {
   SpriteSize_8x8   = (OBJSIZE_8 << 14) | (OBJSHAPE_SQUARE << 12) | (8*8>>5),		//!< 8x8
   SpriteSize_16x16 = (OBJSIZE_16 << 14) | (OBJSHAPE_SQUARE << 12) | (16*16>>5),	//!< 16x16
   SpriteSize_32x32 = (OBJSIZE_32 << 14) | (OBJSHAPE_SQUARE << 12) | (32*32>>5),	//!< 32x32
   SpriteSize_64x64 = (OBJSIZE_64 << 14) | (OBJSHAPE_SQUARE << 12) | (64*64>>5),	//!< 64x64

   SpriteSize_16x8  = (OBJSIZE_8 << 14)  | (OBJSHAPE_WIDE << 12) | (16*8>>5),		//!< 16x8
   SpriteSize_32x8  = (OBJSIZE_16 << 14) | (OBJSHAPE_WIDE << 12) | (32*8>>5),		//!< 32x8
   SpriteSize_32x16 = (OBJSIZE_32 << 14) | (OBJSHAPE_WIDE << 12) | (32*16>>5),		//!< 32x16
   SpriteSize_64x32 = (OBJSIZE_64 << 14) | (OBJSHAPE_WIDE << 12) | (64*32>>5),		//!< 64x32

   SpriteSize_8x16  = (OBJSIZE_8 << 14)  | (OBJSHAPE_TALL << 12) | (8*16>>5),		//!< 8x16
   SpriteSize_8x32  = (OBJSIZE_16 << 14) | (OBJSHAPE_TALL << 12) | (8*32>>5),		//!< 8x32
   SpriteSize_16x32 = (OBJSIZE_32 << 14) | (OBJSHAPE_TALL << 12) | (16*32>>5),		//!< 16x32
   SpriteSize_32x64 = (OBJSIZE_64 << 14) | (OBJSHAPE_TALL << 12) | (32*64>>5)		//!< 32x64

}SpriteSize;

#define SPRITE_SIZE_SHAPE(size) (((size) >> 12) & 0x3)
#define SPRITE_SIZE_SIZE(size)  (((size) >> 14) & 0x3)
#define SPRITE_SIZE_PIXELS(size) (((size) & 0xFFF) << 5)

//! Graphics memory layout options.
typedef enum{
   SpriteMapping_1D_32 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_32 | (0 << 28) | 0,	/**< 1D tile mapping 32 byte boundary between offset */
   SpriteMapping_1D_64 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64 | (1 << 28) | 1,	/**< 1D tile mapping 64 byte boundary between offset */
   SpriteMapping_1D_128 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | (2 << 28) | 2,	/**< 1D tile mapping 128 byte boundary between offset */
   SpriteMapping_1D_256 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256 | (3 << 28) | 3,	/**< 1D tile mapping 256 byte boundary between offset */
   SpriteMapping_2D = DISPLAY_SPR_2D | (4 << 28),									/**< 2D tile mapping 32 byte boundary between offset */
   SpriteMapping_Bmp_1D_128 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP |DISPLAY_SPR_1D_BMP_SIZE_128 | (5 << 28) | 2,/**< 1D bitmap mapping 128 byte boundary between offset */
   SpriteMapping_Bmp_1D_256 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256 | DISPLAY_SPR_1D_BMP |DISPLAY_SPR_1D_BMP_SIZE_256 | (6 << 28) | 3,/**< 1D bitmap mapping 256 byte boundary between offset */
   SpriteMapping_Bmp_2D_128 = DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_128 | (7 << 28) | 2,	/**< 2D bitmap mapping 128 pixels wide bitmap */
   SpriteMapping_Bmp_2D_256 = DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256 | (8 << 28) | 3	/**< 2D bitmap mapping 256 pixels wide bitmap */
}SpriteMapping;

//! Color formats for sprite graphics.
typedef enum{
   SpriteColorFormat_16Color = OBJCOLOR_16,/**< 16 colors per sprite*/
   SpriteColorFormat_256Color = OBJCOLOR_256,/**< 256 colors per sprite*/
   SpriteColorFormat_Bmp = OBJMODE_BITMAP/**< 16-bit sprites*/
}SpriteColorFormat;

#endif // _libnds_sprite_h_
