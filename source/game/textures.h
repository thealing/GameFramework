#pragma once

#include "engine/graphics.h"

typedef struct Textures Textures;

struct Textures
{
	Texture* font;

	Texture* laser;

	Texture* saw_left;

	Texture* saw_right;

	Texture* mine_0;

	Texture* mine_1;

	Texture* mine_2;

	Texture* box;

	Texture* tire;

	Texture* lifter_base;

	Texture* lifter_sensor;

	Texture* booster[3];

	Texture* level_cave;

	Texture* level_tunnel;

	Texture* level_desert_cave;

	Texture* level_desert_tunnel;

	Texture* level_ramp;

	Texture* level_gap;

	Texture* level_bump;

	Texture* level_double_bump;

	Texture* level_triple_bump;

	Texture* level_smiley_face;

	Texture* level_frowny_face;

	Texture* level_neutral_face;

	Texture* level_blades;

	Texture* plank;

	Texture* garbage;

	Texture* car_monster_truck[2];

	Texture* car_dragster[2];

	Texture* car_go_cart[2];

	Texture* car_minibus[2];

	Texture* car_tractor[2];

	Texture* car_timber_lorry[2];

	Texture* car_garbage_truck[2];

	Texture* wheel_small[2];

	Texture* wheel_medium[2];

	Texture* wheel_monster[2];

	Texture* wheel_cycle[2];

	Texture* head_neck[2];

	Texture* head_only[2];

	Texture* timber_holder[2];

	Texture* garbage_lid[2];

	Texture* overlay;

	Texture* ui_checked;

	Texture* ui_unchecked;

	Texture* ui_arrow_pressed;

	Texture* ui_arrow_released;

	Texture* ui_arrow_disabled;

	Texture* ui_star_opaque;

	Texture* ui_star_translucent;

	Texture* ui_play_pressed;

	Texture* ui_play_released;

	Texture* ui_pause_pressed;

	Texture* ui_pause_released;

	Texture* ui_restart_pressed;

	Texture* ui_restart_released;

	Texture* ui_back_pressed;

	Texture* ui_back_released;

	Texture* ui_settings_pressed;

	Texture* ui_settings_released;

	Texture* ui_brake_pressed[2];

	Texture* ui_brake_released[2];

	Texture* ui_gas_pressed[2];

	Texture* ui_gas_released[2];
};

extern Textures g_textures;

void textures_init();
