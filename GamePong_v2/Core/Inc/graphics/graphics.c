#include <font/fonts.h>
#include "graphics.h"
#include "gw_hardware/gw_lcd.h"
#include "main.h"

#define ABS(X)		((X) > 0 ? (X) : -(X))
#define POLY_X(Z)   ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)   ((int32_t)((Points + Z)->Y))
#define PPOS(X, Y)	((Y * GW_LCD_WIDTH) + X)

extern DMA2D_HandleTypeDef hdma2d;

static sFONT *font = &LCD_DEFAULT_FONT;
static uint16_t font_color = 0xFFFF;
static uint16_t font_bcolor = 0x0000;
static uint8_t font_scale = 0x01;

uint16_t color_array[] = {
LCD_COLOR_BLUE, LCD_COLOR_GREEN, LCD_COLOR_RED, LCD_COLOR_CYAN,
LCD_COLOR_MAGENTA, LCD_COLOR_YELLOW,
LCD_COLOR_LIGHTBLUE, LCD_COLOR_LIGHTGREEN, LCD_COLOR_LIGHTRED,
LCD_COLOR_LIGHTCYAN,
LCD_COLOR_LIGHTMAGENTA, LCD_COLOR_LIGHTYELLOW, LCD_COLOR_DARKBLUE,
LCD_COLOR_DARKGREEN,
LCD_COLOR_DARKRED, LCD_COLOR_DARKCYAN, LCD_COLOR_DARKMAGENTA,
LCD_COLOR_DARKYELLOW, LCD_COLOR_WHITE,
LCD_COLOR_LIGHTGRAY, LCD_COLOR_GRAY, LCD_COLOR_DARKGRAY,
LCD_COLOR_BROWN, LCD_COLOR_ORANGE };

static void DrawChar(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		const uint8_t *c, uint8_t scale);
static void FillBuffer(void *pDst, uint32_t xSize, uint32_t ySize,
		uint32_t OffLine, uint16_t ColorIndex);

/**
 * @brief  Gets the LCD X size.
 * @param  None
 * @retval The used LCD X size
 */
uint16_t LCD_GetXSize(void) {
	return GW_LCD_WIDTH;
}

/**
 * @brief  Gets the LCD Y size.
 * @param  None
 * @retval The used LCD Y size
 */
uint16_t LCD_GetYSize(void) {
	return GW_LCD_HEIGHT;
}

/**
 * @brief  Gets the LCD Text color.
 * @param  None
 * @retval Text color
 */
uint16_t LCD_GetTextColor(void) {
	return font_color;
}

/**
 * @brief  Gets the LCD Background color.
 * @param  None
 * @retval Background color
 */
uint16_t LCD_GetBackColor(void) {
	return font_bcolor;
}

/**
 * @brief  Sets the Text color.
 * @param  Color: the Text color code RGB(5-6-5)
 * @retval None
 */
void LCD_SetTextColor(uint16_t Color) {
	font_color = Color;
}

/**
 * @brief  Sets the Background color.
 * @param  Color: the layer Background color code RGB(5-6-5)
 * @retval None
 */
void LCD_SetBackColor(uint16_t Color) {
	font_bcolor = Color;
}

/**
 * @brief  Sets the Text Font.
 * @param  pFonts: the layer font to be used
 * @retval None
 */
void LCD_SetFont(sFONT *pFonts) {
	font = pFonts;
}

/**
 * @brief  Gets the Text Font.
 * @param  None
 * @retval Layer font
 */
sFONT* LCD_GetFont(void) {
	return font;
}

/**
 * @brief  Sets the Text Scale.
 * @param  scale: the scale of font to be used
 * @retval None
 */
void LCD_SetFontScale(uint8_t scale) {
	font_scale = scale;
}

/**
 * @brief  Gets the Text Scale.
 * @param  None
 * @retval Font scale
 */
uint8_t LCD_GetFontScale(void) {
	return font_scale;
}

/**
 * @brief  Reads Pixel.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @retval RGB pixel color
 */
