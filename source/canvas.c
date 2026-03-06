#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <gccore.h>

#include "canvas.h"

u32 blend_pair_colors(u32 color1, u32 color2) {
	u8 cb = (((color1 >> 16) & 0xFF) + ((color2 >> 16 & 0xFF))) / 2;
	u8 cr = ((color1 & 0xFF) + (color2 & 0xFF)) / 2;
	return (color1 & 0xFF000000) | (cb << 16) | (color2 & 0xFF00) | (cr);
}

u32 rgb_to_ypbpr(u32 rgb) {
	u8 r = (rgb >> 16) & 0xFF;
	u8 g = (rgb >> 8) & 0xFF;
	u8 b = (rgb) & 0xFF;

	u8 y = ((76 * r + 150 * g +  29 * b) >> 8);
  u8 cb = ((-43 * r -  85 * g + 128 * b) >> 8) + 128;
  u8 cr = ((128 * r - 107 * g -  21 * b) >> 8) + 128;
	return (y << 24) | (cb << 16) | (y << 8) | (cr);
}

// Base alpha is ignored
u32 blend_rgba_to_rgb(u32 rgb, u32 rgba) {
	u8 alpha = rgba & 0xFF;
	if (alpha == 0xFF) return rgba >> 8;
	if (alpha == 0) return rgb;
	u8 r = (alpha * (rgba >> 24) + (0xFF - alpha) * (rgb >> 16)) / 0xFF;
	u8 g = (alpha * ((rgba >> 16) & 0xFF) + (0xFF - alpha) * ((rgb >> 8) & 0xFF)) / 0xFF;
	u8 b = (alpha * ((rgba >> 8) & 0xFF) + (0xFF - alpha) * (rgb & 0xFF)) / 0xFF;
	return (r << 16) | (g << 8) | b;
}

Canvas* Canvas_Init(GXRModeObj * rmode) {
	Canvas* canvas = malloc(sizeof(Canvas));
	canvas->rmode = rmode;
	canvas->frameBufferUsed = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	canvas->frameBufferUnused = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	canvas->rgbMap = memalign(32, sizeof(u32) * rmode->fbWidth * rmode->xfbHeight);
	canvas->globalAlpha = 0xFF;
	return canvas;
}

void Canvas_Clear(Canvas *canvas) {
	memset(canvas->rgbMap, 0, sizeof(u32) * canvas->rmode->xfbHeight * canvas->rmode->fbWidth);
}

void Canvas_Render(Canvas *canvas) {
	u32* fb_ptr = canvas->frameBufferUnused;
	u32* rgb_ptr = canvas->rgbMap;
	for (int ii = 0; ii < canvas->rmode->fbWidth * canvas->rmode->xfbHeight / 2; ii++)
		*fb_ptr++ = blend_pair_colors(rgb_to_ypbpr(*rgb_ptr++), rgb_to_ypbpr(*rgb_ptr++));
}

void Canvas_Swap(Canvas *canvas) {
	u32 *tmp = canvas->frameBufferUsed;
	canvas->frameBufferUsed = canvas->frameBufferUnused;
	canvas->frameBufferUnused = tmp;

	VIDEO_SetNextFramebuffer(canvas->frameBufferUsed);
}

void Canvas_Draw_RGBA_Image(Canvas* canvas, const uint8_t *image, int x, int y, int w, int h) {
	u32 *img = (u32*) image;
	int rgba;
	for (int jj = 0; jj < h; jj++) {
		if (jj + y < 0 || jj + y >= canvas->rmode->xfbHeight) continue;
		for (int ii = 0; ii < w; ii++) {
			if (ii + x < 0 || ii + x >= canvas->rmode->fbWidth) continue;
			rgba = img[ii + jj * w];
			canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth] = blend_rgba_to_rgb(canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth], (rgba & 0xFFFFFF00) | (rgba & 0xFF * canvas->globalAlpha / 0xFF));
		}
	}
}

void Canvas_Draw_RGBA_Image_Culled(Canvas* canvas, const uint8_t *image, int x, int y, int w, int h) {
	u32 *img = (u32*) image;
	int rgba;
	for (int jj = 0; jj < h; jj++) {
		if (jj + y < 0 || jj + y >= canvas->rmode->xfbHeight) continue;
		for (int ii = 0; ii < w; ii++) {
			if (ii + x < 0 || ii + x >= canvas->rmode->fbWidth) continue;
			if (canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth]) continue;
			rgba = img[ii + jj * w];
			canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth] = blend_rgba_to_rgb(canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth], (rgba & 0xFFFFFF00) | (rgba & 0xFF * canvas->globalAlpha / 0xFF));
		}
	}
}

void Canvas_Draw_RGBA_Image_Overlay_Loop(Canvas *canvas, const uint8_t *image, const uint8_t *overlay, int x, int y, int w, int h, int ox, int oy, int ow, int oh) {
	u32 *img = (u32*) image, *ovl = (u32*) overlay;
	int rgba, oii, ojj;
	for (int jj = 0; jj < h; jj++) {
		if (jj + y < 0 || jj + y >= canvas->rmode->xfbHeight) continue;
		for (int ii = 0; ii < w; ii++) {
			if (ii + x < 0 || ii + x >= canvas->rmode->fbWidth) continue;
			rgba = img[ii + jj * w];
			// Not black
			if (rgba >> 8) {
				oii = ii + ox;
				ojj = jj + oy;
				while (oii >= ow) oii -= ow;
				while (ojj >= oh) ojj -= oh;
				rgba = ovl[oii + ojj * ow];
			}
			rgba = (rgba & 0xFFFFFF00) | (rgba & 0xFF * canvas->globalAlpha / 0xFF);
			canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth] = blend_rgba_to_rgb(canvas->rgbMap[ii + x + (jj + y) * canvas->rmode->fbWidth], rgba);
		}
	}
}

void Canvas_Prerender_RGBA_Image_Overlay_Loop(Canvas *canvas, uint8_t *result, const uint8_t *image, const uint8_t *overlay, int w, int h, int ox, int oy, int ow, int oh) {
	u32 *res = (u32*) result, *img = (u32*) image, *ovl = (u32*) overlay;
	int index, oii, ojj;
	for (int jj = 0; jj < h; jj++) {
		for (int ii = 0; ii < w; ii++) {
			index = ii + jj * w;
			// Not black
			if (img[index] >> 8) {
				oii = ii + ox;
				ojj = jj + oy;
				while (oii >= ow) oii -= ow;
				while (ojj >= oh) ojj -= oh;
				res[index] = ovl[oii + ojj * ow];
			} else res[index] = img[index];
		}
	}
}