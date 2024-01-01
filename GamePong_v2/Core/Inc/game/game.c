#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "in_game_menu.h"
#include "game_over.h"
#include "main_menu/main_menu.h"
#include "main_menu/settings.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics/graphics.h"
#include "graphics/sprites.h"

static void game_loop();
static void demo_loop();
static void draw_background(uint16_t *buf);
static void process_input();
static void process_input_demo();
static void update();
static void update_demo();
static void update_player();
static void update_player_demo();
static void update_ball();
static void update_ai();
static void update_ai_demo();
static void update_score(int score);

#define randint(min, max) 	(rand() % (max + 1 - min) + min)
#define randsing()			(rand() > RAND_MAX/2 ? -1: 1)

extern int restart_game;

int score_p1 = 0;
int score_p2 = 0;
static char score_p1_buf[6];
static char score_p2_buf[6];

uint16_t pallete[] = { 0x0001, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xB184, 0xE4E4, 0x0000, 0x9A40 };

uint8_t paddle_data[] = { 0x11, 0x11, 0x11, 0x11 };
uint8_t ball_data[] = { 0x00, 0x00, 0x02, 0x22, 0x00, 0x02, 0x22, 0x22, 0x00,
		0x22, 0x22, 0x22, 0x02, 0x22, 0x22, 0x22, 0x02, 0x22, 0x22, 0x22, 0x22,
		0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };

Sprite paddle1;
Sprite paddle2;
Sprite ball1;
Sprite ball2;
Sprite ball3;
Sprite ball4;
Sprite **list;

const int p1_speed = 2;
int p2_speed = 2;
const int ball_speed = 3;

int ball_xspeed;
int ball_yspeed;
int ball_x = 150;
int ball_y = 150;

static int8_t dir = 0;

static int offset_x;
static int offset_y;

static int tim = 0;
int in_game = 0;
int in_demo = 0;

void game_init() {
	if (!in_demo) {
		restart_game = 0;
		in_game = 1;
		switch (difficulty) {
		case EASY:
			offset_x = 240;
			offset_y = 20;
			break;
		case MEDIUM:
			offset_x = 220;
			offset_y = 25;
			break;
		case HARD:
			offset_x = 200;
			offset_y = 35;
			break;
		}
	}
	score_p1 = 0;
	score_p2 = 0;
	dir = 0;
	ball_xspeed = 1;
	ball_yspeed = randsing();
	ball_x = randint(30, 50);
	ball_y = randint(100, 140);

	pallete[1] = paddle_color;
	pallete[2] = ball_color;

	init_sprite(&paddle1, 2, 110, 1, 8, SPRITE_SHOW | 0x05, pallete,
			paddle_data);
	init_sprite(&paddle2, 313, 110, 1, 8, SPRITE_SHOW | 0x05, pallete,
			paddle_data);

	init_sprite(&ball1, ball_x, ball_y, 8, 8, SPRITE_SHOW | 0x01, pallete,
			ball_data);
	init_sprite(&ball2, ball_x + 8, ball_y, 8, 8,
	SPRITE_SHOW | SPRITE_FLIP_X | 0x01, pallete, ball_data);
	init_sprite(&ball3, ball_x, ball_y + 8, 8, 8,
	SPRITE_SHOW | SPRITE_FLIP_Y | 0x01, pallete, ball_data);
	init_sprite(&ball4, ball_x + 8, ball_y + 8, 8, 8,
	SPRITE_SHOW | SPRITE_FLIP_X | SPRITE_FLIP_Y | 0x01, pallete, ball_data);

	list = sprites_init_list(&paddle1);
	list = sprites_add_to_list(list, &paddle2);
	list = sprites_add_to_list(list, &ball1);
	list = sprites_add_to_list(list, &ball2);
	list = sprites_add_to_list(list, &ball3);
	list = sprites_add_to_list(list, &ball4);

	while (in_game) {
		game_loop();
	}
	while (in_demo) {
		demo_loop();
	}
	//main_menu_init();
}

static void game_loop() {
	uint16_t *fb = lcd_get_active_buffer();
	draw_background(fb);

	render_sprites(fb, list);

	process_input();
	update();

	lcd_swap();
	lcd_wait_for_vblank();
}

static void demo_loop() {
	uint16_t *fb = lcd_get_active_buffer();
	draw_background(fb);

	LCD_SetFont(&Font24);
	LCD_SetFontScale(3);
	LCD_SetTextColor(LCD_COLOR_DARKYELLOW);
	LCD_DisplayStringAt(fb, 6, 100, (uint8_t*) "DEMO\0", CENTER_MODE);
	LCD_SetFontScale(1);

	render_sprites(fb, list);

	process_input_demo();
	update_demo();

	lcd_swap();
	lcd_wait_for_vblank();
}

static void draw_background(uint16_t *buf) {
	LCD_Clear(buf, LCD_COLOR_BLACK);
	LCD_DrawRect(buf, 0, 20, LCD_GetXSize() - 1, LCD_GetYSize() - 21,
	LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font16);
	sprintf(score_p1_buf, "%d", score_p1);
	LCD_DisplayStringAt(buf, 0, 4, (uint8_t*) score_p1_buf, LEFT_MODE);
	sprintf(score_p2_buf, "%d", score_p2);
	LCD_DisplayStringAt(buf, 0, 4, (uint8_t*) score_p2_buf, RIGHT_MODE);
	LCD_DisplayStringAt(buf, 1, 4, (uint8_t*) "SCORE\0", CENTER_MODE);
}

