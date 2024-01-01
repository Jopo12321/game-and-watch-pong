#include "sprites.h"
#include "graphics.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _draw_sprite_no(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);
static void _draw_sprite_x(uint16_t *buf, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t scale, uint16_t *pallete, uint8_t *data);
static void _draw_sprite_y(uint16_t *buf, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t scale, uint16_t *pallete, uint8_t *data);
static void _draw_sprite_xy(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);
static void _draw_sprite_no_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);
static void _draw_sprite_x_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);
static void _draw_sprite_y_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);
static void _draw_sprite_xy_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data);

void init_sprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t settings, uint16_t *pallete, uint8_t *data) {
	sprite->x = x;
	sprite->y = y;
	sprite->width = width;
	sprite->height = height;
	sprite->settings = settings;
	sprite->pallete = pallete;
	sprite->data = data;
}

Sprite* init_sprite_dynamicaly(uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t settings, uint16_t *pallete, uint8_t *data) {
	Sprite *sprite = (Sprite*) malloc(sizeof(Sprite));
	sprite->x = x;
	sprite->y = y;
	sprite->width = width;
	sprite->height = height;
	sprite->settings = settings;
	sprite->pallete = pallete;
	sprite->data = data;
	return sprite;
}

uint16_t get_sprite_x(Sprite sprite) {
	return sprite.x;
}

uint16_t get_sprite_y(Sprite sprite) {
	return sprite.y;
}

uint8_t get_sprite_width(Sprite sprite) {
	return sprite.width;
}

uint8_t get_sprite_height(Sprite sprite) {
	return sprite.height;
}

uint8_t get_sprite_scale(Sprite sprite) {
	return sprite.settings & SPRITE_SCALE;
}

int get_sprite_show(Sprite sprite) {
	return sprite.settings & SPRITE_SHOW;
}

int get_sprite_flip_x(Sprite sprite) {
	return sprite.settings & SPRITE_FLIP_X;
}

int get_sprite_flip_y(Sprite sprite) {
	return sprite.settings & SPRITE_FLIP_Y;
}

int get_sprite_transpose(Sprite sprite) {
	return sprite.settings & SPRITE_TRANSPOSE;
}

uint16_t* get_sprite_pallete(Sprite sprite) {
	return sprite.pallete;
}

uint8_t* get_sprite_data(Sprite sprite) {
	return sprite.data;
}

void toogle_sprite_show(Sprite *sprite) {
	sprite->settings = (*sprite).settings ^ SPRITE_SHOW;
}

void toogle_sprite_flip_x(Sprite *sprite) {
	sprite->settings = (*sprite).settings ^ SPRITE_FLIP_X;
}

void toogle_sprite_flip_y(Sprite *sprite) {
	sprite->settings = (*sprite).settings ^ SPRITE_FLIP_Y;
}

void toogle_sprite_transpose(Sprite *sprite) {
	sprite->settings = (*sprite).settings ^ SPRITE_TRANSPOSE;
}

void set_sprite_x(Sprite *sprite, uint16_t x) {
	sprite->x = x;
}

void set_sprite_y(Sprite *sprite, uint16_t y) {
	sprite->y = y;
}

void set_sprite_width(Sprite *sprite, uint8_t width) {
	sprite->width = width;
}

void set_sprite_height(Sprite *sprite, uint8_t height) {
	sprite->height = height;
}

void set_sprite_scale(Sprite *sprite, uint8_t scale) {
	sprite->settings = scale & SPRITE_SCALE;
}

void set_sprite_show(Sprite *sprite, int show) {
	sprite->settings =
			(show == 0) ?
					(*sprite).settings & ~SPRITE_SHOW :
					(*sprite).settings | SPRITE_SHOW;
}

void set_sprite_flip_x(Sprite *sprite, int flip_x) {
	sprite->settings =
			(flip_x == 0) ?
					(*sprite).settings & ~SPRITE_FLIP_X :
					(*sprite).settings | SPRITE_FLIP_X;
}

void set_sprite_flip_y(Sprite *sprite, int flip_y) {
	sprite->settings =
			(flip_y == 0) ?
					(*sprite).settings & ~SPRITE_FLIP_Y :
					(*sprite).settings | SPRITE_FLIP_Y;
}

