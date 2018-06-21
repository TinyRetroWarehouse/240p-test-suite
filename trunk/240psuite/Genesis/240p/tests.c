/* 
 * 240p Test Suite
 * Copyright (C)2011-2014 Artemio Urbina
 *
 * This file is part of the 240p Test Suite
 *
 * The 240p Test Suite is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The 240p Test Suite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 240p Test Suite; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "genesis.h"
#include "myvdp.h"
#include "res.h"
#include "tests.h"
#include "help.h"
#include "main.h"

typedef struct timecode
{
	u16 hours;
	u16 minutes;
	u16 seconds;
	u16 frames;
	u16 type;
	u16 res;
} timecode;

void DrawCheckBoard()
{
	char cntstr[4];
	u16 ind = 0, size = 0, count = 0, docounter = 0, loadvram = 1;
	u16 field = 1, alternate = 0, exit = 0;
	u16 buttons, oldButtons = 0xffff, pressedButtons;

	while(!exit)
	{
		if(loadvram)
		{
			if(enable_256)
				VDP_setScreenWidth256();
			else
				VDP_setScreenWidth320();
				
			VDP_setPalette(PAL1, bw_pal);

			ind = TILE_USERINDEX;
			size = sizeof(check_tile) / 32;
			VDP_loadTileData(check_tile, ind, size, USE_DMA);
			ind += size;
			size = sizeof(check_tile_inv) / 32;
			VDP_loadTileData(check_tile_inv, ind, size, USE_DMA);

			VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + TILE_USERINDEX, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
			loadvram = 0;
		}

		if(alternate)
		{
			if(field == 0)
			{
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + TILE_USERINDEX, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 1;
			}
			else
			{
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 0;
			}
		}

		if(docounter)
		{
			count++;

			if(IsPALVDP)
			{
				if(count > 49)
					count = 0;
			}
			else
			{
				if(count > 59)
					count = 0;
			}

			intToStr(count, cntstr, 2);
			VDP_drawText("Frame:", 2, 25);
			VDP_drawText(cntstr, 8, 25);
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_CHECK))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
			alternate = ~alternate;

		if(pressedButtons & BUTTON_B && !alternate)
		{
			if(field == 0)
			{
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + TILE_USERINDEX, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 1;
			}
			else
			{
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 0;
			}
		}

		if(pressedButtons & BUTTON_C)
			docounter = ~docounter;

		if(pressedButtons & BUTTON_START)
			exit = 1;
	
		VDP_waitVSync();
	}
}

void DrawStripes()
{
	char cntstr[4];
	u16 hor1 = 0, hor2 = 0, ver1 = 0, ver2 = 0, size = 0, count = 0, docounter = 0, loadvram = 1;
	u16 field = 1, alternate = 0, exit = 0, vertical = 0, redraw = 0;
	u16 buttons = 0, oldButtons = 0xffff, pressedButtons = 0;	

	while(!exit)
	{
		if(loadvram)
		{
			if(enable_256)
				VDP_setScreenWidth256();
			else
				VDP_setScreenWidth320();
				
			VDP_setPalette(PAL1, bw_pal);

			hor1 = TILE_USERINDEX;
			size = sizeof(bw_tile) / 32;
			VDP_loadTileData(bw_tile, hor1, size, USE_DMA);
			hor2 = hor1 + size;
			size = sizeof(wb_tile) / 32;
			VDP_loadTileData(wb_tile, hor2, size, USE_DMA);
			ver1 = hor2 + size;
			size = sizeof(vstripes_tiles) / 32;
			VDP_loadTileData(vstripes_tiles, ver1, size, USE_DMA);
			ver2 = ver1 + size;
			size = sizeof(vstripesneg_tiles) / 32;
			VDP_loadTileData(vstripesneg_tiles, ver2, size, USE_DMA);

			redraw = 1;
			loadvram = 0;
		}

		if(alternate || redraw)
		{
			if(field == 0)
			{
				if(vertical)
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ver1, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				else
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + hor1, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 1;
			}
			else
			{
				if(vertical)
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ver2, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				else
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + hor2, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 0;
			}
			redraw = 0;
		}

		if(docounter)
		{
			count++;
			if(Detect_VDP_PAL())
			{
				if(count > 49)
					count = 0;
			}
			else
			{
				if(count > 59)
					count = 0;
			}

			intToStr(count, cntstr, 2);
			VDP_drawText("Frame:", 2, 25);
			VDP_drawText(cntstr, 8, 25);
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_STRIPES))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
			alternate = ~alternate;

		if(pressedButtons & BUTTON_B && !alternate)
		{
			if(field == 0)
			{
				if(vertical)
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ver1, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				else
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + hor1, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 1;
			}
			else
			{
				if(vertical)
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ver2, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				else
					VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + hor2, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
				field = 0;
			}
		}

		if(pressedButtons & BUTTON_LEFT || pressedButtons & BUTTON_RIGHT)
		{
			vertical = ~vertical;
			redraw = 1;
		}

		if(pressedButtons & BUTTON_C)
			docounter = ~docounter;

		if(pressedButtons & BUTTON_START)
			exit = 1;

		VDP_waitVSync();
	}
}

void DropShadowTest()
{
	u16 sonic_water = 0, sonic_floor = 0, frame = 0, direction = 0, loadvram = 1;
	u16 size = 0, ind = 0, back = 0, changeback = 1, invert = 0, sprite = 0, redraw = 0;
	u16 field = 1, x = 0, y = 0, exit = 0, text = 0, shadowpos = 0, buzzpos = 0, buzzshadowpos = 0, waterfall = 0;
	u16 buttons = 0, pressedButtons = 0, oldButtons = 0xffff;

	while(!exit)
	{
		if(loadvram)
		{
			VDP_setPalette(PAL0, palette_green);
			VDP_setPalette(PAL1, motoko_pal);
			VDP_setPalette(PAL2, wb_pal);
			VDP_setPalette(PAL3, buzz_pal);

			shadowpos = TILE_USERINDEX;
			size = sizeof(shadow_tiles) / 32;
			VDP_loadTileData(shadow_tiles, shadowpos, size, USE_DMA);

			buzzpos = shadowpos + size;
			size = sizeof(buzzShadow_tiles) / 32;
			VDP_loadTileData(buzz_tiles, buzzpos, size, USE_DMA);

			buzzshadowpos = buzzpos + size;
			size = sizeof(buzzShadow_tiles) / 32;
			VDP_loadTileData(buzzShadow_tiles, buzzshadowpos, size, USE_DMA);

			waterfall = buzzshadowpos + size;
			size = sizeof(waterfall_tiles) / 32;
			VDP_loadTileData(waterfall_tiles, waterfall, size, USE_DMA);

			ind = waterfall + size;

			sprite = random() % 2;

			VDP_setSprite(0, 12, 12, SPRITE_SIZE(4, 4), TILE_ATTR(PAL3, 0, 0, 0) + buzzpos, 1);
			VDP_setSprite(1, 32, 32, SPRITE_SIZE(4, 4), TILE_ATTR(PAL2, 0, 0, 0) + buzzshadowpos, 2);
			VDP_setSprite(2, 0, 0, SPRITE_SIZE(4, 4), TILE_ATTR(PAL2, 0, 0, 0) + shadowpos, 3);
			VDP_setSprite(3, 320, 224, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 4);
			VDP_setSprite(4, 320, 224, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 0);

			if(sprite == 1)
			{
				x = 32;
				y = 32;

				VDP_setSpritePosition(2, 320, 224);
			}
			else
			{
				VDP_setSpritePosition(0, 320, 224);
				VDP_setSpritePosition(1, 320, 224);
			}
			loadvram = 0;
			changeback = 1;
		}

		if(changeback || redraw)
		{
			if(redraw)
				VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);

			if(back != 1)
				VDP_setPalette(PAL0, palette_green);

			if(back == 2)
			{
				VDP_setHorizontalScroll(PLAN_B, 0);
				VDP_setHorizontalScroll(PLAN_A, 0);
				VDP_setSpritePosition(3, 320, 224);
				VDP_setSpritePosition(4, 320, 224);
				VDP_updateSprites();
			}

			switch (back)
			{
			case 0:
				if(!redraw)
				{
					size = sizeof(motoko_tiles) / 32;
					VDP_loadTileData(motoko_tiles, ind, size, USE_DMA);
					VDP_setPalette(PAL1, motoko_pal);
				}
				VDP_setMyTileMapRect(BPLAN, motoko_map, ind, 0, 0, 320 / 8, 224 / 8);
				break;
			case 1:
				if(!redraw)
				{
					VDP_setPalette(PAL0, sonicback_pal);
					VDP_setPalette(PAL1, sonicwater_pal);

					size = sizeof(sonicback_tiles) / 32;
					VDP_loadTileData(sonicback_tiles, ind, size, USE_DMA);

					sonic_water = ind + size;
					size = sizeof(sonicwater_tiles) / 32;
					VDP_loadTileData(sonicwater_tiles, sonic_water, size, USE_DMA);

					sonic_floor = sonic_water + size;
					size = sizeof(sonicfloor_tiles) / 32;
					VDP_loadTileData(sonicfloor_tiles, sonic_floor, size, USE_DMA);
				}

				if(!redraw)
				{
					VDP_setMyTileMapRect(BPLAN, sonicback_map, ind, 0, 0, 256 / 8, 152 / 8);
					VDP_setMyTileMapRect(BPLAN, sonicback_map, ind, 256 / 8, 0, 256 / 8, 152 / 8);
					VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 0, 152 / 8, 256 / 8, 48 / 8);
					VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 256 / 8, 152 / 8, 256 / 8, 48 / 8);
				}
				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 0, 96 / 8, 256 / 8, 128 / 8);
				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 256 / 8, 96 / 8, 256 / 8, 128 / 8);
				break;
			case 2:
				if(!redraw)
				{
					size = sizeof(check_tile) / 32;
					VDP_loadTileData(check_tile, ind, size, USE_DMA);
					VDP_setPalette(PAL1, bw_pal);
				}
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, 224 / 8);
				break;
			case 3:
				if(!redraw)
				{
					size = sizeof(bw_tile) / 32;
					VDP_loadTileData(bw_tile, ind, size, USE_DMA);
					VDP_setPalette(PAL1, bw_pal);
				}
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, 224 / 8);
				break;
			}
			changeback = 0;
			redraw = 0;
		}

		if(text)
		{
			text--;
			if(text == 0)
				redraw = 1;
		}

		if(sprite == 1)
			VDP_setSpritePosition(0, x - 20, y - 20);

		if(field == invert)
		{
			VDP_setSpritePosition(sprite == 1 ? 1 : 2, x, y);

			field = !field;
		}
		else
		{
			VDP_setSpritePosition(sprite == 1 ? 1 : 2, 320, 224);

			field = !field;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;
		
		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_SHADOW))
			loadvram = 1;

		if(buttons & BUTTON_UP)
		{
			if(y > 0)
				y--;
		}

		if(buttons & BUTTON_DOWN)
		{
			if(y < 192)
				y++;
		}

		if(buttons & BUTTON_LEFT)
		{
			if(x > 0)
				x--;
			if(direction == 1)
			{
				direction = 0;
				VDP_setSpriteAttr(0, TILE_ATTR(PAL3, 0, 0, direction) + buzzpos);
				VDP_setSpriteAttr(1, TILE_ATTR(PAL2, 0, 0, direction) + buzzshadowpos);
			}
		}

		if(buttons & BUTTON_RIGHT)
		{
			if(x < 288)
				x++;
			if(direction == 0)
			{
				direction = 1;
				VDP_setSpriteAttr(0, TILE_ATTR(PAL3, 0, 0, direction) + buzzpos);
				VDP_setSpriteAttr(1, TILE_ATTR(PAL2, 0, 0, direction) + buzzshadowpos);
			}
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_A)
		{
			invert = !invert;

			if(invert)
				VDP_drawText("Shadow on odd frames ", 19, 0);
			else
				VDP_drawText("Shadow on even frames", 19, 0);

			text = 60;
		}

		if(back == 1)
		{
			VDP_setSpritePosition(3, x * -2 + 79, 120);
			VDP_setSpritePosition(4, x * -2 + 335, 120);
			VDP_setHorizontalScroll(PLAN_B, x * -2);
			VDP_setHorizontalScroll(PLAN_A, x * -4);

			switch (frame)
			{
			case 30:
				VDP_setPalette(PAL1, sonicwater3_pal);
				break;
			case 60:
				VDP_setPalette(PAL1, sonicwater2_pal);
				break;
			case 90:
				VDP_setPalette(PAL1, sonicwater_pal);
				break;
			}

			frame++;
			if(frame > 90)
				frame = 1;
		}

		if(pressedButtons & BUTTON_B)
		{
			if(back < 3)
				back++;
			else
				back = 0;

			changeback = 1;
			VDP_clearTileMapRect(BPLAN, 0, 0, 320 / 8, 224 / 8);
			VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);
		}

		if(pressedButtons & BUTTON_C)
		{
			if(!sprite)
				VDP_setSpritePosition(2, 320, 224);
			else
			{
				VDP_setSpritePosition(0, 320, 224);
				VDP_setSpritePosition(1, 320, 224);
			}

			sprite = !sprite;
		}

		VDP_updateSprites();
		VDP_waitVSync();
	}
}

void StripedSpriteTest()
{
	u16 sonic_water = 0, sonic_floor = 0, sprite, frame = 0, waterfall = 0;
	u16 size, ind = 0, back = 0, changeback = 1, loadvram = 1;
	u16 x = 0, y = 0, exit = 0;
	u16 buttons, pressedButtons, oldButtons = 0xffff;

	while(!exit)
	{
		if(loadvram)
		{
			VDP_setPalette(PAL1, motoko_pal);
			VDP_setPalette(PAL3, wb_pal);

			sprite = TILE_USERINDEX;
			size = sizeof(striped_tiles) / 32;
			VDP_loadTileData(striped_tiles, sprite, size, USE_DMA);
			VDP_setSprite(0, x, y, SPRITE_SIZE(4, 4), TILE_ATTR(PAL3, 0, 0, 0) + sprite, 1);

			waterfall = sprite + size;
			size = sizeof(waterfall_tiles) / 32;
			VDP_loadTileData(waterfall_tiles, waterfall, size, USE_DMA);

			VDP_setSprite(1, 320, 224, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 2);
			VDP_setSprite(2, 320, 224, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 0);

			ind = waterfall + size;

			loadvram = 0;
			changeback = 1;
		}

		if(changeback)
		{
			changeback = 0;
			VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);
			VDP_clearTileMapRect(BPLAN, 0, 0, 320 / 8, 224 / 8);
			if(back != 1)
			{
				VDP_setHorizontalScroll(PLAN_B, 0);
				VDP_setHorizontalScroll(PLAN_A, 0);
				VDP_setSpritePosition(1, 320, 224);
				VDP_setSpritePosition(2, 320, 224);
				VDP_updateSprites();
			}
			switch (back)
			{
			case 0:
				size = sizeof(motoko_tiles) / 32;
				VDP_loadTileData(motoko_tiles, ind, size, USE_DMA);
				VDP_setPalette(PAL1, motoko_pal);
				VDP_setMyTileMapRect(BPLAN, motoko_map, ind, 0, 0, 320 / 8, 224 / 8);
				break;
			case 1:
				VDP_setPalette(PAL0, sonicback_pal);
				VDP_setPalette(PAL1, sonicwater_pal);
				size = sizeof(sonicback_tiles) / 32;
				VDP_loadTileData(sonicback_tiles, ind, size, USE_DMA);
				sonic_water = ind + size;
				size = sizeof(sonicwater_tiles) / 32;
				VDP_loadTileData(sonicwater_tiles, sonic_water, size, USE_DMA);
				sonic_floor = sonic_water + size;
				size = sizeof(sonicfloor_tiles) / 32;
				VDP_loadTileData(sonicfloor_tiles, sonic_floor, size, USE_DMA);
				VDP_setMyTileMapRect(BPLAN, sonicback_map, ind, 0, 0, 256 / 8, 152 / 8);
				VDP_setMyTileMapRect(BPLAN, sonicback_map, ind, 256 / 8, 0, 256 / 8, 152 / 8);
				VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 0, 152 / 8, 256 / 8, 48 / 8);
				VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 256 / 8, 152 / 8, 256 / 8, 48 / 8);
				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 0, 96 / 8, 256 / 8, 128 / 8);
				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 256 / 8, 96 / 8, 256 / 8, 128 / 8);
				break;
			case 2:
				size = sizeof(check_tile) / 32;
				VDP_loadTileData(check_tile, ind, size, USE_DMA);
				VDP_setPalette(PAL1, bw_pal);
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, 224 / 8);
				break;
			case 3:
				size = sizeof(bw_tile) / 32;
				VDP_loadTileData(bw_tile, ind, size, USE_DMA);
				VDP_setPalette(PAL1, bw_pal);
				VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + ind, 0, 0, 320 / 8, 224 / 8);
				break;
			}
		}

		VDP_setSpritePosition(0, x, y);

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_STRIPED))
			loadvram = 1;

		if(buttons & BUTTON_UP)
		{
			if(y > 0)
				y--;
		}

		if(buttons & BUTTON_DOWN)
		{
			if(y < 192)
				y++;
		}

		if(buttons & BUTTON_LEFT)
		{
			if(x > 0)
				x--;
		}

		if(buttons & BUTTON_RIGHT)
		{
			if(x < 288)
				x++;
		}

		if(back == 1)
		{
			VDP_setSpritePosition(1, x * -2 + 79, 120);
			VDP_setSpritePosition(2, x * -2 + 335, 120);
			VDP_setHorizontalScroll(PLAN_B, x * -2);
			VDP_setHorizontalScroll(PLAN_A, x * -4);

			switch (frame)
			{
			case 30:
				VDP_setPalette(PAL1, sonicwater3_pal);
				break;
			case 60:
				VDP_setPalette(PAL1, sonicwater2_pal);
				break;
			case 90:
				VDP_setPalette(PAL1, sonicwater_pal);
				break;
			}

			frame++;
			if(frame > 90)
				frame = 1;
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_A)
		{
			if(back > 0)
				back--;
			else
				back = 3;

			changeback = 1;
		}

		if(pressedButtons & BUTTON_B)
		{
			if(back < 3)
				back++;
			else
				back = 0;

			changeback = 1;
		}

		VDP_updateSprites();
		VDP_waitVSync();
	}
}



void LagTest()
{
	char str[10];
	u16 pal = PAL0, change = 1, loadvram = 1;
	s16 speed = 1, vary = 0;
	u16 size, ind = 0, usersound = 0;
	u16 x = 0, y = 0, x2 = 0, y2 = 0, exit = 0, variation = 1, draw = 1;
	u16 buttons, pressedButtons, oldButtons = 0xffff;
	u16 pos = 0, view = 0, audio = 1, drawoffset = 0;
	u16 first_pal[16], oldColor = 0;
	s16 clicks[10];

	x = 144;
	y = 60;

	x2 = 108;
	y2 = 96;
	
	PSG_init();
	PSG_setFrequency(0, 1000);

	while(!exit)
	{
		if(loadvram)
		{
			VDP_setPalette(PAL0, palette_grey);
			VDP_setPalette(PAL1, palette_red);
			VDP_setPalette(PAL2, bw_pal);
			VDP_setPalette(PAL3, palette_green);

			ind = TILE_USERINDEX;
			size = sizeof(shadow_tiles) / 32;
			VDP_loadTileData(shadow_tiles, ind, size, USE_DMA);
			ind += size;
			size = sizeof(lag_tiles) / 32;
			VDP_loadTileData(lag_tiles, ind, size, USE_DMA);

			VDP_setSprite(0, x, 96, SPRITE_SIZE(4, 4), TILE_ATTR(PAL2, 0, 0, 0) + ind, 1);
			VDP_setSprite(1, x, y, SPRITE_SIZE(4, 4), TILE_ATTR(PAL2, 0, 0, 0) + ind, 2);
			VDP_setSprite(2, x2, y2, SPRITE_SIZE(4, 4), TILE_ATTR(PAL2, 0, 0, 0) + ind, 0);

			VDP_drawTextBG(APLAN, "Press the \"A\" button when the sprite", TILE_ATTR(PAL3, 0, 0, 0), 2, 21);
			VDP_drawTextBG(APLAN, "is aligned. A negative value means", TILE_ATTR(PAL3, 0, 0, 0), 2, 22);
			VDP_drawTextBG(APLAN, "you pressed \"A\" before they intersect.", TILE_ATTR(PAL3, 0, 0, 0), 2, 23);
			VDP_drawTextBG(APLAN, "\"B\" button toggles horz/vert", TILE_ATTR(PAL3, 0, 0, 0), 2, 24);
			VDP_drawTextBG(APLAN, "\"C\" button toggles audio", TILE_ATTR(PAL3, 0, 0, 0), 2, 25);
			VDP_drawTextBG(APLAN, "DOWN toggles random/rhythmic", TILE_ATTR(PAL3, 0, 0, 0), 2, 26);

			loadvram = 0;
			draw = 1;

			if(pos)
			{
				int i = 0;
				u16 ppos = 0;

				for(i = 0; i < pos; i++)
				{
					pal = PAL0;

					intToStr(clicks[i], str, 1);

					if(clicks[i] == 0)
						pal = PAL3;
					if(clicks[i] < 0)
						pal = PAL1;

					ppos = i + 1;
					intToStr(ppos, str, 1);
					VDP_drawTextBG(APLAN, "Offset", TILE_ATTR(PAL0, 0, 0, 0), 2, i);
					VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL0, 0, 0, 0), 9, i);
					VDP_drawTextBG(APLAN, ":", TILE_ATTR(PAL0, 0, 0, 0), i == 9 ? 11 : 10, i);
					intToStr(clicks[i], str, 1);
					VDP_drawTextBG(APLAN, str, TILE_ATTR(pal, 0, 0, 0), i == 9 ? 13 : 12, i);
					ppos = strlen(str);
					if(clicks[i] == 1)
						VDP_drawTextBG(APLAN, " frame    ", TILE_ATTR(pal, 0, 0, 0), pos == 9 ? 13 : 12 + ppos, i);
					else
						VDP_drawTextBG(APLAN, " frames   ", TILE_ATTR(pal, 0, 0, 0), pos == 9 ? 13 : 12 + ppos, i);
				}
			}
		}
		
		if(y == 96)	// half the screen?        
		{
			if(audio && !usersound)
				PSG_setEnvelope(0, PSG_ENVELOPE_MIN);

			VDP_getPalette(PAL0, first_pal);
			first_pal[0] = oldColor;
			VDP_setPalette(PAL0, first_pal);
		}
		
		if(usersound)
		{
			PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
			usersound = 0;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_MANUALLAG))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
		{
			if(change)
			{
				clicks[pos] = (y - 96) * speed;
				drawoffset = 1;
				if(clicks[pos] >= 0)
					change = 0;
					
				if(audio)
				{
					if(clicks[pos] == 0)
						PSG_setFrequency(0, 1000);
					else
						PSG_setFrequency(0, 500);
						
					PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
					usersound = 1;
				}
			}
		}

		if(pressedButtons & BUTTON_B)
		{
			view++;
			if(view > 2)
				view = 0;
		}

		if(pressedButtons & BUTTON_C)
		{
			audio = !audio;
			draw = 1;
		}

		if(pressedButtons & BUTTON_DOWN)
		{
			variation = !variation;
			if(!variation)
				vary = 0;
			draw = 1;
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(drawoffset)
		{
			u16 ppos = 0;

			pal = PAL0;
			intToStr(clicks[pos], str, 1);

			if(clicks[pos] == 0)
				pal = PAL3;
			if(clicks[pos] < 0)
				pal = PAL1;

			ppos = pos + 1;
			intToStr(ppos, str, 1);
			VDP_drawTextBG(APLAN, "Offset", TILE_ATTR(PAL0, 0, 0, 0), 2, pos);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL0, 0, 0, 0), 9, pos);
			VDP_drawTextBG(APLAN, ":", TILE_ATTR(PAL0, 0, 0, 0), pos == 9 ? 11 : 10, pos);
			intToStr(clicks[pos], str, 1);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(pal, 0, 0, 0), pos == 9 ? 13 : 12, pos);
			ppos = strlen(str);
			if(clicks[pos] == 1)
				VDP_drawTextBG(APLAN, " frame    ", TILE_ATTR(pal, 0, 0, 0), pos == 9 ? 13 : 12 + ppos, pos);
			else
				VDP_drawTextBG(APLAN, " frames   ", TILE_ATTR(pal, 0, 0, 0), pos == 9 ? 13 : 12 + ppos, pos);

			if(clicks[pos] >= 0)
				pos++;

			if(pos > 9)
				exit = 1;
			drawoffset = 0;
		}

		if(draw)
		{
			VDP_drawTextBG(APLAN, "Audio:", TILE_ATTR(PAL0, 0, 0, 0), 24, 0);
			if(audio)
				VDP_drawTextBG(APLAN, "on ", TILE_ATTR(PAL0, 0, 0, 0), 31, 0);
			else
				VDP_drawTextBG(APLAN, "off", TILE_ATTR(PAL0, 0, 0, 0), 31, 0);
			VDP_drawTextBG(APLAN, "Timing:", TILE_ATTR(PAL0, 0, 0, 0), 24, 1);
			if(variation)
				VDP_drawTextBG(APLAN, "random  ", TILE_ATTR(PAL0, 0, 0, 0), 32, 1);
			else
				VDP_drawTextBG(APLAN, "rhythmic", TILE_ATTR(PAL0, 0, 0, 0), 32, 1);
			draw = 0;
		}

		if(y > 132 + vary)
		{
			speed = -1;
			change = 1;
			if(variation)
			{
				if(random() % 2)
					vary = random() % 7;
				else
					vary = -1 * random() % 7;
			}
		}

		if(y < 60 + vary)
		{
			speed = 1;
			change = 1;
			if(variation)
			{
				if(random() % 2)
					vary = random() % 7;
				else
					vary = -1 * random() % 7;
			}
		}

		y += speed;
		x2 += speed;

		if(y == 96)	// Red on the spot
			VDP_setSpriteAttr(0, TILE_ATTR(PAL1, 0, 0, 0) + ind);
		if(y == 95 || y == 97)	//Green one pixel before or after
			VDP_setSpriteAttr(0, TILE_ATTR(PAL3, 0, 0, 0) + ind);
		if(y == 98 || y == 94)	//Back to white two pixels before or after
			VDP_setSpriteAttr(0, TILE_ATTR(PAL2, 0, 0, 0) + ind);

		if(view == 0 || view == 2)
			VDP_setSpritePosition(1, x, y);
		else
			VDP_setSpritePosition(1, 320, 224);

		if(view == 1 || view == 2)
			VDP_setSpritePosition(2, x2, y2);
		else
			VDP_setSpritePosition(2, 320, 224);

		VDP_updateSprites();

		if(y == 96)	// half the screen?        
		{
			if(audio)
			{
				PSG_setFrequency(0, 1000);
				PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
			}

			VDP_getPalette(PAL0, first_pal);
			oldColor = first_pal[0];
			first_pal[0] = 0x0666;
			VDP_setPalette(PAL0, first_pal);
		}
		VDP_waitVSync();
	}

	if(pos > 9)
	{
		fix32 totald = 0, cnt, tot;
		u16 total = 0, count = 0, c = 0;

		PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
		VDP_resetSprites();
		VDP_updateSprites();

		VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);

		VDP_setPalette(PAL2, back_pal);

		size = sizeof(back_tiles) / 32;
		VDP_loadTileData(back_tiles, TILE_USERINDEX, size, USE_DMA);

		VDP_setMyTileMapRect(BPLAN, back_map, TILE_USERINDEX, 0, 0, 320 / 8, 224 / 8);
		exit = 0;

		for(c = 0; c < 10; c++)
		{
			if(clicks[c] != 0xFF)
			{
				intToStr(clicks[c], str, 1);

				pal = PAL0;
				if(clicks[c] == 0)
					pal = PAL3;
				if(clicks[c] < 0)
					pal = PAL1;
				if(clicks[c] >= 0)
				{
					total += clicks[c];
					count++;
				}

				VDP_drawTextBG(APLAN, str, TILE_ATTR(pal, 0, 0, 0), 10, c + 7);
			}
		}

		VDP_drawTextBG(APLAN, "+", TILE_ATTR(PAL1, 0, 0, 0), 8, 11);

		if(count > 0)
		{
			u16 h = 10;
			u16 v = 18;

			VDP_drawTextBG(APLAN, "----", TILE_ATTR(PAL0, 0, 0, 0), h - 2, v++);

			cnt = intToFix32(count);
			tot = intToFix32(total);

			intToStr(total, str, 1);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL0, 0, 0, 0), h, v);
			h += strlen(str);
			VDP_drawTextBG(APLAN, "/", TILE_ATTR(PAL1, 0, 0, 0), h++, v);

			intToStr(count, str, 1);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL0, 0, 0, 0), h, v);
			h += strlen(str);
			VDP_drawTextBG(APLAN, "=", TILE_ATTR(PAL1, 0, 0, 0), h++, v);

			totald = fix32Div(tot, cnt);
			fix32ToStr(totald, str, 4);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL3, 0, 0, 0), h, v);
			h += strlen(str);
			h -= 2;
			VDP_drawTextBG(APLAN, " frames", TILE_ATTR(PAL0, 0, 0, 0), h, v++);
			VDP_drawTextBG(APLAN, "Keep in mind that a frame is", TILE_ATTR(PAL0, 0, 0, 0), 6, ++v);
			VDP_drawTextBG(APLAN, "around 16.68 milliseconds.", TILE_ATTR(PAL0, 0, 0, 0), 6, ++v);

			h = 14;
			v = 12;
			tot = FIX32(16.6834);
			cnt = fix32Mul(totald, tot);
			fix32ToStr(cnt, str, 2);
			VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL3, 0, 0, 0), h, v);
			h += strlen(str);
			//h -= 2;
			VDP_drawTextBG(APLAN, " milliseconds", TILE_ATTR(PAL0, 0, 0, 0), h, v);

			if(total < 5)
				VDP_drawTextBG(APLAN, "EXCELLENT REFLEXES!", TILE_ATTR(PAL1, 0, 0, 0), 14, 15);
			if(total == 0)
				VDP_drawTextBG(APLAN, "INCREDIBLE REFLEXES!!", TILE_ATTR(PAL1, 0, 0, 0), 14, 15);
		}

		while(!exit)
		{
			buttons = JOY_readJoypad(JOY_1);
			pressedButtons = buttons & ~oldButtons;
			oldButtons = buttons;

			if(pressedButtons & BUTTON_START)
				exit = 1;
			VDP_waitVSync();
		}
	}
}

void HScrollTest()
{
	u16 size, sonic_floor, sonic_water, waterfall, loadvram = 1;
	u16 exit = 0, frame = 1, vertical = 0;
	u16 buttons, oldButtons = 0xffff, pressedButtons;
	int x = 0, y = 0, speed = 1, acc = -1, pause = 0;

	while(!exit)
	{
		if(loadvram)
		{	
			if(!vertical)
			{	
				if(enable_256)
					VDP_setScreenWidth256();
				else
					VDP_setScreenWidth320();
				
				VDP_setVerticalScroll(PLAN_A, 0);
				VDP_setHorizontalScroll(PLAN_A, x);
				VDP_setHorizontalScroll(PLAN_B, x / 2);				

				VDP_setPalette(PAL0, sonicback_pal);
				VDP_setPalette(PAL1, sonicwater_pal);
				
				size = sizeof(sonicback_tiles) / 32;
				VDP_loadTileData(sonicback_tiles, TILE_USERINDEX, size, USE_DMA);

				sonic_water = TILE_USERINDEX + size;
				size = sizeof(sonicwater_tiles) / 32;
				VDP_loadTileData(sonicwater_tiles, sonic_water, size, USE_DMA);

				sonic_floor = sonic_water + size;
				size = sizeof(sonicfloor_tiles) / 32;
				VDP_loadTileData(sonicfloor_tiles, sonic_floor, size, USE_DMA);

				waterfall = sonic_floor + size;
				size = sizeof(waterfall_tiles) / 32;
				VDP_loadTileData(waterfall_tiles, waterfall, size, USE_DMA);

				VDP_setMyTileMapRect(BPLAN, sonicback_map, TILE_USERINDEX, 0, 0, 256 / 8, 152 / 8);
				VDP_setMyTileMapRect(BPLAN, sonicback_map, TILE_USERINDEX, 256 / 8, 0, 256 / 8, 152 / 8);

				VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 0, 152 / 8, 256 / 8, 48 / 8);
				VDP_setMyTileMapRect(BPLAN, sonicwater_map, sonic_water, 256 / 8, 152 / 8, 256 / 8, 48 / 8);

				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 0, 96 / 8, 256 / 8, 128 / 8);
				VDP_setMyTileMapRect(APLAN, sonicfloor_map, sonic_floor, 256 / 8, 96 / 8, 256 / 8, 128 / 8);

				VDP_setSprite(0, 72, 120, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 1);
				VDP_setSprite(1, 328, 120, SPRITE_SIZE(4, 4), TILE_ATTR(PAL1, 0, 0, 0) + waterfall, 0);
			}
			else
			{
				VDP_resetSprites();
				VDP_updateSprites();
				VDP_setHorizontalScroll(PLAN_B, 0);
				VDP_setHorizontalScroll(PLAN_A, 0);
								
				VDP_setScreenWidth256();

				VDP_setPalette(PAL0, kiki_pal);
				size = sizeof(kiki_tiles) / 32;
				VDP_loadTileData(kiki_tiles, TILE_USERINDEX, size, USE_DMA);

				VDP_setMyTileMapRect(APLAN, kiki_map, TILE_USERINDEX, 0, 0, 256 / 8, 512 / 8);
			}

			loadvram = 0;
		}

		if(!vertical)
		{
			switch (frame)
			{
			case 30:
				VDP_setPalette(PAL1, sonicwater3_pal);
				break;
			case 60:
				VDP_setPalette(PAL1, sonicwater2_pal);
				break;
			case 90:
				VDP_setPalette(PAL1, sonicwater_pal);
				break;
			}
		}

		frame++;
		if(frame > 90)
			frame = 1;

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_HSCROLL))
			loadvram = 1;
		
		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_UP)
			speed++;

		if(pressedButtons & BUTTON_DOWN)
			speed--;

		if(speed > 20)
			speed = 20;

		if(speed < 0)
			speed = 0;

		if(pressedButtons & BUTTON_A)
			pause = !pause;

		if(pressedButtons & BUTTON_B)
			acc *= -1;
		
		if(pressedButtons & BUTTON_C)
		{
			vertical = !vertical;
			loadvram = 1;
			FadeAndCleanUp();
		}

		if(!vertical)
		{
			if(!pause)
				x += acc * speed;
			
			if(x >= 512)
				x = x % 512;

			if(x <= -512)
				x = x % -512;

			VDP_setSpritePosition(0, x / 2 + 79, 120);
			VDP_setSpritePosition(1, x / 2 + 335, 120);
			VDP_updateSprites();
			VDP_setHorizontalScroll(PLAN_A, x);
			VDP_setHorizontalScroll(PLAN_B, x / 2);
		}
		else
		{
			if(!pause)
				y += acc * speed;
			
			if(y >= 512)
				y = y % 512;

			if(x <= -512)
				y = y % -512;

			VDP_setVerticalScroll(PLAN_A, y);
		}
		VDP_waitVSync();
	}
}

void VScrollTest()
{
	u16 size, loadvram = 1;
	u16 exit = 0;
	u16 buttons, oldButtons = 0xffff, pressedButtons;
	int pos = 0, speed = 1, acc = -1, pause = 0, direction = 0;

	while(!exit)
	{
		if(loadvram)
		{
			if(enable_256)
				VDP_setScreenWidth256();
			else
				VDP_setScreenWidth320();
				
			VDP_setPalette(PAL0, bw_pal);

			size = sizeof(circles_grid_tiles) / 32;
			VDP_loadTileData(circles_grid_tiles, TILE_USERINDEX, size, USE_DMA);

			VDP_fillTileMapRect(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + TILE_USERINDEX, 0, 0, 512 / 8, 512 / 8);
			loadvram = 0;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_VSCROLL))
			loadvram = 1;

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_UP)
			speed++;

		if(pressedButtons & BUTTON_DOWN)
			speed--;

		if(speed > 5)
			speed = 5;

		if(speed < 0)
			speed = 0;

		if(pressedButtons & BUTTON_A)
			pause = !pause;

		if(pressedButtons & BUTTON_B)
			acc *= -1;

		if(pressedButtons & BUTTON_C)
			direction = !direction;

		if(!pause)
			pos += acc * speed;

		if(pos >= 512)
			pos = pos % 512;

		if(pos <= -512)
			pos = pos % -512;

		if(direction)
			VDP_setHorizontalScroll(PLAN_A, pos);
		else
			VDP_setVerticalScroll(PLAN_A, pos);
		VDP_waitVSync();
	}
}

const u16 PITCHES[12] = {
	617,653,692,733,777,823,872,924,979,1037,1099,1164
};

struct {
	u8 msb[12];
	u8 lsb[12];
} pitchTable;

u8 part = 0; //0 or 1

static void init(){
	u8 i,j,f1,f2;
	u16 temp=0;

	//separate pitches into two chars
	for (i=0; i<12; i++){
		temp = PITCHES[i];
		for (j=f1=f2=0; j<11; j++){
			//set each bit to remainder (0 or 1) starting from lsb
			if (j<8) f1 += (temp % 2) << j;
			//do the same for second char
			else f2 += (temp % 2) << (j - 8);
			temp /= 2;
		}
		pitchTable.lsb[i] = f1;
		pitchTable.msb[i] = f2;
	}

	Z80_requestBus(1);
	YM2612_reset();
	Z80_releaseBus();
}

void ymTest(u8 channel, u8 octave)
{
	if(channel < 3)
		part = 0;
	else
	{
		part = 1;
		channel = channel - 3;
	}
		
	Z80_requestBus(1);
	
	// Register 22H - LFO (General)
	YM2612_writeReg(part,0x22,0); 
	// Register 27H - Timers; Ch3/6 mode
	YM2612_writeReg(part,0x27,0); 

	// Register 30H+ - detune & multiple
	YM2612_writeReg(part,0x30+channel,0x71); // OP1
	YM2612_writeReg(part,0x34+channel,0x0D); // OP3
	YM2612_writeReg(part,0x38+channel,0x33); // OP2
	YM2612_writeReg(part,0x3C+channel,0x01); // OP4

	// Register 40H+ - total level: 0 being the largest and 127 (decimal) the smallest.
	YM2612_writeReg(part,0x40+channel,0x23); // OP1
	YM2612_writeReg(part,0x44+channel,0x2D); // OP3
	YM2612_writeReg(part,0x48+channel,0x26); // OP2
	YM2612_writeReg(part,0x4C+channel,0x00); // OP4

	// Register 50H+ - rate scaling; attack rate
	YM2612_writeReg(part,0x50+channel,0x5F); // OP1
	YM2612_writeReg(part,0x54+channel,0x99); // OP3
	YM2612_writeReg(part,0x58+channel,0x5F); // OP2
	YM2612_writeReg(part,0x5C+channel,0x94); // OP4

	// Register 60H+ - first decay rate; amplitude modulation
	YM2612_writeReg(part,0x60+channel,0x5); // OP1
	YM2612_writeReg(part,0x64+channel,0x5); // OP3
	YM2612_writeReg(part,0x68+channel,0x5); // OP2
	YM2612_writeReg(part,0x6C+channel,0x7); // OP4

	// Register 70H+ - secondary decay rate
	YM2612_writeReg(part,0x70+channel,0x2); // OP1
	YM2612_writeReg(part,0x74+channel,0x2); // OP3
	YM2612_writeReg(part,0x78+channel,0x2); // OP2
	YM2612_writeReg(part,0x7C+channel,0x2); // OP4

	// Register 80H+ - secondary amplitude; release rate
	YM2612_writeReg(part,0x80+channel,0x11); // OP1
	YM2612_writeReg(part,0x84+channel,0x11); // OP3
	YM2612_writeReg(part,0x88+channel,0x11); // OP2
	YM2612_writeReg(part,0x8C+channel,0xA6); // OP4

	// Register 90H+ - proprietary
	YM2612_writeReg(part,0x90+channel,0); // OP1
	YM2612_writeReg(part,0x94+channel,0); // OP3
	YM2612_writeReg(part,0x98+channel,0); // OP2
	YM2612_writeReg(part,0x9C+channel,0); // OP4

	// Register B0H+ - feedback; algorithm
	YM2612_writeReg(part,0xB0+channel,0x32); 
	
	// Register B4H+ - stereo; LFO sensitivity
	YM2612_writeReg(part,0xB4+channel,0xC0); 
	
	// Register 28H - Key on/off
	YM2612_writeReg(0,0x28,0x00+(part ? channel+4 : channel));

	/*
	Registers A0H-AFH - frequency
	Channel 1's frequency is in A0 and A4H.
	Channel 2's frequency is in A1 and A5H.
	Channel 3's frequency, if it is in normal mode is in A2 and A6H.
	*/
	YM2612_writeReg(part,0xA4+channel,octave + pitchTable.msb[(part ? channel+4 : channel)*2]);
	YM2612_writeReg(part,0xA0+channel,pitchTable.lsb[(part ? channel+4 : channel)*2]);
	
	// Register 28H - Key on/off
	YM2612_writeReg(0,0x28,0xF0+(part ? channel+4 : channel));
	
	{
		char buffer[2];
		
		intToHex(0xF0+(part ? channel+4 : channel), buffer, 2);
		//VDP_drawTextBG(APLAN, buffer, TILE_ATTR(PAL3, 0, 0, 0), 18, 14);
	}
		
	Z80_releaseBus();
}

