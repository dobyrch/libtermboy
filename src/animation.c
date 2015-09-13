#include <stdlib.h>
#include "common.h"
#include "termboy.h"

static void *animate(void *arg);

/*
TODO: Add destructor
*/
int tb_anim_init(struct tb_anim *anim, struct tb_sprite *sprite, int frames)
{
	anim->sprite = sprite;
	anim->frames = frames;
	anim->_frames = 0;
	anim->delays = calloc(frames, sizeof(int));
	anim->data = calloc(frames, sizeof(enum tb_color *));

	return 0;
}

void tb_anim_del(struct tb_anim *anim)
{
	free(anim->delays);
	free(anim->data);
}

int tb_anim_add_frame(struct tb_anim *anim, enum tb_color *colors, int delay_ms)
{
	if (anim->_frames == anim->frames)
		return -1;

	anim->delays[anim->_frames] = delay_ms;
	anim->data[anim->_frames] = colors;
	++anim->_frames;

	return 0;
}

int tb_anim_start(struct tb_anim *anim)
{
	pthread_create(&anim->_thread, NULL, animate, anim);
	pthread_detach(anim->_thread);

	return 0;
}

int tb_anim_stop(struct tb_anim *anim)
{
	struct tb_sprite sprite = *anim->sprite;
	enum tb_color *colors = anim->data[anim->_frames - 1];

	pthread_cancel(anim->_thread);
	TB_SPRITE_FILL(sprite, colors);
	tb_sprite_redraw(&sprite);

	return 0;
}

static void *animate(void *arg)
{
	struct tb_anim *anim = arg;
	struct tb_sprite sprite = *anim->sprite;
	enum tb_color *colors;
	int i;

	i = 0;
	while (1) {
		colors = anim->data[i];
		/* TODO: Assign pointer instead of copying? */
		TB_SPRITE_FILL(sprite, colors);
		tb_sprite_redraw(&sprite);

		tb_msleep(anim->delays[i]);
		i = (i + 1) % anim->_frames;
	}

	return NULL;
}