uint16_t LCD_ReadPixel(uint16_t *buf, uint16_t Xpos, uint16_t Ypos) {
	if (PPOS(Xpos, Ypos) > GW_LCD_WIDTH * GW_LCD_HEIGHT) {
		return 0;
	}
	return buf[PPOS(Xpos, Ypos)];
}

/**
 * @brief  Writes Pixel.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  RGB_Code: the pixel color in RGB(5-6-5)
 * @retval None
 */
void LCD_DrawPixel(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t pixel) {
	if (PPOS(Xpos, Ypos) > GW_LCD_WIDTH * GW_LCD_HEIGHT) {
		return;
	}
	buf[PPOS(Xpos, Ypos)] = pixel;
}

/**
 * @brief  Clears the whole LCD.
 * @param  Color: the color of the background
 * @retval None
 */
void LCD_Clear(uint16_t *buf, uint16_t color) {
	FillBuffer((uint32_t*) buf, LCD_GetXSize(), LCD_GetYSize(), 0, color);
}

/**
 * @brief  Clears the selected line.
 * @param  Line: the line to be cleared
 * @retval None
 */
void LCD_ClearStringLine(uint16_t *buf, uint16_t Line) {
	LCD_FillRect(buf, 0, (Line * font_scale * font->Height), LCD_GetXSize(),
			font_scale * font->Height, font_bcolor);
}

/**
 * @brief  Displays a string on the LCD.
 * @param  Line: the Line where to display the character shape
 * @param  ptr: pointer to string to display on LCD
 * @retval None
 */
void LCD_DisplayStringAtLine(uint16_t *buf, uint16_t Line, uint8_t *ptr) {
	LCD_DisplayStringAt(buf, 0, (Line * font_scale * font->Height), ptr,
			LEFT_MODE);
}

/**
 * @brief  Displays a string on the LCD.
 * @param  X: pointer to x position (in pixel)
 * @param  Y: pointer to y position (in pixel)
 * @param  pText: pointer to string to display on LCD
 * @param  mode: The display mode
 *    This parameter can be one of the following values:
 *                @arg CENTER_MODE
 *                @arg RIGHT_MODE
 *                @arg LEFT_MODE
 * @retval None
 */
void LCD_DisplayStringAt(uint16_t *buf, uint16_t X, uint16_t Y, uint8_t *pText,
		Text_AlignModeTypdef mode) {
	uint16_t refcolumn = 1, i = 0;
	uint32_t size = 0, xsize = 0;
	uint32_t mod_size = 0;
	uint8_t *ptr = pText;
	uint32_t font_width = font->Width * font_scale;

	/* Get the text size */
	while (*ptr++)
		size++;

	/* Characters number per line */
	xsize = (LCD_GetXSize() / font_width);
	mod_size = LCD_GetXSize() % font_width;

	switch (mode) {
	case CENTER_MODE: {
		refcolumn = X + ((xsize - size) * font_width + mod_size) / 2;
		break;
	}
	case LEFT_MODE: {
		refcolumn = X;
		break;
	}
	case RIGHT_MODE: {
		refcolumn = X + ((xsize - size) * font_width) + mod_size;
		break;
	}
	default: {
		refcolumn = X;
		break;
	}
	}

	/* Send the string character by character on LCD */
	while ((*pText != 0)
			& (((LCD_GetXSize() - (i * font_width)) & 0xFFFF) >= font_width)) {
		/* Display one character on LCD */
		LCD_DisplayChar(buf, refcolumn, Y, *pText);
		/* Decrement the column position by 16 */
		refcolumn += font_width;
		/* Point on the next character */
		pText++;
		i++;
	}
}

/**
 * @brief  Displays one character.
 * @param  Xpos: start column address
 * @param  Ypos: the Line where to display the character shape
 * @param  Ascii: character ascii code, must be between 0x20 and 0x7E
 * @retval None
 */