void set_sprite_transpose(Sprite *sprite, int transpose) {
	sprite->settings =
			(transpose == 0) ?
					(*sprite).settings & ~SPRITE_TRANSPOSE :
					(*sprite).settings | SPRITE_TRANSPOSE;
}

void set_sprite_pallete(Sprite *sprite, uint16_t *pallete) {
	sprite->pallete = pallete;
}

void set_sprite_data(Sprite *sprite, uint8_t *data) {
	sprite->data = data;
}

void render_sprite(uint16_t *buf, Sprite sprite) {
	if (!(sprite.settings & SPRITE_SHOW) || !(sprite.settings & SPRITE_SCALE)) {
		return;
	}
	uint16_t x = sprite.x;
	uint16_t y = sprite.y;
	uint8_t width = sprite.width;
	uint8_t height = sprite.height;
	uint8_t scale = (sprite.settings & SPRITE_SCALE);
	uint16_t *pallete = sprite.pallete;
	uint8_t *data = sprite.data;
	if ((sprite.settings & SPRITE_TRANSPOSE)) {
		if ((sprite.settings & SPRITE_FLIP_X)
				&& (sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_xy_t(buf, x, y, width, height, scale, pallete, data);
		} else if (!(sprite.settings & SPRITE_FLIP_X)
				&& (sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_y_t(buf, x, y, width, height, scale, pallete, data);
		} else if ((sprite.settings & SPRITE_FLIP_X)
				&& !(sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_x_t(buf, x, y, width, height, scale, pallete, data);
		} else {
			_draw_sprite_no_t(buf, x, y, width, height, scale, pallete, data);
		}
	} else {
		if ((sprite.settings & SPRITE_FLIP_X)
				&& (sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_xy(buf, x, y, width, height, scale, pallete, data);
		} else if (!(sprite.settings & SPRITE_FLIP_X)
				&& (sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_y(buf, x, y, width, height, scale, pallete, data);
		} else if ((sprite.settings & SPRITE_FLIP_X)
				&& !(sprite.settings & SPRITE_FLIP_Y)) {
			_draw_sprite_x(buf, x, y, width, height, scale, pallete, data);
		} else {
			_draw_sprite_no(buf, x, y, width, height, scale, pallete, data);
		}
	}
}

void render_sprites(uint16_t *buf, Sprite **list) {
	int i;
	int length = get_sprite_list_length(list);
	for (i = 0; i < length; i++) {

		render_sprite(buf, (*list[(length - 1) - i]));
	}
}

Sprite** sprites_init_list(Sprite *first_sprite) {
	Sprite **list = (Sprite**) malloc(sizeof(Sprite*) * 2);
	list[0] = first_sprite;
	list[1] = (Sprite*) NULL;
	return list;
}

Sprite** sprites_add_to_list(Sprite **list, Sprite *sprite) {
	int i;
	int length = get_sprite_list_length(list);
	Sprite **new_list = (Sprite**) malloc(sizeof(Sprite*) * (length + 2));
	if (new_list == NULL) {
		Error_Handler();
	}
	for (i = 0; i < length; i++) {
		new_list[i] = list[i];
	}
	new_list[i] = sprite;
	new_list[i + 1] = (Sprite*) NULL;
	free((void*) list);
	list = NULL;
	return new_list;
}

Sprite** sprites_remove_from_list(Sprite **list, Sprite *sprite) {
	int i;
	int removed = 0;
	int length = get_sprite_list_length(list);
	Sprite **new_list = (Sprite**) realloc(list, (length - 1));
	if (new_list == NULL) {
		Error_Handler();
	}
	for (i = 0; i < length - 1; i++) {
		if (list[i] == sprite) {
			removed = 1;
		}
		new_list[i] = list[i + removed];
	}
	new_list[i] = (Sprite*) NULL;
	free((void*) list);
	list = NULL;
	return new_list;
}

Sprite** sprites_clear_list(Sprite **list) {
	free((void*) list);
	return NULL;
}

Sprite* sprites_get_from_list(Sprite **list, int idx) {
	return list[idx];
}

int check_sprites_collision(Sprite sprite1, Sprite sprite2) {
	if (!(sprite1.settings & SPRITE_SCALE)
			|| !(sprite2.settings & SPRITE_SCALE)) {
		return 0;
	}
	uint16_t x1 = sprite1.x;
	uint16_t y1 = sprite1.y;
	uint16_t x2 = sprite2.x;
	uint16_t y2 = sprite2.y;
	uint16_t height1;
	uint16_t width1;
	uint16_t height2;
	uint16_t width2;
	uint8_t out = 0;
	if (sprite1.settings & SPRITE_TRANSPOSE) {
		height1 = sprite1.width * (sprite1.settings & SPRITE_SCALE);
		width1 = sprite1.height * (sprite1.settings & SPRITE_SCALE);
	} else {
		width1 = sprite1.width * (sprite1.settings & SPRITE_SCALE);
		height1 = sprite1.height * (sprite1.settings & SPRITE_SCALE);
	}
	if (sprite2.settings & SPRITE_TRANSPOSE) {
		height2 = sprite2.width * (sprite2.settings & SPRITE_SCALE);
		width2 = sprite2.height * (sprite2.settings & SPRITE_SCALE);
	} else {
		width2 = sprite2.width * (sprite2.settings & SPRITE_SCALE);
		height2 = sprite2.height * (sprite2.settings & SPRITE_SCALE);
	}
	if ((x2 > x1) && (x2 <= (x1 + width1)) && ((x2 + width2) > (x1 + width1))) {
		out |= SPRITE_COLLISION_E;
		if ((y2 > y1) && (y2 <= (y1 + height1))
				&& ((y2 + height2) > (y1 + height1))) {
			out |= SPRITE_COLLISION_N;
		} else if (((y2 <= y1) && ((y2 + height2) >= (y1 + height1)))
				|| ((y1 <= y2) && ((y1 + height1) >= (y2 + height2)))) {
			out |= SPRITE_COLLISION_N | SPRITE_COLLISION_S;
		} else if ((y2 < y1) && ((y2 + height2) >= y1)
				&& ((y2 + height2) < (y1 + height1))) {
			out |= SPRITE_COLLISION_S;
		} else {
			out = 0;
		}
	} else if (((x2 <= x1) && ((x2 + width2) >= (x1 + width1)))
			|| ((x1 <= x2) && ((x1 + width1) >= (x2 + width2)))) {
		out |= SPRITE_COLLISION_E | SPRITE_COLLISION_W;
		if ((y2 > y1) && (y2 < (y1 + height1))
				&& ((y2 + height2) > (y1 + height1))) {
			out |= SPRITE_COLLISION_N;
		} else if (((y2 <= y1) && ((y2 + height2) >= (y1 + height1)))
				|| ((y1 <= y2) && ((y1 + height1) >= (y2 + height2)))) {
			out |= SPRITE_COLLISION_N | SPRITE_COLLISION_S;
		} else if ((y2 < y1) && ((y2 + height2) >= y1)
				&& ((y2 + height2) < (y1 + height1))) {
			out |= SPRITE_COLLISION_S;
		} else {
			out = 0;
		}
	} else if ((x2 < x1) && ((x2 + width2) >= x1)
			&& ((x2 + width2) < (x1 + width1))) {
		out |= SPRITE_COLLISION_W;
		if ((y2 > y1) && (y2 <= (y1 + height1))
				&& ((y2 + height2) > (y1 + height1))) {
			out |= SPRITE_COLLISION_N;
		} else if (((y2 <= y1) && ((y2 + height2) >= (y1 + height1)))
				|| ((y1 <= y2) && ((y1 + height1) >= (y2 + height2)))) {
			out |= SPRITE_COLLISION_N | SPRITE_COLLISION_S;
		} else if ((y2 < y1) && ((y2 + height2) >= y1)
				&& ((y2 + height2) < (y1 + height1))) {
			out |= SPRITE_COLLISION_S;
		} else {
			out = 0;
		}
	}
	if (((x1 + width1 + 1) == x2)
			&& (((y1 + height1) >= y2) && (y1 <= (y2 + height2)))) {
		out |= SPRITE_TOUCH_E;
	}
	if (((x2 + width2 + 1) == x1)
			&& (((y1 + height1) >= y2) && (y1 <= (y2 + height2)))) {
		out |= SPRITE_TOUCH_W;
	}
	if (((y1 + height1 + 1) == y2)
			&& (((x1 + width1) >= x2) && (x1 <= (x2 + width2)))) {
		out |= SPRITE_TOUCH_S;
	}
	if (((y2 + height2 + 1) == y1)
			&& (((x1 + width1) >= x2) && (x1 <= (x2 + width2)))) {
		out |= SPRITE_TOUCH_N;
	}
	return out;
}

int check_sprites_collision_quick(Sprite sprite1, Sprite sprite2) {
	if (!(sprite1.settings & SPRITE_SCALE)
			|| !(sprite2.settings & SPRITE_SCALE)) {
		return 0;
	}
	uint16_t x1 = sprite1.x;
	uint16_t y1 = sprite1.y;
	uint16_t x2 = sprite2.x;
	uint16_t y2 = sprite2.y;
	uint16_t height1;
	uint16_t width1;
	uint16_t height2;
	uint16_t width2;
	if (sprite1.settings & SPRITE_TRANSPOSE) {
		height1 = sprite1.width * (sprite1.settings & SPRITE_SCALE);
		width1 = sprite1.height * (sprite1.settings & SPRITE_SCALE);
	} else {
		width1 = sprite1.width * (sprite1.settings & SPRITE_SCALE);
		height1 = sprite1.height * (sprite1.settings & SPRITE_SCALE);
	}
	if (sprite2.settings & SPRITE_TRANSPOSE) {
		height2 = sprite2.width * (sprite2.settings & SPRITE_SCALE);
		width2 = sprite2.height * (sprite2.settings & SPRITE_SCALE);
	} else {
		width2 = sprite2.width * (sprite2.settings & SPRITE_SCALE);
		height2 = sprite2.height * (sprite2.settings & SPRITE_SCALE);
	}
	if (x1 + width1 > x2 &&     	  	// r1 right edge past r2 left
			x1 < x2 + width2 &&       	// r1 left edge past r2 right
			y1 + height1 > y2 &&       // r1 top edge past r2 bottom
			y1 < y2 + height2) {       // r1 bottom edge past r2 top
		return 1;
	}
	return 0;
}

int get_sprite_list_length(Sprite **list) {
	int i = 0;
	while (list[i] != NULL) {
		i++;
	}
	return i;
}

static void _draw_sprite_no(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + i * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + j * scale + l;
					LCD_DrawPixel(buf, Xpos, Ypos, color);
				}

			}
		}
	}
}
static void _draw_sprite_x(uint16_t *buf, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t scale, uint16_t *pallete, uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + ((width - 1) - i) * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + j * scale + l;
					LCD_DrawPixel(buf, Xpos, Ypos, color);
				}

			}
		}
	}
}
static void _draw_sprite_y(uint16_t *buf, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t scale, uint16_t *pallete, uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + i * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + ((height - 1) - j) * scale + l;
					LCD_DrawPixel(buf, Xpos, Ypos, color);
				}

			}
		}
	}
}
static void _draw_sprite_xy(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + ((width - 1) - i) * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + ((height - 1) - j) * scale + l;
					LCD_DrawPixel(buf, Xpos, Ypos, color);
				}

			}
		}
	}
}