void ym2612_keyoff()
{
	Z80_requestBus(1);
	YM2612_reset();
	Z80_releaseBus();
}

void SoundTest()
{
	int sel = 2, loadvram = 1, type = 0, psgoff = 0;
	u16 ind = 0, size = 0, exit = 0;
	u16 buttons, oldButtons = 0xffff, pressedButtons;
	u16 len = 0, redraw = 0, selmax = 4;
	u8 octave = 24;

	init();
	PSG_init();
	while(!exit)
	{
		if(loadvram)
		{
			len = sizeof(beep);
			VDP_setPalette(PAL0, palette_grey);
			VDP_setPalette(PAL1, palette_green);
			VDP_setPalette(PAL2, back_pal);
			VDP_setPalette(PAL3, palette_red);

			ind = TILE_USERINDEX;
			size = sizeof(back_tiles) / 32;
			VDP_loadTileData(back_tiles, ind, size, USE_DMA);

			VDP_setMyTileMapRect(BPLAN, back_map, TILE_USERINDEX, 0, 0, 320 / 8, 224 / 8);
			loadvram = 0;
			redraw = 1;
		}
		
		if(redraw)
		{
			int y = 9;
			char buffer[2];
			
			VDP_drawTextBG(APLAN, "Sound Test", TILE_ATTR(PAL1, 0, 0, 0), 14, 6);
			
			VDP_drawTextBG(APLAN, "YM2612 FM", TILE_ATTR(PAL1, 0, 0, 0), 14, y++);
			VDP_drawTextBG(APLAN, "1", TILE_ATTR((type == 0 && sel == 0) ? PAL3 : PAL0, 0, 0, 0), 14, y);
			VDP_drawTextBG(APLAN, "2", TILE_ATTR((type == 0 && sel == 1) ? PAL3 : PAL0, 0, 0, 0), 16, y);
			VDP_drawTextBG(APLAN, "3", TILE_ATTR((type == 0 && sel == 2) ? PAL3 : PAL0, 0, 0, 0), 18, y);
			VDP_drawTextBG(APLAN, "4", TILE_ATTR((type == 0 && sel == 3) ? PAL3 : PAL0, 0, 0, 0), 20, y);
			VDP_drawTextBG(APLAN, "5", TILE_ATTR((type == 0 && sel == 4) ? PAL3 : PAL0, 0, 0, 0), 22, y);
			y++;
			VDP_drawTextBG(APLAN, "FM Octave:", TILE_ATTR((type == 1 && sel == 0) ? PAL3 : PAL0, 0, 0, 0), 12, y);
			intToStr(octave, buffer, 2);
			VDP_drawTextBG(APLAN, buffer, TILE_ATTR((type == 1 && sel == 0) ? PAL3 : PAL0, 0, 0, 0), 23, y);
			
			y+=2;
			VDP_drawTextBG(APLAN, "YM2612 PCM (CH6)", TILE_ATTR(PAL1, 0, 0, 0), 11, y++);
			VDP_drawTextBG(APLAN, "Left", 		TILE_ATTR((type == 2 && sel == 0) ? PAL3 : PAL0, 0, 0, 0), 10, y);
			VDP_drawTextBG(APLAN, "Center", 	TILE_ATTR((type == 2 && sel == 1) ? PAL3 : PAL0, 0, 0, 0), 15, y);
			VDP_drawTextBG(APLAN, "Right", 		TILE_ATTR((type == 2 && sel == 2) ? PAL3 : PAL0, 0, 0, 0), 22, y);
			
			y+=2;
			VDP_drawTextBG(APLAN, "PSG CHANNEL", TILE_ATTR(PAL1, 0, 0, 0), 13, y++);
			VDP_drawTextBG(APLAN, "0", TILE_ATTR((type == 3 && sel == 0) ? PAL3 : PAL0, 0, 0, 0), 15, y);
			VDP_drawTextBG(APLAN, "1", TILE_ATTR((type == 3 && sel == 1) ? PAL3 : PAL0, 0, 0, 0), 17, y);
			VDP_drawTextBG(APLAN, "2", TILE_ATTR((type == 3 && sel == 2) ? PAL3 : PAL0, 0, 0, 0), 19, y);
			VDP_drawTextBG(APLAN, "3", TILE_ATTR((type == 3 && sel == 3) ? PAL3 : PAL0, 0, 0, 0), 21, y);
			
			redraw = 0;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_SOUND))
			loadvram = 1;

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_UP)
		{
			type--;
			redraw = 1;
		}

		if(pressedButtons & BUTTON_DOWN)
		{
			type++;
			redraw = 1;
		}
		
		if(type > 3)
			type = 0;

		if(type < 0)
			type = 3;
		
		if(redraw)
			switch(type)
			{
				case 0:
					selmax = 4;
					break;
				case 1:
					selmax = 0;
					break;
				case 2:
					selmax = 2;
					break;
				case 3:
					selmax = 3;
					break;
				default:
					break;
			}
		
		if(pressedButtons & BUTTON_LEFT)
		{
			sel--;
			redraw = 1;
		}

		if(pressedButtons & BUTTON_RIGHT)
		{
			sel++;
			redraw = 1;
		}

		if(sel > selmax)
			sel = 0;

		if(sel < 0)
			sel = selmax;

		if(pressedButtons & BUTTON_A)
		{
			switch(type)
			{
				case 0: //YM2612
				{
					ymTest(sel, octave);
				}
				break;
				case 1:
				{
					octave += 8;
					if(octave > 56)
						octave = 0;
					redraw = 1;
				}
				break;
				case 2: // PCM
				{
					if(sel == 0)
					{
						SND_stopPlay_PCM();
						SND_startPlay_PCM(beep, len, (u8) 16000, SOUND_PAN_LEFT, 0);
					}
					if(sel == 1)
					{
						SND_stopPlay_PCM();
						SND_startPlay_PCM(beep, len, (u8) 16000, SOUND_PAN_CENTER, 0);
					}
					if(sel == 2)
					{
						SND_stopPlay_PCM();
						SND_startPlay_PCM(beep, len, (u8) 16000, SOUND_PAN_RIGHT, 0);
					}
				}
				break;
				case 3: // PSG
				{
					if(sel == 0)
					{
						PSG_setFrequency(0, 200);
						PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
					}
					if(sel == 1)
					{
						PSG_setFrequency(1, 2000);
						PSG_setEnvelope(1, PSG_ENVELOPE_MAX);
					}
					if(sel == 2)
					{
						PSG_setFrequency(2, 4095);
						PSG_setEnvelope(2, PSG_ENVELOPE_MAX);
					}
					if(sel == 3)
					{
						PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3);
						PSG_setFrequency(3, 500);
						PSG_setEnvelope(3, PSG_ENVELOPE_MAX);
					}
					
					psgoff = 60;
				}
				break;
				default:
					break;
			}
		}
		
		if(psgoff)
		{
			psgoff --;
			if(psgoff == 0)
				StopPSG();
		}


		VDP_waitVSync();
	}
	ym2612_keyoff();
	SND_stopPlay_PCM();
	StopPSG();
}

