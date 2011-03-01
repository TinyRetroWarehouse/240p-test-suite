/* 
 * 240p Test Suite
 * Copyright (C)2011 Artemio Urbina
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 240p Test Suite; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA
 */

#include <kos.h>
#include <stdlib.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>

#include "image.h"
#include "font.h"
#include "vmodes.h"
#include "vmu.h"

#include "controller.h"
#include "tests.h"
#include "patterns.h"

#include "help.h"

/* romdisk */
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
KOS_INIT_FLAGS(INIT_DEFAULT);

ImagePtr		scanlines = NULL;

static inline void DrawScanlines()
{
	if(vmode == FAKE_640_SL && scanlines)
		DrawImage(scanlines);
}

void TestPatternsMenu();
void DrawCredits();

int main(void)
{
	int 				done = 0, sel = 1, joycnt = 0, restart = 1;
	uint16			oldbuttons, pressed;		
	ImagePtr		title;
	controller	*st;

	InitVideo();
	
	while(restart)	// This loop is used to Change resolutions
	{
		LoadFont();
		title = LoadImage("/rd/title.png", 1);		
		if(!scanlines && vmode == FAKE_640_SL)
		{
			scanlines = LoadImage("/rd/scanlines.png", 0);
			scanlines->layer = 5.0;
			scanlines->alpha = 0.7f; 
			scanlines->scale = 0;
			CalculateUV(0, 0, 640, 480, scanlines);
		}
		
		oldbuttons = InitController(0);
 		while(!done) // Main loop
		{
			char		res[40];
			float 	r = 1.0f;
			float 	g = 1.0f;
			float 	b = 1.0f;
			int   	c = 1;				
			float 	x = 40.0f;
			float 	y = 55.0f;
					
			pvr_wait_ready();
			pvr_scene_begin();
			pvr_list_begin(PVR_LIST_TR_POLY);
	
			DrawImage(title);
			
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Test Patterns"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Drop Shadow Test"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Striped Sprite Test"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Lag Test"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Scroll Test"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Horizontal Stripes"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Checkerboard"); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Sound Test"); y += fh; c++;
	
			switch(vmode)
			{
				case NATIVE_320:
					sprintf(res, "Video: 240p");
					break;
				case NATIVE_640:
					sprintf(res, "Video: 240p in 480i");
					break;
				case FAKE_640:
					sprintf(res, "Video: Fake 480i");
					break;
				case NATIVE_640_FS:
					sprintf(res, "Video: 480i");
					break;
				case FAKE_640_SL:
					sprintf(res, "Video: 480p %s scanlines %0.0f%%", scanlines->y == 0 ? "even" : "odd", (double)scanlines->alpha*100);
					break;
			}
			DrawStringS(x, y, r, sel == c ? 0 : g, sel == c ? 0 : b, res); y += fh; c++;
			DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Help"); y += fh; c++;
			DrawStringS(x, y + fh, r, sel == c ? 0 : g, sel == c ? 0 : b, "Credits"); y += fh; 
	
			switch(vcable)
			{
				case CT_RGB:
					DrawStringS(265.0f, 225.0f, 0, g,	b, "RGB");
					break;
				case CT_VGA:
					DrawStringS(265.0f, 225.0f, 0, g,	b, "VGA");
					break;
				case CT_COMPOSITE:
					DrawStringS(215.0f, 225.0f, 0, g,	b, "Composite");
					break;
			}
			
			DrawScanlines();
					
			pvr_list_finish();				
			pvr_scene_finish();
	
			st = ReadController(0);
			if(st)
			{
				pressed = st->buttons & ~oldbuttons;
				oldbuttons = st->buttons;
#ifdef SERIAL
				if (st->buttons & CONT_START && st->buttons & CONT_B)
				{
					updateVMU(" Goodbye ", " m(_ _)m ", 1);
					done =	1;
					restart = 0;
				}
#endif
	
				if (pressed & CONT_X)
				{
					if(scanlines)
					{
						if(scanlines->y == 0)
							scanlines->y = -1;
						else
							scanlines->y = 0;
					}
				}
	
				if (pressed & CONT_DPAD_RIGHT && st->buttons & CONT_Y)
				{
					if(scanlines)
					{
						scanlines->alpha += 0.05f; 
						if(scanlines->alpha > 1.0f)
							scanlines->alpha = 1.0f;
					}
				}
	
				if (pressed & CONT_DPAD_LEFT && st->buttons & CONT_Y)
				{
					if(scanlines)
					{
						scanlines->alpha -= 0.05f; 
						if(scanlines->alpha < 0.0f)
								scanlines->alpha = 0.0f;
					}
				}
	
				if (pressed & CONT_DPAD_UP)
				{
					sel --;
					if(sel < 1)
						sel = c;				
				}
				
				if (pressed & CONT_DPAD_DOWN)
				{
					sel ++;
					if(sel > c)
						sel = 1;				
				}
				
				if(st->joyy != 0)
				{
					if(++joycnt > 5)
					{
						if(st->joyy > 0)
							sel ++;
						if(st->joyy < 0)
							sel --;
			
						if(sel < 1)
							sel = c;
						if(sel > c)
							sel = 1;					
						joycnt = 0;
					}
				}
				else
					joycnt = 0;
				
				if (pressed & CONT_A)
				{
					switch(sel)
					{
						case 1:
							TestPatternsMenu();
							break;
						case 2:					
							DropShadowTest();
							break;
						case 3:
							StripedSpriteTest();
							break;
						case 4:
							LagTest();
							break;
						case 5:
							ScrollTest();
							break;
						case 6:
							DrawStripes();
							break;
						case 7:
							DrawCheckBoard();
							break;
						case 8:
							SoundTest();
							break;
						case 9:
							ChangeResolution();
							done = 1;		// Reset PVR, but go on
							break;
						case 10:
							HelpWindow(GENERALHELP, NULL, 1);
							break;
						case 11:
							DrawCredits();
							break;
					} 												
					updateVMU("240p Test", "", 1);				
					oldbuttons = InitController(0);
				}
			}
			updateVMU("240p Test", "", 0);
		}
	
		FreeImage(&scanlines);		
		FreeImage(&title);		
		ReleaseFont();
	}
