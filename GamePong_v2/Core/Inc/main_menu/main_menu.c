#include <main_menu/about_menu.h>
#include <main_menu/main_menu.h>
#include <main_menu/settings_menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game/game.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics/graphics.h"
#include "graphics/sprites.h"

static void main_menu_loop();
static void process_input();
static void update();
static void draw_background(uint16_t *buf);
static void menu_select();

static uint16_t title_pallete[] = { LCD_COLOR_DARKGREEN, LCD_COLOR_ORANGE,
LCD_COLOR_DARKBLUE, LCD_COLOR_RED };
static uint8_t title_pallete_idx = 0;
static uint16_t selected_item_color = LCD_COLOR_DARKYELLOW;

static int ticks = 0;
static int action_delay = 0;
static uint8_t selected_item = 0;

extern int in_demo;
int demo_ticks = 0;

int restart_game;
int in_main_menu;

void main_menu_init() {
	if (restart_game) {
		game_init();
		return;
	}
	demo_ticks = 0;
	in_main_menu = 1;
	action_delay = 20;
	title_pallete_idx = 0;
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font24);
	LCD_SetFontScale(1);
	while (in_main_menu) {
		main_menu_loop();
	}
	menu_select();
}

static void main_menu_loop() {
	uint16_t *fb = lcd_get_active_buffer();
	draw_background(fb);

	process_input();
	update();

	lcd_swap();
	lcd_wait_for_vblank();
}

static void draw_background(uint16_t *buf) {
	LCD_Clear(buf, LCD_COLOR_BLACK);

	if (ticks % 10 == 0) {
		title_pallete_idx--;
	}

	LCD_SetFontScale(3);
	LCD_SetFont(&Font24);
	LCD_SetTextColor(title_pallete[(title_pallete_idx + 0) % 4]);
	LCD_DisplayChar(buf, 40, 20, 'P');
	LCD_SetTextColor(title_pallete[(title_pallete_idx + 1) % 4]);
	LCD_DisplayChar(buf, 80, 20, 'o');
	LCD_SetTextColor(title_pallete[(title_pallete_idx + 2) % 4]);
	LCD_DisplayChar(buf, 120, 20, 'n');
	LCD_SetTextColor(title_pallete[(title_pallete_idx + 3) % 4]);
	LCD_DisplayChar(buf, 160, 20, 'g');
	LCD_SetFontScale(1);

	LCD_FillRect(buf, 220, 64, 50, 8, LCD_COLOR_WHITE);
	LCD_FillCircle(buf, 270, 40, 10, LCD_COLOR_WHITE);
	LCD_DrawLine(buf, 221, 40, 245, 64, LCD_COLOR_WHITE);
	LCD_DrawLine(buf, 269, 40, 245, 64, LCD_COLOR_WHITE);

	if (selected_item % 4 == 0) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Start\0", CENTER_MODE);
	if (selected_item % 4 == 1) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 140, (uint8_t*) "Settings\0", CENTER_MODE);
	if (selected_item % 4 == 2) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 170, (uint8_t*) "About\0", CENTER_MODE);
	if (selected_item % 4 == 3) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 200, (uint8_t*) "Exit\0", CENTER_MODE);
}

static void process_input() {
	uint32_t buttons = buttons_get();
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	if (buttons & ~B_POWER) {
		demo_ticks = 0;
	}
	if (action_delay <= 0) {
		if (buttons & B_Up) {
			selected_item--;
			action_delay = 10;
		}
		if (buttons & B_Down) {
			selected_item++;
			action_delay = 10;
		}
		if (buttons & B_A) {
			in_main_menu = 0;
		}
	} else {
		action_delay--;
	}
}

static void update() {
	ticks++;
	demo_ticks++;
	if (demo_ticks > 1000) {
		in_demo = 1;
		game_init();
		action_delay = 20;
	}
}

static void menu_select() {
	switch (selected_item % 4) {
	case 0:
		game_init();
		break;
	case 1:
		settings_menu_init();
		break;
	case 2:
		about_menu_init();
		break;
	case 3:
		HAL_NVIC_SystemReset();
		break;
	default:
		break;
	}
}
