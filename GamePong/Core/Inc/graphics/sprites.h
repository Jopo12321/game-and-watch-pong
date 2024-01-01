#ifndef INC_SPRITES_H_
#define INC_SPRITES_H_

#include <stdint.h>

#define SPRITE_SCALE			0b00001111
#define SPRITE_SHOW				0b00010000
#define SPRITE_FLIP_X			0b00100000
#define SPRITE_FLIP_Y			0b01000000
#define SPRITE_TRANSPOSE		0b10000000

#define SPRITE_COLLISION_N		0b00000001
#define SPRITE_COLLISION_E		0b00000010
#define SPRITE_COLLISION_S		0b00000100
#define SPRITE_COLLISION_W		0b00001000
#define SPRITE_TOUCH_N			0b00010000
#define SPRITE_TOUCH_E			0b00100000
#define SPRITE_TOUCH_S			0b01000000
#define SPRITE_TOUCH_W			0b10000000

typedef struct sprite {
	uint16_t x, y;
	uint8_t width, height;
	uint8_t settings;
	uint16_t *pallete;
	uint8_t *data;
} Sprite;

void init_sprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t width,
		uint8_t height, uint8_t settings, uint16_t *pallete, uint8_t *data);

Sprite* init_sprite_dynamicaly(uint16_t x, uint16_t y, uint8_t width, uint8_t height,
		uint8_t settings, uint16_t *pallete, uint8_t *data);

uint16_t get_sprite_x(Sprite sprite);
uint16_t get_sprite_y(Sprite sprite);
uint8_t get_sprite_width(Sprite sprite);
uint8_t get_sprite_height(Sprite sprite);
uint8_t get_sprite_scale(Sprite sprite);
int get_sprite_show(Sprite sprite);
int get_sprite_flip_x(Sprite sprite);
int get_sprite_flip_y(Sprite sprite);
int get_sprite_transpose(Sprite sprite);
uint16_t* get_sprite_pallete(Sprite sprite);
uint8_t* get_sprite_data(Sprite sprite);

void toogle_sprite_show(Sprite *sprite);
void toogle_sprite_flip_x(Sprite *sprite);
void toogle_sprite_flip_y(Sprite *sprite);
void toogle_sprite_transpose(Sprite *sprite);

void set_sprite_x(Sprite *sprite, uint16_t x);
void set_sprite_y(Sprite *sprite, uint16_t y);
void set_sprite_width(Sprite *sprite, uint8_t width);
void set_sprite_height(Sprite *sprite, uint8_t height);
void set_sprite_scale(Sprite *sprite, uint8_t scale);
void set_sprite_show(Sprite *sprite, int show);
void set_sprite_flip_x(Sprite *sprite, int flip_x);
void set_sprite_flip_y(Sprite *sprite, int flip_y);
void set_sprite_transpose(Sprite *sprite, int transpose);
void set_sprite_pallete(Sprite *sprite, uint16_t *pallete);
void set_sprite_data(Sprite *sprite, uint8_t *data);

void render_sprite(uint16_t *buf, Sprite sprite);
void render_sprites(uint16_t *buf, Sprite **list);

Sprite** sprites_init_list(Sprite *first_sprite);
Sprite** sprites_add_to_list(Sprite **list, Sprite *sprite);
Sprite** sprites_remove_from_list(Sprite **list, Sprite *sprite);
Sprite** sprites_clear_list(Sprite **list);
Sprite* sprites_get_from_list(Sprite **list, int idx);

int get_sprite_list_length(Sprite **list);

int check_sprites_collision(Sprite sprite1, Sprite sprite2);
int check_sprites_collision_quick(Sprite sprite1, Sprite sprite2);

#endif /* INC_SPRITES_H_ */
