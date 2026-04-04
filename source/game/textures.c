#include "textures.h"

Textures g_textures;

static void create_blue_red_textures(Texture* textures[2], const char* path)
{
	Image* image = image_load(path);

	texture_create_from_image(&textures[0], image);

	for (int i = 0; i < image->height; i++)
	{
		for (int j = 0, k = image->width - 1; j <= k; j++, k--)
		{
			Pixel* left_pixel = &image->pixels[i * image->width + j];

			Pixel* right_pixel = &image->pixels[i * image->width + k];

			Pixel temp = *left_pixel;

			left_pixel->red = right_pixel->blue;

			left_pixel->green = right_pixel->green;

			left_pixel->blue = right_pixel->red;

			left_pixel->alpha = right_pixel->alpha;

			right_pixel->red = temp.blue;

			right_pixel->green = temp.green;

			right_pixel->blue = temp.red;

			right_pixel->alpha = temp.alpha;
		}
	}

	texture_create_from_image(&textures[1], image);

	image_destroy(image);
}

void textures_init()
{
	texture_create_from_file(&g_textures.font, "images/font.png");

	texture_create_from_file(&g_textures.laser, "images/laser.png");

	texture_resize(g_textures.laser, vector_create(1300, 4));

	texture_create_from_file(&g_textures.saw_left, "images/saw_left.png");

	texture_create_from_file(&g_textures.saw_right, "images/saw_right.png");

	texture_resize(g_textures.saw_left, vector_create(2.2, 2.2));

	texture_resize(g_textures.saw_right, vector_create(2.2, 2.2));

	texture_create_from_file(&g_textures.mine_0, "images/mine_0.png");

	texture_create_from_file(&g_textures.mine_1, "images/mine_1.png");

	texture_create_from_file(&g_textures.mine_2, "images/mine_2.png");

	texture_resize(g_textures.mine_0, vector_create(50, 25));

	texture_resize(g_textures.mine_1, vector_create(50, 25));

	texture_resize(g_textures.mine_2, vector_create(50, 25));

	texture_create_from_file(&g_textures.box, "images/box.png");

	texture_resize(g_textures.box, vector_create(44, 44));

	texture_create_from_file(&g_textures.tire, "images/tire.png");

	texture_resize(g_textures.tire, vector_create(2.1, 2.1));

	texture_create_from_file(&g_textures.lifter_base, "images/lifter_base.png");

	texture_create_from_file(&g_textures.lifter_sensor, "images/lifter_sensor.png");

	texture_align(g_textures.lifter_base, ALIGNMENT_CENTER, ALIGNMENT_BOTTOM);

	texture_align(g_textures.lifter_sensor, ALIGNMENT_CENTER, ALIGNMENT_BOTTOM);

	texture_create_from_file(&g_textures.booster[0], "images/booster_0.png");

	texture_create_from_file(&g_textures.booster[1], "images/booster_1.png");

	texture_create_from_file(&g_textures.booster[2], "images/booster_2.png");

	texture_create_from_file(&g_textures.level_cave, "images/levels/cave.png");

	texture_create_from_file(&g_textures.level_tunnel, "images/levels/tunnel.png");

	texture_create_from_file(&g_textures.level_desert_cave, "images/levels/desert_cave.png");

	texture_create_from_file(&g_textures.level_desert_tunnel, "images/levels/desert_tunnel.png");

	texture_create_from_file(&g_textures.level_ramp, "images/levels/ramp.png");

	texture_create_from_file(&g_textures.level_gap, "images/levels/gap.png");

	texture_create_from_file(&g_textures.level_bump, "images/levels/bump.png");

	texture_create_from_file(&g_textures.level_double_bump, "images/levels/double_bump.png");

	texture_create_from_file(&g_textures.level_triple_bump, "images/levels/triple_bump.png");

	texture_create_from_file(&g_textures.level_smiley_face, "images/levels/smiley_face.png");

	texture_create_from_file(&g_textures.level_frowny_face, "images/levels/frowny_face.png");

	texture_create_from_file(&g_textures.level_neutral_face, "images/levels/neutral_face.png");

	texture_create_from_file(&g_textures.level_blades, "images/levels/blades.png");

	texture_create_from_file(&g_textures.plank, "images/cars/plank.png");

	texture_create_from_file(&g_textures.garbage, "images/cars/garbage.png");

	create_blue_red_textures(g_textures.car_monster_truck, "images/cars/monster_truck.png");

	create_blue_red_textures(g_textures.car_dragster, "images/cars/dragster.png");

	create_blue_red_textures(g_textures.car_go_cart, "images/cars/go_cart.png");

	create_blue_red_textures(g_textures.car_minibus, "images/cars/minibus.png");

	create_blue_red_textures(g_textures.car_tractor, "images/cars/traktor.png");

	create_blue_red_textures(g_textures.car_timber_lorry, "images/cars/timber_lorry.png");

	create_blue_red_textures(g_textures.car_garbage_truck, "images/cars/garbage_truck.png");

	create_blue_red_textures(g_textures.wheel_small, "images/wheels/small.png");

	create_blue_red_textures(g_textures.wheel_medium, "images/wheels/medium.png");

	create_blue_red_textures(g_textures.wheel_monster, "images/wheels/monster.png");

	create_blue_red_textures(g_textures.wheel_cycle, "images/wheels/cycle.png");

	create_blue_red_textures(g_textures.head_neck, "images/head_neck.png");

	create_blue_red_textures(g_textures.head_only, "images/head_only.png");

	texture_resize(g_textures.head_neck[0], vector_create(32, 32));

	texture_resize(g_textures.head_neck[1], vector_create(32, 32));

	texture_resize(g_textures.head_only[0], vector_create(32, 32));

	texture_resize(g_textures.head_only[1], vector_create(32, 32));

	create_blue_red_textures(g_textures.timber_holder, "images/cars/timber_holder.png");

	create_blue_red_textures(g_textures.garbage_lid, "images/cars/garbage_lid.png");

	texture_create_from_file(&g_textures.overlay, "images/overlay.png");

	texture_create_from_file(&g_textures.ui_checked, "images/ui/checked.png");

	texture_create_from_file(&g_textures.ui_unchecked, "images/ui/unchecked.png");

	texture_create_from_file(&g_textures.ui_arrow_pressed, "images/ui/arrow_pressed.png");

	texture_create_from_file(&g_textures.ui_arrow_released, "images/ui/arrow_released.png");

	texture_create_from_file(&g_textures.ui_arrow_disabled, "images/ui/arrow_disabled.png");

	texture_create_from_file(&g_textures.ui_star_opaque, "images/ui/star_opaque.png");

	texture_create_from_file(&g_textures.ui_star_translucent, "images/ui/star_translucent.png");

	g_textures.ui_star_opaque->center.y = 56;

	g_textures.ui_star_translucent->center.y = 56;

	texture_resize(g_textures.ui_star_opaque, vector_create(50, 50));

	texture_resize(g_textures.ui_star_translucent, vector_create(50, 50));

	texture_create_from_file(&g_textures.ui_play_pressed, "images/ui/play_pressed.png");

	texture_create_from_file(&g_textures.ui_play_released, "images/ui/play_released.png");

	texture_create_from_file(&g_textures.ui_pause_pressed, "images/ui/pause_pressed.png");

	texture_create_from_file(&g_textures.ui_pause_released, "images/ui/pause_released.png");

	texture_create_from_file(&g_textures.ui_restart_pressed, "images/ui/restart_pressed.png");

	texture_create_from_file(&g_textures.ui_restart_released, "images/ui/restart_released.png");

	texture_create_from_file(&g_textures.ui_back_pressed, "images/ui/back_pressed.png");

	texture_create_from_file(&g_textures.ui_back_released, "images/ui/back_released.png");

	texture_create_from_file(&g_textures.ui_settings_pressed, "images/ui/settings_pressed.png");

	texture_create_from_file(&g_textures.ui_settings_released, "images/ui/settings_released.png");

	create_blue_red_textures(g_textures.ui_brake_pressed, "images/ui/left_pressed.png");

	create_blue_red_textures(g_textures.ui_brake_released, "images/ui/left_released.png");

	create_blue_red_textures(g_textures.ui_gas_pressed, "images/ui/right_pressed.png");

	create_blue_red_textures(g_textures.ui_gas_released, "images/ui/right_released.png");
}