static void process_input() {
	uint32_t buttons = buttons_get();
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	if (buttons & B_GAME) {
		lcd_sync();
		in_game_menu_init();
	}
	if (buttons & B_Up) {
		dir = -1;
	}
	if (buttons & B_Down) {
		dir = 1;
	}
}

static void process_input_demo() {
	uint32_t buttons = buttons_get();
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	if (buttons & ~B_POWER) {
		lcd_sync();
		in_demo = 0;
	}
}

static void update() {
	tim++;
	if (score_p1 >= 10) {
		lcd_sync();
		game_over_init(1);
	}
	if (score_p2 >= 10) {
		lcd_sync();
		game_over_init(2);
	}
	update_player();
	update_ai();
	update_ball();
}

static void update_demo() {
	tim++;
	update_player_demo();
	update_ai_demo();
	update_ball();
}

static void update_player() {
	paddle1.y = paddle1.y + dir * p1_speed;
	if (paddle1.y < 22) {
		paddle1.y = 22;
	}
	if (paddle1.y > 198) {
		paddle1.y = 198;
	}
	dir = 0;
}

static void update_player_demo() {
	if (ball_xspeed > 0 || ball_x > 100) {
		if (paddle1.y < 108) {
			paddle1.y = paddle1.y + 3;
		} else if (paddle1.y > 112) {
			paddle1.y = paddle1.y - 3;
		}
		return;
	}
	if ((ball_y + 8) < (paddle1.y + 20)) {
		paddle1.y = paddle1.y - 3;
		if (paddle1.y < 22) {
			paddle1.y = 22;
		}
	}
	if ((ball_y + 8) > (paddle1.y + 20)) {
		paddle1.y = paddle1.y + 3;
		if (paddle1.y > 198) {
			paddle1.y = 198;
		}
	}
}

static void update_ball() {
	int i;
	for (i = 0; i < ball_speed; ++i) {

		if (ball_x == 7) {
			if ((paddle1.y - 12 < ball_y) && (paddle1.y + 40 - 4 > ball_y)) {
				ball_xspeed = 1;
			} else {
				ball_xspeed = 1;
				update_score(-1);
			}
		}
		if (ball_x == 297) {
			if ((paddle2.y - 12 < ball_y) && (paddle2.y + 40 - 4 > ball_y)) {
				ball_xspeed = -1;
			} else {
				ball_xspeed = -1;
				update_score(1);
			}
		}
		if (ball_y == 21) {
			ball_yspeed = 1;
		}
		if (ball_y == 223) {
			ball_yspeed = -1;
		}
		ball_x = ball_x + ball_xspeed;
		ball_y = ball_y + ball_yspeed;

	}

	ball1.x = ball_x;
	ball1.y = ball_y;
	ball2.x = ball_x + 8;
	ball2.y = ball_y;
	ball3.x = ball_x;
	ball3.y = ball_y + 8;
	ball4.x = ball_x + 8;
	ball4.y = ball_y + 8;
}

static void update_ai() {
	if (ball_xspeed < 0 || ball_x < offset_x) {
		if (paddle2.y < 108) {
			paddle2.y = paddle2.y + p2_speed;
		} else if (paddle2.y > 112) {
			paddle2.y = paddle2.y - p2_speed;
		}
		return;
	}
	int offset = 0;
	if (ball_yspeed > 0) {
		//offset = 10;
		offset = 40 - offset_y;
	} else {
		//offset = 30;
		offset = offset_y;
	}
	if ((ball_y + 8) < (paddle2.y + offset)) {
		paddle2.y = paddle2.y - p2_speed;
		if (paddle2.y < 22) {
			paddle2.y = 22;
		}
	}
	if ((ball_y + 8) > (paddle2.y + offset)) {
		paddle2.y = paddle2.y + p2_speed;
		if (paddle2.y > 198) {
			paddle2.y = 198;
		}
	}
}

static void update_ai_demo() {
	if (ball_xspeed < 0 || ball_x < 220) {
		if (paddle2.y < 108) {
			paddle2.y = paddle2.y + 3;
		} else if (paddle2.y > 112) {
			paddle2.y = paddle2.y - 3;
		}
		return;
	}
	if ((ball_y + 8) < (paddle2.y + 20)) {
		paddle2.y = paddle2.y - 3;
		if (paddle2.y < 22) {
			paddle2.y = 22;
		}
	}
	if ((ball_y + 8) > (paddle2.y + 20)) {
		paddle2.y = paddle2.y + 3;
		if (paddle2.y > 198) {
			paddle2.y = 198;
		}
	}
}

static void update_score(int score) {
	lcd_sync();
	if (score == 1) {
		score_p1++;
	}
	if (score == -1) {
		score_p2++;
	}
	HAL_Delay(500);
	dir = 0;
	ball_xspeed = 1;
	ball_yspeed = randsing();
	ball_x = randint(30, 50);
	ball_y = randint(100, 140);
	paddle2.y = 115;
}
