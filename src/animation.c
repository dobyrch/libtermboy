#include <stdlib.h>
#include "common.h"
#include "termboy.h"

static void *animate(void *arg);

/*
TODO: Add destructor
TODO: Consider abbreviating "animation"
*/
int tb_animation_init(struct tb_animation *animation, struct tb_sprite *sprite, int frames)
{
	animation->sprite = sprite;
	animation->frames = frames;
	animation->_frames = 0;
	animation->delays = calloc(frames, sizeof(int));
	animation->data = calloc(frames, sizeof(enum tb_color *));

	return 0;
}

void tb_animation_del(struct tb_animation *animation)
{
	free(animation->delays);
	free(animation->data);
}

int tb_animation_add_frame(struct tb_animation *animation, enum tb_color *colors, int delay_ms)
{
	if (animation->_frames == animation->frames)
		return -1;

	animation->delays[animation->_frames] = delay_ms;
	animation->data[animation->_frames] = colors;
	++animation->_frames;

	return 0;
}

int tb_animation_start(struct tb_animation *animation)
{
	pthread_create(&animation->_thread, NULL, animate, animation);
	pthread_detach(animation->_thread);

	return 0;
}

int tb_animation_stop(struct tb_animation *animation)
{
	struct tb_sprite sprite = *animation->sprite;
	enum tb_color *colors = animation->data[animation->_frames - 1];

	pthread_cancel(animation->_thread);
	TB_SPRITE_FILL(sprite, colors);
	tb_sprite_redraw(&sprite);

	return 0;
}

static void *animate(void *arg)
{
	struct tb_animation *animation = arg;
	struct tb_sprite sprite = *animation->sprite;
	enum tb_color *colors;
	int i;

	i = 0;
	while (1) {
		colors = animation->data[i];
		/* TODO: Assign pointer instead of copying? */
		TB_SPRITE_FILL(sprite, colors);
		tb_sprite_redraw(&sprite);

		tb_msleep(animation->delays[i]);
		i = (i + 1) % animation->frames;
	}

	return NULL;
}
