/* 
 * 240p Test Suite for Nintendo 64
 * Copyright (C)2016 Artemio Urbina
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

#include "image.h"
#include "utils.h"

// RSP local memory
#define DMEM 0xA4000000
#define IMEM 0xA4001000

// RSP DMA registers
#define DMA_LADDR (*(volatile uint32_t *)0xA4040000)
#define DMA_RADDR (*(volatile uint32_t *)0xA4040004)
#define DMA_RDLEN (*(volatile uint32_t *)0xA4040008)
#define DMA_WRLEN (*(volatile uint32_t *)0xA404000C)
#define DMA_FULL  (*(volatile uint32_t *)0xA4040014)
#define DMA_BUSY  (*(volatile uint32_t *)0xA4040018)

#define IMEM_SIZE 4000

sprite_t *LoadImage(char *name)
{
	int fp = 0;
	sprite_t *image = NULL;

	fp = dfs_open(name);
	if(fp == DFS_ESUCCESS)
		return NULL;
#ifdef USE_N64MEM
    image = n64_malloc(dfs_size(fp));
#else
	image = malloc(dfs_size(fp));
#endif
	if(!image)
		return NULL;
    if(dfs_read(image, 1, dfs_size(fp), fp) < 0)
	{
#ifdef USE_N64MEM
		n64_free(image);
#else
		free(image);
#endif
		return NULL;
	}
    dfs_close(fp);
	return image;
}

void FreeImage(sprite_t **image)
{
	if(*image)
	{
#ifdef USE_N64MEM
		n64_free(*image);
#else
		free(*image);
#endif
		*image = NULL;
	}
}

void SoftDrawImage(int x, int y, sprite_t *image)
{
	if(!image)
		return;
		
	graphics_draw_sprite_trans(__dc, x, y, image);
}

void SoftDrawImageSolid(int x, int y, sprite_t *image)
{
	if(!image)
		return;
		
	graphics_draw_sprite(__dc, x, y, image);
}

void rdp_fill_start()
{
	rdp_sync(SYNC_PIPE);
	rdp_set_default_clipping();
	rdp_enable_primitive_fill();
	rdp_attach_display(__dc);
}

void rdp_texture_start()
{
	rdp_sync(SYNC_PIPE);
	rdp_set_default_clipping();
	rdp_enable_texture_copy();
	rdp_attach_display(__dc);
}

void rdp_rectangle(int x0, int y0, int x1, int y1, int r, int g, int b)
{
	rdp_set_primitive_color(graphics_make_color(r, g, b, 0xff));
	rdp_draw_filled_rectangle(x0, y0, x1, y1);
}

void rdp_DrawImage(int x, int y, sprite_t *image)
{	
	if(!image)
		return;
		
	if(bD == 2 && image->width < 20 && image->height < 20)
	{
		rdp_sync(SYNC_PIPE);
		rdp_load_texture(0, 0, MIRROR_DISABLED, image);
		rdp_draw_sprite(0, x, y);
	}
	else
		SoftDrawImage(x, y, image);
}

void rdp_end()
{
	rdp_detach_display();
}

void rdp_FillScreenWithTexture(sprite_t *image)
{
	rdp_sync(SYNC_PIPE);
	rdp_load_texture(0, 0, MIRROR_DISABLED, image);
	rdp_draw_textured_rectangle (0, 0, 0, dW, dH);
}

void rdp_FillScreenWithTextureXY(int x, int y, sprite_t *image)
{
	rdp_sync(SYNC_PIPE);
	rdp_load_texture(0, 0, MIRROR_DISABLED, image);
	rdp_draw_textured_rectangle (0, x, y, dW, dH);
}

void ClearScreen()
{
	rdp_fill_start();
	rdp_rectangle(0, 0, dW, dH, 0x0, 0x0, 0x0);
	rdp_end();
}

// x must be a multiple of four
void drawImageDMA(int x, int y, sprite_t *image) 
{ 
    unsigned char*	target; 
	int32_t 		t_rowsize = 0, chunk = 0, width = 0;
	int32_t 		height = 0, s_rowsize = 0;
	int32_t 		start = 0, end = 0, sx_start = 0, sy_start = 0;
	
	//SoftDrawImage(x, y, image);
	//return;
	
	if(!image || !__dc)
		return;
		
	width = image->width;
	height = image->height;
	
	if(width + x > dW)
		width = dW - x;
	if(height + y > dH)
		height = dH - y;
	
	if(x < 0)
	{
		width += x;
		sx_start = -1*x*bD;
		x = 0;
	}
	
	if(y < 0)
	{
		height += y;
		sy_start = -1*y;
		y = 0;
	}
	
	if(width <= 0 || height <= 0)
		return;
		
	target = __safe_buffer[(__dc)-1]; 	
	chunk = width*bD;
	s_rowsize = image->width*bD;
	t_rowsize = dW*bD;
	
	start = y;
	end = y + height;
	
	data_cache_hit_writeback_invalidate(target+t_rowsize*(end-1), t_rowsize*((end-start)/10));
	for(uint32_t line = start; line < end; line ++)
	{
		/* DMA to DMEM */
		while (DMA_BUSY) ;
		DMA_LADDR = 0x0000;
		DMA_RADDR = (uint32_t)image->data+s_rowsize*(line-start+sy_start)+sx_start;
		DMA_RDLEN = chunk;

		/* DMA to frame buffer */
		while (DMA_FULL) ;
		DMA_LADDR = 0x0000;
		DMA_RADDR = (uint32_t)target+t_rowsize*line+x*bD;
		DMA_WRLEN = chunk;
	}
	data_cache_hit_invalidate(target+t_rowsize*(end-1), t_rowsize*((end-start)/10));	
}