void LCD_DisplayChar(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint8_t Ascii) {
	DrawChar(buf, Xpos, Ypos,
			&font->table[(Ascii - ' ') * font->Height * ((font->Width + 7) / 8)],
			font_scale);
}

/**
 * @brief  Displays an horizontal line.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Length: line length
 * @retval None
 */
void LCD_DrawHLine(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint16_t color) {
	uint16_t *xaddress = 0;

	/* Get the line address */
	xaddress = (buf) + (LCD_GetXSize() * Ypos + Xpos);

	/* Write line */
	FillBuffer((uint32_t*) xaddress, Length, 1, 0, color);
}

/**
 * @brief  Displays a vertical line.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Length: line length
 * @retval None
 */
void LCD_DrawVLine(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint16_t color) {
	uint16_t *xaddress = 0;

	/* Get the line address */
	xaddress = (buf) + (LCD_GetXSize() * Ypos + Xpos);

	/* Write line */
	FillBuffer((uint32_t*) xaddress, 1, Length, (LCD_GetXSize() - 1), color);
}

/**
 * @brief  Displays an uni-line (between two points).
 * @param  X1: the point 1 X position
 * @param  Y1: the point 1 Y position
 * @param  X2: the point 2 X position
 * @param  Y2: the point 2 Y position
 * @retval None
 */
void LCD_DrawLine(uint16_t *buf, uint16_t X1, uint16_t Y1, uint16_t X2,
		uint16_t Y2, uint16_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(X2 - X1); /* The difference between the x's */
	deltay = ABS(Y2 - Y1); /* The difference between the y's */
	x = X1; /* Start x off at the first pixel */
	y = Y1; /* Start y off at the first pixel */

	if (X2 >= X1) /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	} else /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (Y2 >= Y1) /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	} else /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) /* There is at least one x-value for every y-value */
	{
		xinc1 = 0; /* Don't change the x when numerator >= denominator */
		yinc2 = 0; /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; /* There are more x-values than y-values */
	} else /* There is at least one y-value for every x-value */
	{
		xinc2 = 0; /* Don't change the x for every iteration */
		yinc1 = 0; /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		LCD_DrawPixel(buf, x, y, color); /* Draw the current pixel */
		num += numadd; /* Increase the numerator by the top of the fraction */
		if (num >= den) /* Check if numerator >= denominator */
		{
			num -= den; /* Calculate the new numerator value */
			x += xinc1; /* Change the x as appropriate */
			y += yinc1; /* Change the y as appropriate */
		}
		x += xinc2; /* Change the x as appropriate */
		y += yinc2; /* Change the y as appropriate */
	}
}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Height: display rectangle height
 * @param  Width: display rectangle width
 * @retval None
 */
void LCD_DrawRect(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height, uint16_t color) {
	/* Draw horizontal lines */
	LCD_DrawHLine(buf, Xpos, Ypos, Width, color);
	LCD_DrawHLine(buf, Xpos, (Ypos + Height), Width, color);

	/* Draw vertical lines */
	LCD_DrawVLine(buf, Xpos, Ypos, Height, color);
	LCD_DrawVLine(buf, (Xpos + Width), Ypos, Height, color);
}

/**
 * @brief  Displays a circle.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Radius: the circle radius
 * @retval None
 */
void LCD_DrawCircle(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t Radius, uint16_t color) {
	int32_t d;/* Decision Variable */
	uint32_t curx;/* Current X Value */
	uint32_t cury;/* Current Y Value */

	d = 3 - (Radius << 1);
	curx = 0;
	cury = Radius;

	while (curx <= cury) {
		LCD_DrawPixel(buf, (Xpos + curx), (Ypos - cury), color);
		LCD_DrawPixel(buf, (Xpos - curx), (Ypos - cury), color);
		LCD_DrawPixel(buf, (Xpos + cury), (Ypos - curx), color);
		LCD_DrawPixel(buf, (Xpos - cury), (Ypos - curx), color);
		LCD_DrawPixel(buf, (Xpos + curx), (Ypos + cury), color);
		LCD_DrawPixel(buf, (Xpos - curx), (Ypos + cury), color);
		LCD_DrawPixel(buf, (Xpos + cury), (Ypos + curx), color);
		LCD_DrawPixel(buf, (Xpos - cury), (Ypos + curx), color);

		if (d < 0) {
			d += (curx << 2) + 6;
		} else {
			d += ((curx - cury) << 2) + 10;
			cury--;
		}
		curx++;
	}
}

