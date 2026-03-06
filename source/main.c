#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <mp3player.h>
#include <ogc/lwp_watchdog.h>

#include "canvas.h"
#include "prophecy.h"

#include "delta_mp3.h"
#include "overlay_raw.h"
#include "wii_raw.h"

#include "prophecy-1_raw.h"

static Canvas *canvas = NULL;
static GXRModeObj* rmode = NULL;

void crash() {
	volatile unsigned int *bad_pointer = (unsigned int*)0;
	*bad_pointer = 69420;
}

int main(int argc, char** argv) {
	VIDEO_Init();
	WPAD_Init();

	ASND_Init();
	MP3Player_Init();

	rmode = VIDEO_GetPreferredMode(NULL);
	canvas = Canvas_Init(rmode);
	console_init(canvas->frameBufferUsed, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
	PROPH_Init();

	VIDEO_Configure(rmode);
	Canvas_Swap(canvas);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	int phase = 0;
	u32 elapsed, wait, swap;
	u64 last = gettime();

	float counter = 0, ox = 0, oy = 0;
	uint8_t *overlayedImage = malloc(sizeof(uint8_t) * 293 * 313 * 4);
	Prophecy *prophecy = PROPH_Get();
	int off_x = (rmode->fbWidth - 293) / 2, off_y = (rmode->xfbHeight - 313) / 2 + 20;
	int off_y_prophecy = rmode->xfbHeight / 10;
	Canvas_Clear(canvas);
	while (SYS_MainLoop()) {
		elapsed = diff_msec(last, gettime());
		last = gettime();

		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if (pressed & WPAD_BUTTON_HOME) exit(0);

		if (phase == 0) {
			if (pressed) {
				MP3Player_PlayBuffer(delta_mp3, delta_mp3_size, NULL);
				phase = 1;
				//wait = 25417;
				wait = 25217;
			}
		} else if (phase == 1) {
			Canvas_Draw_RGBA_Image(canvas, wii_raw, off_x, off_y, 293, 313);
			if (wait < elapsed) {
				phase = 2;
				wait = 221583;
				swap = 221583 / PROPHECY_NUM;
			} else wait -= elapsed;
		} else {
			counter += 0.05;
			ox += 1;
			oy += 1;
			if (ox >= 256) ox -= 256;
			if (oy >= 256) oy -= 256;
			int y = sinf(counter) * 20;
			float alpha_factor = (sinf(counter * 0.7 + 0.1) + 1.0) / 10.0 + 0.8;
			Canvas_Clear(canvas);
			Canvas_Prerender_RGBA_Image_Overlay_Loop(canvas, overlayedImage, wii_raw, overlay_raw, 293, 313, ox, oy, 256, 256);
			canvas->globalAlpha = (u8) (255 * alpha_factor);
			Canvas_Draw_RGBA_Image(canvas, overlayedImage, off_x, y + off_y, 293, 313);
			canvas->globalAlpha = (u8) (192 * alpha_factor);
			Canvas_Draw_RGBA_Image_Culled(canvas, overlayedImage, y * 0.25 + off_x, y * 1.25 + off_y, 293, 313);
			canvas->globalAlpha = (u8) (128 * alpha_factor);
			Canvas_Draw_RGBA_Image_Culled(canvas, overlayedImage, y * 0.5 + off_x, y * 1.5 + off_y, 293, 313);

			canvas->globalAlpha = 255;
			Canvas_Draw_RGBA_Image(canvas, prophecy->data, (rmode->fbWidth - prophecy->width) / 2, sinf(counter - 0.5) * 10 + off_y_prophecy + (25 - prophecy->height) / 2, prophecy->width, prophecy->height);

			if (swap < elapsed) {
				PROPH_Next();
				prophecy = PROPH_Get();
				swap = 221583 / PROPHECY_NUM;
			} else swap -= elapsed;

			if (wait < elapsed) {
				// intentional crash
				crash();
				// this is for dolphin
				//exit(0);
			} else wait -= elapsed;
		}

		Canvas_Render(canvas);
		Canvas_Swap(canvas);
		VIDEO_Flush();

		VIDEO_WaitVSync();
	}

	return 0;
}