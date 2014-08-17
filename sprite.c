#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "termboy.h"

#define SPRITES(x, y) (sprites[(y)*(background.width) + (x)])
#define ON_SCREEN(x, y) ((x) >= 0 && (x) < background.width &&  \
			    (y) >= 0 && (y) < background.height)

struct sprite_listnode
{
	struct tb_sprite *sprite;
	struct sprite_listnode *next;
};

/*
TODO: Make sprite first param to be consistent with method calling convention
i.e. instance.method(args...) => method(instance, args...)
*/
static int sprite_insert(int x, int y, struct tb_sprite *sprite);
static int sprite_remove(int x, int y, struct tb_sprite *sprite);
static int redraw(int x, int y, int width, int height);

static struct tb_sprite background;
static struct sprite_listnode **sprites;
static pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t move_lock = PTHREAD_MUTEX_INITIALIZER;

int tb_sprite_init(struct tb_sprite *sprite, int width, int height)
{
	/* TODO: Check for invalid parameters */
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
(Or remove the concept of a 'background' entirely and let user
initialize it as an ordinary sprite)
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

/* TODO: Rename to 'tb_sprite_show' and add a 'tb_sprite_hide' function */
int tb_sprite_add(struct tb_sprite *sprite)
{
	int i, j, x, y, width, height;
	x = sprite->x;
	y = sprite->y;
	width = sprite->width;
	height = sprite->height;

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			sprite_insert(i, j, sprite);
		}
	}

	redraw(x, y, width, height);

	return 0;
}

int tb_sprite_move(struct tb_sprite *sprite, int new_x, int new_y)
{
	int i, j, x, y, width, height;
	pthread_mutex_lock(&move_lock);

	x = sprite->x;
	y = sprite->y;
	width = sprite->width;
	height = sprite->height;

	if (x == new_x && y == new_y)
		goto success;

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			if (i >= new_x && i < new_x+width &&
			    j >= new_y && j < new_y+height)
				continue;
			sprite_remove(i, j, sprite);
		}
	}

	for (i = new_x; i < new_x+width; ++i) {
		for (j = new_y; j < new_y+height; ++j) {
			if (i >=x && i < x+width &&
			    j >= y && j < y+height)
				continue;
			sprite_insert(i, j, sprite);
		}
	}

	sprite->x = new_x;
	sprite->y = new_y;

	redraw(x, y, width, height);
	redraw(new_x, new_y, width, height);

success:
	pthread_mutex_unlock(&move_lock);
	return 0;
}

/* TODO: Specify sprite to redraw */
int tb_sprite_redraw(void)
{
	return redraw(background.x, background.y,
		      background.width, background.height);
}

static int redraw(int x, int y, int width, int height)
{
	int i, j;
	struct sprite_listnode *list;
	struct tb_sprite *top_layer;

	for (i = x; i < x+width; ++i) {
		for (j = y; j < y+height; ++j) {
			if (!ON_SCREEN(i, j))
				continue;

			list = SPRITES(i, j);
			while (list != NULL && TB_SPRITE_COLOR(*list->sprite,
						i - list->sprite->x,
						j - list->sprite->y)
					== TB_COLOR_TRANSPARENT) {
				list = list->next;
			}
			if (list == NULL) {
				tb_screen_put(i, j, TB_SPRITE_COLOR(background,
							i, j));
			} else {
				top_layer = list->sprite;
				tb_screen_put(i, j, TB_SPRITE_COLOR(*top_layer,
							i - top_layer->x,
							j - top_layer->y));
			}
		}
	}
	tb_screen_flush();

	return 0;
}

static int sprite_insert(int x, int y, struct tb_sprite *sprite)
{
	struct sprite_listnode *curr_node, *next_node, *new_node;
	pthread_mutex_lock(&list_lock);

	if (!ON_SCREEN(x, y))
		goto failure;

	curr_node = SPRITES(x, y);
	new_node = malloc(sizeof(struct sprite_listnode));
	new_node->sprite = sprite;
	new_node->next = NULL;

	if (curr_node == NULL) {
		SPRITES(x, y) = new_node;
		goto success;
	}

	if (sprite->layer >= curr_node->sprite->layer) {
		new_node->next = curr_node;
		SPRITES(x, y) = new_node;
		goto success;
	}

	while (curr_node->next != NULL) {
		next_node = curr_node->next;

		if (sprite->layer < curr_node->sprite->layer &&
		    sprite->layer >= next_node->sprite->layer) {
			curr_node->next = new_node;
			new_node->next = next_node;
			goto success;
		}

		curr_node = next_node;
	}

	curr_node->next = new_node;

success:
	pthread_mutex_unlock(&list_lock);
	return 0;

failure:
	pthread_mutex_unlock(&list_lock);
	return -1;
}

static int sprite_remove(int x, int y, struct tb_sprite *sprite)
{
	struct sprite_listnode *curr_node, *next_node;
	pthread_mutex_lock(&list_lock);

	if (!ON_SCREEN(x, y))
		goto failure;

	curr_node = SPRITES(x, y);

	if (curr_node == NULL)
		goto failure;

	if (curr_node->sprite == sprite) {
		SPRITES(x, y) = curr_node->next;
		free(curr_node);
		goto success;
	}

	while (curr_node->next != NULL) {
		next_node = curr_node->next;

		if (next_node->sprite == sprite) {
			curr_node->next = next_node->next;
			free(next_node);
			goto success;
		}

		curr_node = next_node;
	}

failure:
	pthread_mutex_unlock(&list_lock);
	return -1;

success:
	pthread_mutex_unlock(&list_lock);
	return 0;
}