/**
 * @brief  Displays an poly-line (between many points).
 * @param  Points: pointer to the points array
 * @param  PointCount: Number of points
 * @retval None
 */
void LCD_DrawPolygon(uint16_t *buf, pPoint Points, uint16_t PointCount,
		uint16_t color) {
	int16_t x = 0, y = 0;

	if (PointCount < 2) {
		return;
	}

	LCD_DrawLine(buf, Points->X, Points->Y, (Points + PointCount - 1)->X,
			(Points + PointCount - 1)->Y, color);

	while (--PointCount) {
		x = Points->X;
		y = Points->Y;
		Points++;
		LCD_DrawLine(buf, x, y, Points->X, Points->Y, color);
	}
}

/**
 * @brief  Displays an Ellipse.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  XRadius: the X radius of ellipse
 * @param  YRadius: the Y radius of ellipse
 * @retval None
 */
void LCD_DrawEllipse(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t XRadius, uint16_t YRadius, uint16_t color) {
	int x = 0, y = -YRadius, err = 2 - 2 * XRadius, e2;
	float k = 0, rad1 = 0, rad2 = 0;

	rad1 = XRadius;
	rad2 = YRadius;

	k = (float) (rad2 / rad1);

	do {
		LCD_DrawPixel(buf, (Xpos - (uint16_t) (x / k)), (Ypos + y), color);
		LCD_DrawPixel(buf, (Xpos + (uint16_t) (x / k)), (Ypos + y), color);
		LCD_DrawPixel(buf, (Xpos + (uint16_t) (x / k)), (Ypos - y), color);
		LCD_DrawPixel(buf, (Xpos - (uint16_t) (x / k)), (Ypos - y), color);

		e2 = err;
		if (e2 <= x) {
			err += ++x * 2 + 1;
			if (-y == x && e2 <= y)
				e2 = 0;
		}
		if (e2 > y)
			err += ++y * 2 + 1;
	} while (y <= 0);
}

/**
 * @brief  Displays a bitmap picture loaded in the internal Flash (32 bpp).
 * @param  X: the bmp x position in the LCD
 * @param  Y: the bmp Y position in the LCD
 * @param  pBmp: Bmp picture address in the internal Flash
 * @retval None
 */
void LCD_DrawBitmap(uint16_t *buf, uint32_t X, uint32_t Y, uint8_t *pBmp) {
	// Not implemented today
}

/**
 * @brief  Displays a full rectangle.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Height: rectangle height
 * @param  Width: rectangle width
 * @retval None
 */
void LCD_FillRect(uint16_t *buf, uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height, uint16_t color) {
	uint16_t *xaddress = 0;

	/* Get the rectangle start address */
	xaddress = (buf) + (LCD_GetXSize() * Ypos + Xpos);

	/* Fill the rectangle */
	FillBuffer((uint32_t*) xaddress, Width, Height, (LCD_GetXSize() - Width),
			color);
}

/**
 * @brief  Displays a full circle.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  Radius: the circle radius
 * @retval None
 */