#ifndef SERIAL
	arch_menu();
#endif
	return 0;
}

void TestPatternsMenu()
{
	int 				done = 0, sel = 1, joycnt = 0;
	uint16			oldbuttons, pressed;		
	ImagePtr		title;
	controller	*st;

	oldbuttons = InitController(0);
	title = LoadImage("/rd/title.png", 1);		
 	while(!done) 
	{		
		float 	r = 1.0f;
		float 	g = 1.0f;
		float 	b = 1.0f;
		int 	c = 1;				
		float 	x = 40.0f;
		float 	y = 55.0f;
				
		pvr_wait_ready();
		pvr_scene_begin();
		pvr_list_begin(PVR_LIST_TR_POLY);

		DrawImage(title);

		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Pluge"); y += fh; c++;
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Color Bars"); y += fh; c++;
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Color Bars with Gray Scale"); y += fh; c++;
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Grid"); y += fh; c++;
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Linearity"); y += fh; c++;		
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "Gray Ramp"); y += fh; c++;		
		DrawStringS(x, y, r, sel == c ? 0 : g,	sel == c ? 0 : b, "White Screen"); y += fh; c++;				
		DrawStringS(x, y + fh, r, sel == c ? 0 : g, sel == c ? 0 : b, "Back to Main Menu"); y += fh; 

		switch(vcable)
		{
			case CT_RGB:
				DrawStringS(265.0f, 225.0f, 0, g,	b, "RGB");
				break;
			case CT_VGA:
				DrawStringS(265.0f, 225.0f, 0, g,	b, "VGA");
				break;
			case CT_COMPOSITE:
				DrawStringS(215.0f, 225.0f, 0, g,	b, "Composite");
				break;
		}
		
		DrawScanlines();
				
		pvr_list_finish();				
		pvr_scene_finish();

		st = ReadController(0);
		if(st)
		{
			pressed = st->buttons & ~oldbuttons;
			oldbuttons = st->buttons;

			if (pressed & CONT_DPAD_RIGHT && st->buttons & CONT_Y)
			{
				if(scanlines)
				{
					scanlines->alpha += 0.1f; 
					if(scanlines->alpha > 1.0f)
						scanlines->alpha = 1.0f;
				}
			}

			if (pressed & CONT_DPAD_LEFT && st->buttons & CONT_Y)
			{
				if(scanlines)
				{
					scanlines->alpha -= 0.1f; 
					if(scanlines->alpha < 0.0f)
						scanlines->alpha = 0.0f;
				}
			}

			if (pressed & CONT_DPAD_UP)
			{
				sel --;
				if(sel < 1)
					sel = c;				
			}
		
			if (pressed & CONT_DPAD_DOWN)
			{
				sel ++;
				if(sel > c)
					sel = 1;				
			}
		
			if(st->joyy != 0)
			{
				if(++joycnt > 5)
				{
					if(st->joyy > 0)
						sel ++;
					if(st->joyy < 0)
						sel --;
		
					if(sel < 1)
						sel = c;
					if(sel > c)
						sel = 1;					
					joycnt = 0;
				}
			}
			else
				joycnt = 0;
		
			if (pressed & CONT_B)			
				done = 1;			
		
			if (pressed & CONT_A)
			{
				switch(sel)
				{
					case 1:
						DrawPluge();
						break;
					case 2:
						DrawColorBars();
						break;
					case 3:
						Draw601ColorBars();
						break;
					case 4:
						DrawGrid();
						break;
					case 5:
						DrawLinearity();
						break;					
					case 6:
						DrawGrayRamp();
						break;
					case 7:
						DrawWhiteScreen();
						break;
					case 8:
						done = 1;
						break;
				} 												
				oldbuttons = InitController(0);
				updateVMU("Patterns", "", 1);
			}			
		}

		updateVMU("Patterns", "", 0);
	}
	
	FreeImage(&title);			
	return;
}

