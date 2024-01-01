#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game/game.h"
#include "main_menu/about_menu.h"
#include "main_menu/main_menu.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics/graphics.h"
#include "graphics/sprites.h"

#define randint(min, max) 	(rand() % (max + 1 - min) + min)

extern uint16_t pallete[];
int secret_data1 = 0;

static void about_menu_loop();
static void process_input();
static void update();
static void draw_background(uint16_t *buf);
static void secret_function();

static int ticks = 0;
static int action_delay = 0;
int in_about_menu = 1;

void about_menu_init() {
	in_about_menu = 1;
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font24);
	LCD_SetFontScale(1);
	while (in_about_menu) {
		about_menu_loop();
	}
	//main_menu_init();
}
static void about_menu_loop() {
	uint16_t *fb = lcd_get_active_buffer();
	draw_background(fb);

	process_input();
	update();

	lcd_swap();
	lcd_wait_for_vblank();
}

static void process_input() {
	uint32_t buttons = buttons_get();
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	if (action_delay <= 0) {
		if (buttons & B_B) {
			in_about_menu = 0;
		}
	} else {
		action_delay--;
	}
	if (buttons & B_Up) {
		secret_data1 = 1;
	}
	if (buttons & B_Down && secret_data1) {
		secret_function();
		action_delay = 15;
	}
	if (buttons & ~B_Up) {
		secret_data1 = 0;
	}
}

static void update() {
	ticks++;
}

static void draw_background(uint16_t *buf) {
	LCD_Clear(buf, LCD_COLOR_BLACK);

	LCD_SetFontScale(3);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font24);
	LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	LCD_DisplayChar(buf, 40, 20, 'P');
	LCD_SetTextColor(LCD_COLOR_ORANGE);
	LCD_DisplayChar(buf, 80, 20, 'o');
	LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	LCD_DisplayChar(buf, 120, 20, 'n');
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DisplayChar(buf, 160, 20, 'g');
	LCD_SetFontScale(1);

	LCD_FillRect(buf, 220, 64, 50, 8, LCD_COLOR_WHITE);
	LCD_FillCircle(buf, 270, 40, 10, LCD_COLOR_WHITE);
	LCD_DrawLine(buf, 221, 40, 245, 64, LCD_COLOR_WHITE);
	LCD_DrawLine(buf, 269, 40, 245, 64, LCD_COLOR_WHITE);

	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font20);
	LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Developed by:\0", CENTER_MODE);
	LCD_SetFont(&Font24);
	LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
	LCD_DisplayStringAt(buf, 0, 140, (uint8_t*) "Jopo12321\0", CENTER_MODE);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font12);
	LCD_DisplayStringAt(buf, 0, 175, (uint8_t*) "jozef.kromka22@gmail.com\0",
			CENTER_MODE);
	LCD_DisplayStringAt(buf, 0, 200,
			(uint8_t*) "https://github.com/Jopo12321/\0", CENTER_MODE);
	LCD_DisplayStringAt(buf, 0, 212, (uint8_t*) "game-and-watch-pong\0",
			CENTER_MODE);
}