void LCD_FillCircle(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t Radius, uint16_t color) {
	int32_t d; /* Decision Variable */
	uint32_t curx;/* Current X Value */
	uint32_t cury;/* Current Y Value */

	d = 3 - (Radius << 1);

	curx = 0;
	cury = Radius;

	while (curx <= cury) {
		if (cury > 0) {
			LCD_DrawHLine(buf, Xpos - cury, Ypos + curx, 2 * cury, color);
			LCD_DrawHLine(buf, Xpos - cury, Ypos - curx, 2 * cury, color);
		}

		if (curx > 0) {
			LCD_DrawHLine(buf, Xpos - curx, Ypos - cury, 2 * curx, color);
			LCD_DrawHLine(buf, Xpos - curx, Ypos + cury, 2 * curx, color);
		}
		if (d < 0) {
			d += (curx << 2) + 6;
		} else {
			d += ((curx - cury) << 2) + 10;
			cury--;
		}
		curx++;
	}
	LCD_DrawCircle(buf, Xpos, Ypos, Radius, color);
}

/**
 * @brief  Fill triangle.
 * @param  X1: the point 1 x position
 * @param  Y1: the point 1 y position
 * @param  X2: the point 2 x position
 * @param  Y2: the point 2 y position
 * @param  X3: the point 3 x position
 * @param  Y3: the point 3 y position
 * @retval None
 */
void LCD_FillTriangle(uint16_t *buf, uint16_t X1, uint16_t X2, uint16_t X3,
		uint16_t Y1, uint16_t Y2, uint16_t Y3, uint16_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(X2 - X1); /* The difference between the x's */
	deltay = ABS(Y2 - Y1); /* The difference between the y's */
	x = X1; /* Start x off at the first pixel */
	y = Y1; /* Start y off at the first pixel */

	if (X2 >= X1) /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	} else /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (Y2 >= Y1) /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	} else /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) /* There is at least one x-value for every y-value */
	{
		xinc1 = 0; /* Don't change the x when numerator >= denominator */
		yinc2 = 0; /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; /* There are more x-values than y-values */
	} else /* There is at least one y-value for every x-value */
	{
		xinc2 = 0; /* Don't change the x for every iteration */
		yinc1 = 0; /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		LCD_DrawLine(buf, x, y, X3, Y3, color);

		num += numadd; /* Increase the numerator by the top of the fraction */
		if (num >= den) /* Check if numerator >= denominator */
		{
			num -= den; /* Calculate the new numerator value */
			x += xinc1; /* Change the x as appropriate */
			y += yinc1; /* Change the y as appropriate */
		}
		x += xinc2; /* Change the x as appropriate */
		y += yinc2; /* Change the y as appropriate */
	}
}

/**
 * @brief  Displays a full poly-line (between many points).
 * @param  Points: pointer to the points array
 * @param  PointCount: Number of points
 * @retval None
 */
void LCD_FillPolygon(uint16_t *buf, pPoint Points, uint16_t PointCount,
		uint16_t color) {
	int16_t x = 0, y = 0, x2 = 0, y2 = 0, xcenter = 0, ycenter = 0, xfirst = 0,
			yfirst = 0, pixelx = 0, pixely = 0, counter = 0;
	uint16_t imageleft = 0, imageright = 0, imagetop = 0, imagebottom = 0;

	imageleft = imageright = Points->X;
	imagetop = imagebottom = Points->Y;

	for (counter = 1; counter < PointCount; counter++) {
		pixelx = POLY_X(counter);
		if (pixelx < imageleft) {
			imageleft = pixelx;
		}
		if (pixelx > imageright) {
			imageright = pixelx;
		}

		pixely = POLY_Y(counter);
		if (pixely < imagetop) {
			imagetop = pixely;
		}
		if (pixely > imagebottom) {
			imagebottom = pixely;
		}
	}

	if (PointCount < 2) {
		return;
	}

	xcenter = (imageleft + imageright) / 2;
	ycenter = (imagebottom + imagetop) / 2;

	xfirst = Points->X;
	yfirst = Points->Y;

	while (--PointCount) {
		x = Points->X;
		y = Points->Y;
		Points++;
		x2 = Points->X;
		y2 = Points->Y;

		LCD_FillTriangle(buf, x, x2, xcenter, y, y2, ycenter, color);
		LCD_FillTriangle(buf, x, xcenter, x2, y, ycenter, y2, color);
		LCD_FillTriangle(buf, xcenter, x2, x, ycenter, y2, y, color);
	}

	LCD_FillTriangle(buf, xfirst, x2, xcenter, yfirst, y2, ycenter, color);
	LCD_FillTriangle(buf, xfirst, xcenter, x2, yfirst, ycenter, y2, color);
	LCD_FillTriangle(buf, xcenter, x2, xfirst, ycenter, y2, yfirst, color);
}