static void _draw_sprite_no_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + i * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + j * scale + l;
					LCD_DrawPixel(buf, Ypos, Xpos, color);
				}

			}
		}
	}
}

static void _draw_sprite_x_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + ((width - 1) - i) * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + j * scale + l;
					LCD_DrawPixel(buf, Ypos, Xpos, color);
				}

			}
		}
	}
}

static void _draw_sprite_y_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + i * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + ((height - 1) - j) * scale + l;
					LCD_DrawPixel(buf, Ypos, Xpos, color);
				}

			}
		}
	}
}

static void _draw_sprite_xy_t(uint16_t *buf, uint16_t x, uint16_t y,
		uint8_t width, uint8_t height, uint8_t scale, uint16_t *pallete,
		uint8_t *data) {
	int i;
	int j;
	int k;
	int l;
	uint16_t color;
	uint16_t Xpos;
	uint16_t Ypos;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			color = pallete[
					(i % 2 == 0) ?
							(((data[(j * width / 2) + i / 2]) & 0xF0) >> 4) :
							((data[(j * width / 2) + i / 2]) & 0x0F)];
			for (k = 0; k < scale; ++k) {
				if (color == pallete[0]) {
					break;
				}
				Xpos = x + ((width - 1) - i) * scale + k;
				for (l = 0; l < scale; ++l) {
					Ypos = y + ((height - 1) - j) * scale + l;
					LCD_DrawPixel(buf, Ypos, Xpos, color);
				}

			}
		}
	}
}