void LEDZoneTest()
{
	u16 size = 0, sprite0 = 0, sprite1 = 0, sprite2 = 0, sprite3 = 0, sprite4 = 0, tmp = 0;
	u16 x = 160, y = 112, exit = 0, sprite = 1, change = 0, draw = 1;
	u16 buttons, pressedButtons, oldButtons = 0xffff, loadvram = 1;

	if(enable_256)
		x = 128;
		
	while(!exit)
	{
		if(loadvram)
		{
			if(enable_256)
				VDP_setScreenWidth256();
			else
				VDP_setScreenWidth320();
				
			VDP_setPalette(PAL1, bw_pal);

			size = sizeof(size0led_t) / 32;

			sprite0 = TILE_USERINDEX;
			VDP_loadTileData(size0led_t, sprite0, size, USE_DMA);
			sprite1 = sprite0 + size;
			VDP_loadTileData(size1led_t, sprite1, size, USE_DMA);
			sprite2 = sprite1 + size;
			VDP_loadTileData(size2led_t, sprite2, size, USE_DMA);
			sprite3 = sprite2 + size;
			VDP_loadTileData(size3led_t, sprite3, size, USE_DMA);
			sprite4 = sprite3 + size;
			VDP_loadTileData(size4led_t, sprite4, size, USE_DMA);

			VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite1, 0);

			VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
			loadvram = 0;
		}

		VDP_setSpritePosition(0, x, y);

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;
		
		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_LED))
			loadvram = 1;

		if(draw)
		{
			if(buttons & BUTTON_UP)
			{
				if(y > 0)
					y--;
			}

			if(buttons & BUTTON_DOWN)
			{
				if(y + size < (pal_240 ? 240 : 224))
					y++;
			}

			if(buttons & BUTTON_LEFT)
			{
				if(x > 0)
					x--;
			}

			if(buttons & BUTTON_RIGHT)
			{
				if(x + size < (enable_256 ? 256 : 320))
					x++;
			}
			
			if(enable_256 && x > (256 - size))
				x = 256 - size;
			
			if(!pal_240 && y > (224 - size))
				y = 224 - size;

		}
		
		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(pressedButtons & BUTTON_A)
		{
			if(sprite == 0)
				sprite = 4;
			else
				sprite--;
			change = 1;
		}

		if(pressedButtons & BUTTON_B)
		{
			if(sprite == 4)
				sprite = 0;
			else
				sprite++;
			change = 1;
		}

		if(pressedButtons & BUTTON_C)
		{
			draw = !draw;
			change = 1;
			if(draw)
				x = tmp;
			else
			{
				tmp = x;
				x = 340;
			}
		}

		if(change)
		{
			switch (sprite)
			{
			case 0:
				VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite0, 0);
				size = 1;
				break;
			case 1:
				VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite1, 0);
				size = 2;
				break;
			case 2:
				VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite2, 0);
				size = 4;
				break;
			case 3:
				VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite3, 0);
				size = 6;
				break;
			case 4:
				VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite4, 0);
				size = 8;
				break;
			}
		}

		VDP_updateSprites();
		VDP_waitVSync();
	}
}

