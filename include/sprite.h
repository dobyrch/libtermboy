struct tb_sprite {
	int x;
	int y;
	int width;
	int height;
	int layer;
	enum color *colors;
};

struct tb_sprite *tb_sprite_background(void);
int tb_sprite_init(struct tb_sprite *sprite, int width, int height);
int tb_sprite_add(struct tb_sprite *sprite);
int tb_sprite_move(struct tb_sprite *sprite, int x, int y);
int tb_sprite_redraw(void);
