#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include <font/fonts.h>
#include <stdint.h>

typedef struct {
	int16_t X;
	int16_t Y;
} Point, *pPoint;

typedef enum {
	CENTER_MODE = 0x01, /* center mode */
	RIGHT_MODE = 0x02, /* right mode  */
	LEFT_MODE = 0x03, /* left mode   */
} Text_AlignModeTypdef;

#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_LIGHTBLUE     0x841F
#define LCD_COLOR_LIGHTGREEN    0x87F0
#define LCD_COLOR_LIGHTRED      0xFC10
#define LCD_COLOR_LIGHTCYAN     0x87FF
#define LCD_COLOR_LIGHTMAGENTA  0xFC1F
#define LCD_COLOR_LIGHTYELLOW   0xFFF0
#define LCD_COLOR_DARKBLUE      0x0010
#define LCD_COLOR_DARKGREEN     0x0400
#define LCD_COLOR_DARKRED       0x8000
#define LCD_COLOR_DARKCYAN      0x0410
#define LCD_COLOR_DARKMAGENTA   0x8010
#define LCD_COLOR_DARKYELLOW    0x8400
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_LIGHTGRAY     0xD69A
#define LCD_COLOR_GRAY          0x8410
#define LCD_COLOR_DARKGRAY      0x4208
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_BROWN         0xA145
#define LCD_COLOR_ORANGE        0xFD80

extern uint16_t color_array[24];

#define LCD_DEFAULT_FONT        Font12

uint16_t LCD_GetXSize(void);
uint16_t LCD_GetYSize(void);

void LCD_SetTextColor(uint16_t Color);
void LCD_SetBackColor(uint16_t Color);
uint16_t LCD_GetTextColor(void);
uint16_t LCD_GetBackColor(void);
void LCD_SetFont(sFONT *pFonts);
sFONT* LCD_GetFont(void);
void LCD_SetFontScale(uint8_t scale);
uint8_t LCD_GetFontScale(void);

uint16_t LCD_ReadPixel(uint16_t *buf, uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPixel(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t pixel);
void LCD_Clear(uint16_t *buf, uint16_t color);
void LCD_ClearStringLine(uint16_t *buf, uint16_t Line);
void LCD_DisplayStringAtLine(uint16_t *buf, uint16_t Line, uint8_t *ptr);
void LCD_DisplayStringAt(uint16_t *buf, uint16_t X, uint16_t Y, uint8_t *pText,
		Text_AlignModeTypdef mode);
void LCD_DisplayChar(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

void LCD_DrawHLine(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint16_t color);
void LCD_DrawVLine(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint16_t color);
void LCD_DrawLine(uint16_t *buf, uint16_t X1, uint16_t Y1, uint16_t X2,
		uint16_t Y2, uint16_t color);
void LCD_DrawRect(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height, uint16_t color);
void LCD_DrawCircle(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t Radius, uint16_t color);
void LCD_DrawPolygon(uint16_t *buf, pPoint Points, uint16_t PointCount,
		uint16_t color);
void LCD_DrawEllipse(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t XRadius, uint16_t YRadius, uint16_t color);
void LCD_DrawBitmap(uint16_t *buf, uint32_t X, uint32_t Y, uint8_t *pBmp);

void LCD_FillRect(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height, uint16_t color);
void LCD_FillCircle(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t Radius, uint16_t color);
void LCD_FillTriangle(uint16_t *buf, uint16_t X1, uint16_t X2, uint16_t X3,
		uint16_t Y1, uint16_t Y2, uint16_t Y3, uint16_t color);
void LCD_FillPolygon(uint16_t *buf, pPoint Points, uint16_t PointCount,
		uint16_t color);
void LCD_FillEllipse(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t XRadius, uint16_t YRadius, uint16_t color);

#endif /* INC_GRAPHICS_H_ */