void PassiveLagTest()
{
	u16 frames = 0, seconds = 0, minutes = 0, hours = 0, framecnt = 1, bgcol = PAL2;
	u16 exit = 0, color = 1, loadvram = 1;
	u16 buttons, oldButtons = 0xffff, pressedButtons;
	u16 numbers[11], size, lsd, msd, pause = 0, circle = 0, cposx = 32, cposy = 17, solid, bars;

	while(!exit)
	{
		if(loadvram)
		{
			int i = 0;
			
			VDP_setPalette(PAL1, btw_pal);
			VDP_setPalette(PAL2, bluew_pal);
			VDP_setPalette(PAL3, redw_pal);

			numbers[0] = TILE_USERINDEX;
			size = sizeof(tiles_0) / 32;

			VDP_loadTileData(tiles_0, numbers[0], size, USE_DMA);
			numbers[1] = numbers[0] + size;
			VDP_loadTileData(tiles_1, numbers[1], size, USE_DMA);
			numbers[2] = numbers[1] + size;
			VDP_loadTileData(tiles_2, numbers[2], size, USE_DMA);
			numbers[3] = numbers[2] + size;
			VDP_loadTileData(tiles_3, numbers[3], size, USE_DMA);
			numbers[4] = numbers[3] + size;
			VDP_loadTileData(tiles_4, numbers[4], size, USE_DMA);
			numbers[5] = numbers[4] + size;
			VDP_loadTileData(tiles_5, numbers[5], size, USE_DMA);
			numbers[6] = numbers[5] + size;
			VDP_loadTileData(tiles_6, numbers[6], size, USE_DMA);
			numbers[7] = numbers[6] + size;
			VDP_loadTileData(tiles_7, numbers[7], size, USE_DMA);
			numbers[8] = numbers[7] + size;
			VDP_loadTileData(tiles_8, numbers[8], size, USE_DMA);
			numbers[9] = numbers[8] + size;
			VDP_loadTileData(tiles_9, numbers[9], size, USE_DMA);
			numbers[10] = numbers[9] + size;
			VDP_loadTileData(tiles_c, numbers[10], size, USE_DMA);

			circle = numbers[10] + size;
			size = sizeof(circle56_tiles) / 32;
			VDP_loadTileData(circle56_tiles, circle, size, USE_DMA);

			solid = circle + size;
			size = sizeof(solidw_tiles) / 32;
			VDP_loadTileData(solidw_tiles, solid, size, USE_DMA);

			VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);
			VDP_clearTileMapRect(BPLAN, 0, 0, 320 / 8, 224 / 8);

			VDP_fillTileMapRect(BPLAN, TILE_ATTR(PAL2, 0, 0, 0) + solid, 0, 0, 320 / 8, (pal_240 ? 240 : 224) / 8);
			
			// Load side bars
			bars = solid + size;
			size = sizeof(vbars_tiles) / 32;
			VDP_loadTileData(vbars_tiles, bars, size, USE_DMA);
			
			//Draw Side bars
			for(i = 0; i < 28; i++)
			{
				VDP_fillTileMapInc(BPLAN, TILE_ATTR_FULL(PAL0, 0, 0, 0, 0) + bars,    i*64, 1);
				VDP_fillTileMapInc(BPLAN, TILE_ATTR_FULL(PAL0, 0, 0, 1, 0) + bars, 39+i*64, 1);
			}

			VDP_drawTextBG(APLAN, "hours", TILE_ATTR(PAL1, 0, 0, 0), 4, 1);
			VDP_drawTextBG(APLAN, "minutes", TILE_ATTR(PAL1, 0, 0, 0), 13, 1);
			VDP_drawTextBG(APLAN, "seconds", TILE_ATTR(PAL1, 0, 0, 0), 22, 1);
			VDP_drawTextBG(APLAN, "frames", TILE_ATTR(PAL1, 0, 0, 0), 31, 1);

			// Draw Frame divisor numbers
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[1], 4, 10, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[2], 14, 10, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[3], 24, 10, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[4], 34, 10, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[5], 4, 18, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[6], 14, 18, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[7], 24, 18, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL0, 0, 0, 0) + numbers[8], 34, 18, 3, 5);

			// Draw counter separators
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[10], 10, 2, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[10], 19, 2, 3, 5);
			VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[10], 28, 2, 3, 5);

			//Draw initial circles
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 2, 9, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 12, 9, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 22, 9, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 32, 9, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 2, 17, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 12, 17, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 22, 17, 7, 7);
			VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, 32, 17, 7, 7);

			loadvram = 0;
		}

		if(framecnt > 8)
			framecnt = 1;

		if(!pause && color)
		{
			if(bgcol == PAL2)
			{
				bgcol = PAL3;
				VDP_setPaletteColor(0xE, 0x0);
			}
			else
			{
				bgcol = PAL2;
				VDP_setPaletteColor(0xE, 0x0fff);
			}
		}

		if(Detect_VDP_PAL())
		{
			if(frames > 49)
			{
				frames = 0;
				seconds++;
			}
		}
		else
		{
			if(frames > 59)
			{
				frames = 0;
				seconds++;
			}
		}

		if(seconds > 59)
		{
			seconds = 0;
			minutes++;
		}

		if(minutes > 59)
		{
			minutes = 0;
			hours++;
		}

		if(hours > 99)
			hours = 0;

		// Draw Hours
		lsd = hours % 10;
		msd = hours / 10;
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[msd], 4, 2, 3, 5);
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[lsd], 7, 2, 3, 5);

		// Draw Minutes
		lsd = minutes % 10;
		msd = minutes / 10;
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[msd], 13, 2, 3, 5);
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[lsd], 16, 2, 3, 5);

		// Draw Seconds
		lsd = seconds % 10;
		msd = seconds / 10;
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[msd], 22, 2, 3, 5);
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(PAL1, 0, 0, 0) + numbers[lsd], 25, 2, 3, 5);

		// Draw frames
		lsd = frames % 10;
		msd = frames / 10;
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(bgcol, 0, 0, 0) + numbers[msd], 31, 2, 3, 5);
		VDP_fillTileMapRectInc(APLAN, TILE_ATTR(bgcol, 0, 0, 0) + numbers[lsd], 34, 2, 3, 5);

		// Draw Frame divisor circles

		VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL3, 0, 0, 0) + circle, cposx, cposy, 7, 7);
		if(framecnt > 4)
		{
			cposx = framecnt - 4;
			cposy = 17;
		}
		else
		{
			cposx = framecnt;
			cposy = 9;
		}
		cposx = (cposx - 1) * 10 + 2;

		VDP_fillTileMapRectInc(BPLAN, TILE_ATTR(PAL2, 0, 0, 0) + circle, cposx, cposy, 7, 7);

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_LAG))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
			pause = !pause;

		if(pressedButtons & BUTTON_B && pause)
		{
			frames = hours = minutes = seconds = 0;
			framecnt = 1;
		}

		if(pressedButtons & BUTTON_C)
		{
			color = !color;
			if(!color)
			{
				VDP_setPalette(PAL1, btw_pal);
				VDP_setPalette(PAL2, btw_pal);
				VDP_setPalette(PAL3, bw_pal);
			}
			else
			{
				VDP_setPalette(PAL1, btw_pal);
				VDP_setPalette(PAL2, bluew_pal);
				VDP_setPalette(PAL3, redw_pal);
			}

			if(color)
				bgcol = PAL3;
			else
				bgcol = PAL2;
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(!pause)
		{
			frames++;
			framecnt++;
		}
		VDP_waitVSync();
	}
}

