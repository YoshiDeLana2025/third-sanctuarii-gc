#include <ogc/gx_struct.h>
#include <tuxedo/types.h>

typedef struct {
	GXRModeObj *rmode;
	u32 *frameBufferUsed;
	u32 *frameBufferUnused;
	u32 *rgbMap;
	u8 globalAlpha;
} Canvas;

u32 blend_pair_colors(u32 color1, u32 color2);
u32 rgb_to_ypbpr(u32 rgb);

Canvas* Canvas_Init(GXRModeObj *rmode);
void Canvas_Clear(Canvas *canvas);
void Canvas_Render(Canvas *canvas);
void Canvas_Swap(Canvas *canvas);
void Canvas_Draw_RGBA_Image(Canvas* canvas, const uint8_t *image, int x, int y, int w, int h);
void Canvas_Draw_RGBA_Image_Culled(Canvas* canvas, const uint8_t *image, int x, int y, int w, int h);
void Canvas_Draw_RGBA_Image_Overlay_Loop(Canvas *canvas, const uint8_t *image, const uint8_t *overlay, int x, int y, int w, int h, int ox, int oy, int ow, int oh);
void Canvas_Prerender_RGBA_Image_Overlay_Loop(Canvas *canvas, uint8_t *result, const uint8_t *image, const uint8_t *overlay, int w, int h, int ox, int oy, int ow, int oh);