static void secret_function() {
	int i, j;
	uint8_t secret_data2[] = { 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE,
			0xFF, 0xFF, 0xFF, 0xFE, 0xEE, 0xEE, 0xEE, 0xEE, 0xFF, 0xFE, 0xFF,
			0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xEE, 0xEE,
			0xEE, 0xEE };
	uint8_t secret_data6[] = { 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x0D,
			0x00, 0x00, 0x00, 0xDD, 0x00, 0x00, 0x00, 0xDD,

			0x00, 0x00, 0x0D, 0xDD, 0x0D, 0xDD, 0xDD, 0xDD, 0x0D, 0xDD, 0xDD,
			0xCD, 0x00, 0xDD, 0xDD, 0xCD,

			0x00, 0x0D, 0xDD, 0xCD, 0x00, 0x00, 0xDD, 0xDD, 0x00, 0x00, 0xDD,
			0xDD, 0x00, 0x0D, 0xDD, 0xDD,

			0x00, 0x0D, 0xDD, 0xDD, 0x00, 0x0D, 0xDD, 0xD0, 0x00, 0xDD, 0xD0,
			0x00, 0x00, 0xDD, 0x00, 0x00 };
	Sprite secret_data3;
	Sprite secret_data7;
	uint16_t x[] = { 304, 288, 288, 0, 96, 192, 192, 96, 144, 288, 144, 0, 144,
			288, 144, 0 };
	uint16_t y[] = { 224, 0, 208, 208, 104, 104, 104, 104, 0, 104, 208, 104, 0,
			104, 208, 104 };
	int8_t x_speed[] =
			{ 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, -1, -1, 1, 1 };
	int8_t y_speed[] =
			{ 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, -1, -1, 1 };
	int secret_data8 = 16;
	uint16_t secret_data9[100];
	uint16_t secret_dataA[100];
	for (i = 0; i < 100; ++i) {
		secret_data9[i] = randint(0, 319);
		secret_dataA[i] = randint(0, 239);
	}
	uint16_t max_x = 304;
	uint16_t min_x = 0;
	uint16_t max_y = 224;
	uint16_t min_y = 0;
	uint8_t delay = 30;
	uint8_t dir = 0;
	init_sprite(&secret_data3, x[0], y[0], 8, 8, SPRITE_SHOW | 0x01, pallete,
			secret_data2);
	init_sprite(&secret_data7, x[1], y[1], 8, 16, SPRITE_SHOW | 0x02, pallete,
			secret_data6);
	int secret_data4 = 1;
	int secret_data5 = 1;
	while (secret_data4) {
		uint16_t *fb = lcd_get_active_buffer();

		if (secret_data5) {
			secret_data3.x = x[0];
			secret_data3.y = y[0];
			render_sprite(fb, secret_data3);
			secret_data3.x = x[0] + 8;
			render_sprite(fb, secret_data3);
			secret_data3.y = y[0] + 8;
			render_sprite(fb, secret_data3);
			secret_data3.x = x[0];
			render_sprite(fb, secret_data3);
			if (x[0] == 112 && y[0] == 112) {
				secret_data5 = 0;
				delay = 30;
				x[0] = 0;
				y[0] = 0;
				dir = 5;
			}
			switch (dir) {
			case 0:
				x[0] = x[0] - 16;
				if (x[0] == min_x) {
					dir = 1;
					max_y = max_y - 16;
				}
				break;
			case 1:
				y[0] = y[0] - 16;
				if (y[0] == min_y) {
					dir = 2;
					min_x = min_x + 16;
				}
				break;
			case 2:
				x[0] = x[0] + 16;
				if (x[0] == max_x) {
					dir = 3;
					min_y = min_y + 16;
				}
				break;
			case 3:
				y[0] = y[0] + 16;
				if (y[0] == max_y) {
					dir = 0;
					max_x = max_x - 16;
				}
				break;
			}
			lcd_sync();
		} else {
			if (delay <= 0) {
				LCD_Clear(fb, LCD_COLOR_BLACK);
				for (i = 0; i < 100; ++i) {
					LCD_DrawPixel(fb, secret_data9[i], secret_dataA[i],
							LCD_COLOR_WHITE);
				}
				LCD_SetBackColor(LCD_COLOR_BLACK);
				LCD_SetTextColor(LCD_COLOR_ORANGE);
				LCD_SetFont(&Font24);
				LCD_DisplayStringAt(fb, 0, 96, (uint8_t*) "Thank you for\0",
						CENTER_MODE);
				LCD_DisplayStringAt(fb, 0, 120, (uint8_t*) "playing!\0",
						CENTER_MODE);
				for (i = 0; i < secret_data8; ++i) {
					set_sprite_flip_x(&secret_data7, 0);
					secret_data7.x = x[i];
					secret_data7.y = y[i];
					render_sprite(fb, secret_data7);
					secret_data7.x = x[i] + 16;
					set_sprite_flip_x(&secret_data7, 1);
					render_sprite(fb, secret_data7);
					for (j = 0; j < 3; ++j) {
						if (y[i] == 0) {
							y_speed[i] = 1;
						}
						if (y[i] == 208) {
							y_speed[i] = -1;
						}
						if (x[i] == 0) {
							x_speed[i] = 1;
						}
						if (x[i] == 288) {
							x_speed[i] = -1;
						}
						x[i] = x[i] + x_speed[i];
						y[i] = y[i] + y_speed[i];
					}
				}
			}
		}

		uint32_t buttons = buttons_get();
		if (buttons & B_POWER) {
			HAL_NVIC_SystemReset();
		}
		if (delay <= 0) {
			if (buttons & ~B_POWER) {
				secret_data4 = 0;
			}
		} else {
			delay--;
		}

		lcd_swap();
		lcd_wait_for_vblank();
	}
}