/**
 * @brief  Draw a full ellipse.
 * @param  Xpos: the X position
 * @param  Ypos: the Y position
 * @param  XRadius: X radius of ellipse
 * @param  YRadius: Y radius of ellipse.
 * @retval None
 */
void LCD_FillEllipse(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		uint16_t XRadius, uint16_t YRadius, uint16_t color) {
	int x = 0, y = -YRadius, err = 2 - 2 * XRadius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = XRadius;
	rad2 = YRadius;
	K = (float) (rad2 / rad1);

	do {
		LCD_DrawHLine(buf, (Xpos - (uint16_t) (x / K)), (Ypos + y),
				(2 * (uint16_t) (x / K) + 1), color);
		LCD_DrawHLine(buf, (Xpos - (uint16_t) (x / K)), (Ypos - y),
				(2 * (uint16_t) (x / K) + 1), color);

		e2 = err;
		if (e2 <= x) {
			err += ++x * 2 + 1;
			if (-y == x && e2 <= y)
				e2 = 0;
		}
		if (e2 > y)
			err += ++y * 2 + 1;
	} while (y <= 0);
}

/**
 * @brief  Draws a character on LCD.
 * @param  Xpos: the Line where to display the character shape
 * @param  Ypos: start column address
 * @param  c: pointer to the character data
 * @retval None
 */
static void DrawChar(uint16_t *buf, uint16_t Xpos, uint16_t Ypos,
		const uint8_t *c, uint8_t scale) {
	uint32_t i = 0, j = 0;
	uint16_t height, width;
	uint8_t offset;
	uint8_t *pchar;
	uint32_t line = 0;

	height = font->Height;
	width = font->Width;

	offset = 8 * ((width + 7) / 8) - width;

	for (i = 0; i < height; i++) {
		pchar = ((uint8_t*) c + (width + 7) / 8 * i);

		switch (((width + 7) / 8)) {
		case 1:
			line = pchar[0];
			break;

		case 2:
			line = (pchar[0] << 8) | pchar[1];
			break;

		case 3:
		default:
			line = (pchar[0] << 16) | (pchar[1] << 8) | pchar[2];
			break;
		}

		for (j = 0; j < width; j++) {
			if (line & (1 << (width - j + offset - 1))) {
				//LCD_DrawPixel(buf, (Xpos + j), Ypos, font_color);
				LCD_FillRect(buf, (Xpos + j * scale), Ypos, scale, scale,
						font_color);
			} else {
				//LCD_DrawPixel(buf, (Xpos + j), Ypos, font_bcolor);
				LCD_FillRect(buf, (Xpos + j * scale), Ypos, scale, scale,
						font_bcolor);
			}
		}
		Ypos = Ypos + scale;
	}
}

/**
 * @brief  Fills buffer.
 * @param  pDst: output color
 * @param  xSize: buffer width
 * @param  ySize: buffer height
 * @param  OffLine: offset
 * @param  ColorIndex: color Index
 * @retval None
 */
static void FillBuffer(void *pDst, uint32_t xSize, uint32_t ySize,
		uint32_t OffLine, uint16_t ColorIndex) {

	DMA2D->CR = DMA2D_R2M;
	DMA2D->OCOLR = 0x0000FFFF & ColorIndex;
	DMA2D->OMAR = (uint32_t) pDst;
	DMA2D->OOR = OffLine;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
	DMA2D->NLR = (uint32_t) (xSize << 16) | (uint16_t) ySize;

	DMA2D->CR |= DMA2D_CR_START;

	while (DMA2D->CR & DMA2D_CR_START) {

	}
}