u16 ConvertToFrames(timecode * time)
{
	u16 frames = 0;

	if(!time)
		return frames;

	frames = time->frames;
	if(Detect_VDP_PAL())
		frames += time->seconds * 50;
	else
		frames += time->seconds * 60;
	frames += time->minutes * 3600;
	frames += time->hours * 216000;
	return frames;
}

void ConvertFromFrames(timecode * value, u16 Frames)
{
	if(!value)
		return;
	value->hours = Frames / 216000;
	Frames = Frames % 216000;
	value->minutes = Frames / 3600;
	Frames = Frames % 3600;
	if(Detect_VDP_PAL())
	{
		value->seconds = Frames / 50;
		value->frames = Frames % 50;
	}
	else
	{
		value->seconds = Frames / 60;
		value->frames = Frames % 60;
	}
}

void Alternate240p480i()
{
	u16 frames = 0, seconds = 0, minutes = 0, hours = 0;
	u16 exit = 0, loadvram = 1;
	u16 buttons, oldButtons = 0xffff, pressedButtons;
	u16 oldinterlaced, res = 0, current = 0, status = 0, changed = 1;
	u16 redraw = 1;
	timecode times[20];
	char str[10];

	oldinterlaced = VDP_Detect_Interlace();

	VDP_setScanMode(INTERLACED_NONE);

	while(!exit)
	{
		if(loadvram)
		{
			VDP_clearTileMapRect(BPLAN, 0, 0, 320 / 8, 224 / 8);
			VDP_clearTileMapRect(APLAN, 0, 0, 320 / 8, 224 / 8);

			VDP_setPalette(PAL0, palette_grey);
			VDP_setPalette(PAL1, palette_green);
			VDP_setPalette(PAL2, palette_red);

			VDP_drawText("Elapsed Timer: 00:00:00:00", 7, 3);
			loadvram = 0;
			changed = 1;
		}

		intToStr(hours, str, 2);
		VDP_drawText(str, 22, 3);
		intToStr(minutes, str, 2);
		VDP_drawText(str, 25, 3);
		intToStr(seconds, str, 2);
		VDP_drawText(str, 28, 3);
		intToStr(frames, str, 2);
		VDP_drawText(str, 31, 3);

		if(changed)
		{
			u16 i = 0;

			if(redraw)
			{
				i = 0;
				redraw = 0;
			}
			else
				i = current - 1;

			VDP_drawTextBG(APLAN, "Current Resolution:", TILE_ATTR(PAL1, 0, 0, 0), 7, 2);
			if(res == 0)
				VDP_drawTextBG(APLAN, "240p", TILE_ATTR(PAL1, 0, 0, 0), 27, 2);
			else
				VDP_drawTextBG(APLAN, "480i", TILE_ATTR(PAL1, 0, 0, 0), 27, 2);

			if(current)
			{
				for(i = 0; i < current; i++)
				{
					u8 len = 0;

					if(times[i].type == 0)
					{
						VDP_drawTextBG(APLAN, "Switched     :", TILE_ATTR(PAL1, 0, 0, 0), 1, i + 5);
						VDP_drawTextBG(APLAN, times[i].res == 0 ? "240p" : "480i", TILE_ATTR(PAL1, 0, 0, 0), 10, i + 5);
						len = 14;
					}
					else
					{
						VDP_drawTextBG(APLAN, "Viewed:", TILE_ATTR(PAL1, 0, 0, 0), 1, i + 5);
						len = 8;
					}

					VDP_drawText("00:00:00:00", 3 + len, i + 5);
					intToStr(times[i].hours, str, 2);
					VDP_drawText(str, 3 + len, i + 5);
					intToStr(times[i].minutes, str, 2);
					VDP_drawText(str, 6 + len, i + 5);
					intToStr(times[i].seconds, str, 2);
					VDP_drawText(str, 9 + len, i + 5);
					intToStr(times[i].frames, str, 2);
					VDP_drawText(str, 12 + len, i + 5);

					if(times[i].type != 0 && i >= 1 && i <= 19)
					{
						u16 framesA = 0, framesB = 0, res = 0;
						timecode len;

						framesB = ConvertToFrames(&times[i]);
						framesA = ConvertToFrames(&times[i - 1]);
						res = framesB - framesA;
						ConvertFromFrames(&len, res);

						VDP_drawTextBG(APLAN, "00:00:00:00", TILE_ATTR(PAL2, 0, 0, 0), 24, i + 5);
						intToStr(len.hours, str, 2);
						VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL2, 0, 0, 0), 24, i + 5);
						intToStr(len.minutes, str, 2);
						VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL2, 0, 0, 0), 27, i + 5);
						intToStr(len.seconds, str, 2);
						VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL2, 0, 0, 0), 30, i + 5);
						intToStr(len.frames, str, 2);
						VDP_drawTextBG(APLAN, str, TILE_ATTR(PAL2, 0, 0, 0), 33, i + 5);
					}
				}
			}
			changed = 0;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;

		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_ALTERNATE))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
		{
			if(current <= 19)
				current++;
			else
			{
				current = 1;
				loadvram = 1;
			}

			times[current - 1].frames = frames;
			times[current - 1].minutes = minutes;
			times[current - 1].seconds = seconds;
			times[current - 1].hours = hours;

			status++;
			if(status == 1)
			{
				times[current - 1].type = 0;
				res = !res;
				times[current - 1].res = res;
				if(!res)
					VDP_setScanMode(INTERLACED_NONE);
				else
					VDP_setScanMode(INTERLACED_MODE1);
			}
			if(status == 2)
			{
				times[current - 1].type = 1;
				times[current - 1].res = res;
				status = 0;
			}
			changed = 1;
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		frames++;

		if(Detect_VDP_PAL())
		{
			if(frames > 49)
			{
				frames = 0;
				seconds++;
			}
		}
		else
		{
			if(frames > 59)
			{
				frames = 0;
				seconds++;
			}
		}

		if(seconds > 59)
		{
			seconds = 0;
			minutes++;
		}

		if(minutes > 59)
		{
			minutes = 0;
			hours++;
		}

		if(hours > 99)
			hours = 0;

		VDP_waitVSync();
	}

	if(oldinterlaced)
		VDP_setScanMode(INTERLACED_MODE1);
	else
		VDP_setScanMode(INTERLACED_NONE);

	return;
}