// x must be a multiple of four
void drawScreenBufferDMA(int x, int y) 
{ 
    unsigned char*	target; 
	int32_t 		t_rowsize = 0, chunk = 0, width = 0;
	int32_t 		height = 0, s_rowsize = 0;
	int32_t 		start = 0, end = 0, sx_start = 0, sy_start = 0;
	
	if(!__screen_buffer || !__dc)
		return;
		
	width = dW;
	height = dH;
	

	if(width + x > dW)
		width = dW - x;
	if(height + y > dH)
		height = dH - y;
	
	if(x < 0)
	{
		width += x;
		sx_start = -1*x*bD;
		x = 0;
	}
	
	if(y < 0)
	{
		height += y;
		sy_start = -1*y;
		y = 0;
	}
	
	if(width <= 0 || height <= 0)
		return;
	
	target = __safe_buffer[(__dc)-1]; 	
	chunk = width*bD;
	s_rowsize = dW*bD;
	t_rowsize = dW*bD;
	
	start = y;
	end = y + height;
	if(end > dH)
		end = dH;
	
	data_cache_hit_writeback_invalidate(target+t_rowsize*(end-1), t_rowsize*((end-start)/10));
	for(uint32_t line = start; line < end; line ++)
	{
		/* DMA to DMEM */
		while (DMA_BUSY) ;
		DMA_LADDR = 0x0000;
		DMA_RADDR = (uint32_t)__screen_buffer+s_rowsize*(line-start+sy_start)+sx_start;
		DMA_RDLEN = chunk;

		/* DMA to frame buffer */
		while (DMA_FULL) ;
		DMA_LADDR = 0x0000;
		DMA_RADDR = (uint32_t)target+t_rowsize*line+x*bD;
		DMA_WRLEN = chunk;
	}
	data_cache_hit_invalidate(target+t_rowsize*(end-1), t_rowsize*((end-start)/10));	
}


void drawPatchBackgroundFromCapture(int x, int y, sprite_t *sprite) 
{ 
    unsigned char*	target, *src; 
	int 			width = 0, height = 0;
	
	if(!sprite || !__screen_buffer)
		return;
		
	width = sprite->width+2;
	height = sprite->height+2;
	
	x = x - 1;
	y = y - 1;
	
	if(x < 0)
		x = 0;
	if(x+width > dW)
		x = dW - width;
	if(y < 0)
		y = 0;
	if(y+height > dH)
		y = dH - height;
		
	src = __screen_buffer + y*dW*bD + x*bD; 
    target = __safe_buffer[(__dc)-1] + y*dW*bD + x*bD; 

    while (height--) 
    { 
#ifdef USE_N64MEM
		n64_memcpy(target, src, width*bD); 
#else
		memcpy(target, src, width*bD); 
#endif
		target += dW*bD; 
		src += dW*bD; 
    } 
} 

void drawPatchBackground(int x, int y, sprite_t *sprite, sprite_t *backgd) 
{ 
    unsigned char*	target, *src; 
	int 	size = 0, width = 0, height = 0;
	
	if(!sprite || !backgd)
		return;
		
	width = sprite->width+2;
	height = sprite->height+2;
	
	x = x - 1;
	y = y - 1;
	
	if(x < 0)
		x = 0;
	if(x+width > dW)
		x = dW - width;
	if(y < 0)
		y = 0;
	if(y+height > dH)
		y = dH - height;
 
	size = bD;
		
	src = (unsigned char*)backgd->data + y*backgd->width*size + x*size;
    target = __safe_buffer[(__dc)-1] + y*dW*size + x*size; 

    while (height--) 
    { 
#ifdef USE_N64MEM
		n64_memcpy(target, src, width*size); 
#else
		memcpy(target, src, width*size); 
#endif
		target += dW*size; 
		src += backgd->width*size; 
    } 
} 
