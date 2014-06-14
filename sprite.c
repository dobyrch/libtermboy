#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "termboy.h"

#define SPRITES(x, y) (sprites[(y)*(background.width) + (x)])
#define COLOR(sprite, x, y) ((sprite).colors[(y)*(sprite.width) + (x)])

struct sprite_listnode
{
	struct tb_sprite *sprite;
	struct sprite_listnode *next;
};

static int sprite_insert(struct sprite_listnode **list, struct tb_sprite *sprite);
static int sprite_remove(struct sprite_listnode **list, struct tb_sprite *sprite);
static int redraw(int x, int y, int width, int height);

static struct tb_sprite background;
static struct sprite_listnode **sprites;

int tb_sprite_init(struct tb_sprite *sprite, int width, int height)
{
	/* TODO: check for invalid parameters */
	sprite->x = 0;
	sprite->y = 0;
	sprite->width = width;
	sprite->height = height;
	sprite->layer = 0;
	sprite->colors = calloc(sizeof(enum tb_color), width * height);

	return 0;
}

/*
TODO: Accept tb_sprite and point its background to the same buffer
in order to prevent the user from modifying the other attributes
*/

struct tb_sprite *tb_sprite_background(void)
{
	int width, height;
	tb_screen_size(&width, &height);

	background.x = 0;
	background.y = 0;
	background.width = width;
	background.height = height;
	background.layer = -1;
	background.colors = calloc(sizeof(enum tb_color), width * height);
	sprites = calloc(sizeof(struct sprite_listnote *), width * height);

	return &background;
}

/* TODO: Get lock when modifying layers */
int tb_sprite_add(struct tb_sprite *sprite)
{
	int i, j, x, y, width, height;
	x = sprite->x;
	y = sprite->y;
	width = sprite->width;
	height = sprite->height;
	/* TODO: bounds checking */

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			sprite_insert(&SPRITES(i, j), sprite);
		}
	}

	redraw(x, y, width, height);

	return 0;
}

int tb_sprite_move(struct tb_sprite *sprite, int new_x, int new_y)
{
	int i, j, x, y, width, height;
	x = sprite->x;
	y = sprite->y;
	width = sprite->width;
	height = sprite->height;

	if (x == new_x && y == new_y)
		return 0;

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			if (i >= new_x && i < new_x+width &&
			    j >= new_y && j < new_y+height)
				continue;
			sprite_remove(&SPRITES(i, j), sprite);
		}
	}

	for (i = new_x; i < new_x+width; ++i) {
		for (j = new_y; j < new_y+height; ++j) {
			if (i >=x && i < x+width &&
			    j >= y && j < y+height)
				continue;
			sprite_insert(&SPRITES(i, j), sprite);
		}
	}

	sprite->x = new_x;
	sprite->y = new_y;

	redraw(x, y, width, height);
	redraw(new_x, new_y, width, height);

	return 0;
}

int tb_sprite_redraw(void)
{
	return redraw(background.x, background.y, 
		      background.width, background.height);
}

static int redraw(int x, int y, int width, int height)
{
	int i, j;
	struct sprite_listnode *list;
	struct tb_sprite top_layer;

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			list = SPRITES(i, j);
			if (list == NULL) {
				tb_screen_put(i, j, COLOR(background, i, j));
			} else {
				/* TODO: Why can't this be dereferenced in the macro? */
				top_layer = *list->sprite;
				tb_screen_put(i, j, COLOR(top_layer,
						 i - top_layer.x,
						 j - top_layer.y));
			}
		}
	}
	tb_screen_flush();

	return 0;
}

static int sprite_insert(struct sprite_listnode **list, struct tb_sprite *sprite)
{
	struct sprite_listnode *curr_node, *next_node, *new_node;

	if (list == NULL)
		return -1;

	curr_node = *list;
	new_node = malloc(sizeof(struct sprite_listnode));
	new_node->sprite = sprite;
	new_node->next = NULL;

	if (curr_node == NULL) {
		*list = new_node;
		return 0;
	}

	if (sprite->layer >= curr_node->sprite->layer) {
		new_node->next = curr_node;
		*list = new_node;
		return 0;
	}

	while (curr_node->next != NULL) {
		next_node = curr_node->next;

		if (sprite->layer < curr_node->sprite->layer &&
		    sprite->layer >= next_node->sprite->layer) {
			curr_node->next = new_node;
			new_node->next = next_node;
			return 0;
		}

		curr_node = next_node;
	}

	curr_node->next = new_node;
	return 0;
}

static int sprite_remove(struct sprite_listnode **list, struct tb_sprite *sprite)
{
	struct sprite_listnode *curr_node, *next_node;

	if (list == NULL || *list == NULL)
		return -1;

	curr_node = *list;

	if (curr_node->sprite == sprite) {
		*list = curr_node->next;
		free(curr_node);
		return 0;
	}

	while (curr_node->next != NULL) {
		next_node = curr_node->next;

		if (next_node->sprite == sprite) {
			curr_node->next = next_node->next;
			free(next_node);
			return 0;
		}

		curr_node = next_node;
	}

	return -1;
}