void AudioSyncTest()
{
	u16 loadvram = 1, exit = 0, cycle = 0;
	u16 size, tiles, i, sprite = 0, x = 160, y = 160;
	u16 buttons, pressedButtons, oldButtons = 0xffff;
	u16 black_pal[16];
	s16 acc = 1, status = -1;

	PSG_init();
	PSG_setFrequency(0, 1000);

	for(i = 0; i < 16; i++)
		black_pal[i] = 0x0;

	while(!exit)
	{
		if(loadvram)
		{
			VDP_setPalette(PAL0, black_pal);
			VDP_setPaletteColor(17, 0x0ddd);
			VDP_setPaletteColor(34, 0x0ddd);

			tiles = TILE_USERINDEX;
			size = sizeof(color_tiles) / 32;
			VDP_loadTileData(color_tiles, tiles, size, USE_DMA);

			sprite = tiles + size;
			size = sizeof(size1led_t) / 32;
			VDP_loadTileData(size1led_t, sprite, size, USE_DMA);

			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 5) + tiles, 0, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 4) + tiles, 4, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 3) + tiles, 8, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 2) + tiles, 12, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 1) + tiles, 16, 6, 4, 2);

			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 1) + tiles, 20, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 2) + tiles, 24, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 3) + tiles, 28, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 4) + tiles, 32, 6, 4, 2);
			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL0, 1, 0, 0, 5) + tiles, 36, 6, 4, 2);

			VDP_setSprite(0, x, y, SPRITE_SIZE(1, 1), TILE_ATTR(PAL1, 0, 0, 0) + sprite, 0);
			VDP_setSpritePosition(0, x, y);
			VDP_updateSprites();	

			VDP_fillTileMapRect(APLAN, TILE_ATTR_FULL(PAL2, 1, 0, 0, 1) + tiles, 0, 20, 40, 1);

			loadvram = 0;
		}

		buttons = JOY_readJoypad(JOY_1);
		pressedButtons = buttons & ~oldButtons;
		oldButtons = buttons;
		
		if(CheckHelpAndVO(&buttons, &pressedButtons, HELP_AUDIOSYNC))
			loadvram = 1;

		if(pressedButtons & BUTTON_A)
		{
			cycle = !cycle;
			if(!cycle)
				status = 121;
			else
				y = 160;
		}

		if(cycle == 1 && status == -1)
		{
			status = 0;
			acc = -1;
		}

		if(pressedButtons & BUTTON_START)
			exit = 1;

		if(status > -1)
		{
			status++;
			if(status <= 120)
			{
				y += acc;
				VDP_setSpritePosition(0, x, y);
				VDP_updateSprites();
			}
		}

		if(status >= 20 && status <= 120)
		{
			switch (status)
			{
			case 20:
				black_pal[7] = 0x0ddd;
				break;
			case 40:
				black_pal[6] = 0x0ddd;
				break;
			case 60:
				acc = 1;
				black_pal[5] = 0x0ddd;
				break;
			case 80:
				black_pal[4] = 0x0ddd;
				break;
			case 100:
				black_pal[3] = 0x0ddd;
				break;
			case 120:
				black_pal[2] = 0x0ddd;
				break;
			}

			VDP_setPalette(PAL0, black_pal);
		}
		
		if(status == 120)
		{
			PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
			black_pal[0] = 0xFFF;
			VDP_setPalette(PAL0, black_pal);
		}

		if(status == 122)
		{
			for(i = 0; i < 8; i++)
				black_pal[i] = 0x0;

			PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
			VDP_setPalette(PAL0, black_pal);
			status = -1;
		}
		
		VDP_waitVSync();
	}
	VDP_resetSprites();
	VDP_updateSprites();
	PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
}

