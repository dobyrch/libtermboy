#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "termboy.h"

struct sprite_listnode
{
	struct tb_sprite *sprite;
	struct sprite_listnode *next;
};

static enum tb_color sprite_color(struct tb_sprite *sprite, int x, int y);

static struct sprite_listnode *sprites;
static pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t move_lock = PTHREAD_MUTEX_INITIALIZER;

int tb_sprite_init(struct tb_sprite *sprite, int width, int height)
{
	/* TODO: Check that sprite is non-NULL */
	/* TODO: Use unsigned for dimensions */
	sprite->x = 0;
	sprite->y = 0;
	sprite->width = width;
	sprite->height = height;
	sprite->layer = 0;
	sprite->tile = TB_TILE_NONE;
	sprite->colors = calloc(sizeof(enum tb_color), width * height);

	if (sprite->colors == NULL)
		return -1;

	return 0;
}

/* TODO: Add a 'tb_sprite_hide' function */
int tb_sprite_show(struct tb_sprite *sprite)
{
	struct sprite_listnode *curr_node, *next_node, *new_node;
	pthread_mutex_lock(&list_lock);

	curr_node = sprites;
	new_node = malloc(sizeof(struct sprite_listnode));
	if (new_node == NULL)
		goto failure;

	new_node->sprite = sprite;
	new_node->next = NULL;

	if (curr_node == NULL) {
		sprites = new_node;
		goto success;
	}

	if (sprite->layer >= curr_node->sprite->layer) {
		new_node->next = curr_node;
		sprites = new_node;
		goto success;
	}

	while (curr_node->next != NULL) {
		next_node = curr_node->next;

		if (sprite->layer < curr_node->sprite->layer &&
		    sprite->layer >= next_node->sprite->layer) {
			new_node->next = next_node;
			curr_node->next = new_node;
			goto success;
		}

		curr_node = next_node;
	}

	curr_node->next = new_node;

success:
	pthread_mutex_unlock(&list_lock);
	tb_sprite_redraw(sprite);
	return 0;

failure:
	pthread_mutex_unlock(&list_lock);
	return -1;
}

int tb_sprite_move(struct tb_sprite *sprite, int new_x, int new_y)
{
	pthread_mutex_lock(&move_lock);
	sprite->x = new_x;
	sprite->y = new_y;
	/* TODO: redraw old position */
	tb_sprite_redraw(sprite);
	pthread_mutex_unlock(&move_lock);

	return 0;
}

int tb_sprite_redraw(struct tb_sprite *sprite)
{
	int left, right, top, bottom, maxwidth, maxheight, x, y;
	struct sprite_listnode *curr_node;
	struct tb_sprite *curr_sprite;
	enum tb_color color;

	/* TODO: Factor out this duplicated block */
	left = sprite->x;
	right = left + sprite->width;
	top = sprite->y;
	bottom = top + sprite->height;
	tb_screen_size(&maxwidth, &maxheight);

	if (sprite->tile & TB_TILE_HORIZONTAL) {
		left = 0;
		right = maxwidth;
	}
	if (sprite->tile & TB_TILE_VERTICAL) {
		top = 0;
		bottom = maxheight;
	}

	for (y = top; y < bottom; ++y) {
		for (x = left; x < right; ++x) {
			curr_node = sprites;
			while (curr_node != NULL) {
				curr_sprite = curr_node->sprite;
				color = sprite_color(curr_sprite, x, y);
				if (color != TB_COLOR_TRANSPARENT) {
					tb_screen_put(x, y, color);
					break;
				}
				curr_node = curr_node->next;
			}
		}
	}

	tb_screen_flush();

	return 0;
}

enum tb_color sprite_color(struct tb_sprite *sprite, int x, int y)
{
	int left, right, top, bottom;

	left = sprite->x;
	right = left + sprite->width;
	top = sprite->y;
	bottom = top + sprite->height;

	if (sprite->tile & TB_TILE_HORIZONTAL) {
		while (x < left)
			x += sprite->width;
		while (x >= right)
			x -= sprite->width;
	}
	if (sprite->tile & TB_TILE_VERTICAL) {
		while (y < top)
			y += sprite->height;
		while (y >= bottom)
			y -= sprite->height;
	}

	if (x >= left && x < right && y >= top && y < bottom)
		return TB_SPRITE_COLOR(*sprite, x - left, y - top);

	return TB_COLOR_TRANSPARENT;
}