void DrawCredits()
{
	int 				done = 0;
	uint16			oldbuttons, pressed;		
	ImagePtr		back;
	controller	*st;

	oldbuttons = InitController(0);

	back = LoadImage("/rd/back.png", 1);
		
	updateVMU("	Credits", "", 1);
	while(!done) 
	{
		int x = 30, y = 60;
		pvr_wait_ready();

		st = ReadController(0);
		if(st)
		{
			pressed = st->buttons & ~oldbuttons;
			oldbuttons = st->buttons;
			
			if (pressed & CONT_START)
				done =	1;				
		}

		pvr_scene_begin();

		pvr_list_begin(PVR_LIST_TR_POLY);
		DrawImage(back);

		DrawStringS(x, y, 0.0, 1.0, 0.0, "Code and Patterns:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "Artemio Urbina"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "SDK:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "KallistiOS"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "SDK Assistance:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "BlueCrab"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "Menu Pixel Art:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "Asher"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "Advisor:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "Fudoh"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "Toolchain built with:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "https://github.com/losinggeneration/buildcross"); y += fh; 
		DrawStringS(x, y, 0.0, 1.0, 0.0, "Info on using this suite:"); y += fh; 
		DrawStringS(x+5, y, 1.0, 1.0, 1.0, "http://junkerhq.net/xrgb/"); y += fh; 

		DrawStringS(220, 58, 1.0, 1.0, 1.0, "Ver. 1.07"); y += fh; 

		DrawScanlines();
		pvr_list_finish();				

		pvr_scene_finish();
	}
	FreeImage(&back);
}