/*
void VAPanelScrollTest()
{    
  u16 size;
  u16 exit = 0;
  u16 buttons, oldButtons = 0xffff, pressedButtons;
  int x = 0, speed = 1, acc = -1, pause = 0;
  char str[10];
  
  VDP_setPalette(PAL0, VAPanel_pal);    
  VDP_setPalette(PAL1, palette_grey);

  size = sizeof(VAPanel_tiles) / 32; 
  VDP_loadTileData(VAPanel_tiles, TILE_USERINDEX, size, USE_DMA);       

  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 0, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 64/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 128/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 192/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 256/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 320/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 384/8, 0, 64/8, 136/8);                            
  VDP_setMyTileMapRect(APLAN, VAPanel_map, TILE_USERINDEX, 448/8, 0, 64/8, 136/8);                            
    
  VDP_drawTextBG(BPLAN, "Scroll Speed:", TILE_ATTR(PAL1, 0, 0, 0), 20, 26);
  while(!exit)
  {  
    buttons = JOY_readJoypad(JOY_1);
    pressedButtons = buttons & ~oldButtons;
    oldButtons = buttons;
        
    if (pressedButtons & BUTTON_START)
      exit = 1;

    if (pressedButtons & BUTTON_UP)
      speed++;

    if (pressedButtons & BUTTON_DOWN)
      speed--;

    if(speed > 10)        
      speed = 10;          
    
    if(speed < 0)        
      speed = 0;          

    if (pressedButtons & BUTTON_A)
      pause = !pause;

    if (pressedButtons & BUTTON_B)
      acc *= -1;

    if(!pause)
      x += acc*speed;

    if(x >= 512)
      x = x % 512;

    if(x <= -512)
      x = x % -512;

    VDP_drawTextBG(BPLAN, "   ", TILE_ATTR(PAL1, 0, 0, 0), 34, 26);
    intToStr(speed, str, 1);    
    VDP_drawTextBG(BPLAN, str, TILE_ATTR(PAL1, 0, 0, 0), 34, 26);
    
    VDP_setHorizontalScroll(PLAN_A, x);            
    VDP_waitVSync();
  }
  VDP_setHorizontalScroll(PLAN_A, 0);  
}
*/
