#include "main.h"

#define MAX_SIZE 10

static Physics_World* s_world;

static double s_width;

static double s_height;

static int s_mouse_down;

static Vector s_mouse_position;

static Vector s_view_position;

static Physics_Body* s_grabbed_body;

static Physics_Body* s_mouse_body;

static Physics_Joint* s_mouse_joint;

static Rect s_main_camera;

static double s_zoom;

static double s_delta_time;

static double s_step_time;

static double s_total_error;

static double s_total_velocity;

static double s_update_fps;

static double s_render_fps;

enum
{
	BUTTON_PAUSE,

	BUTTON_STEP,

	BUTTON_RESTART,

	BUTTON_SIZE,

	BUTTON_WS,

	BUTTON_PBD,

	BUTTON_COUNT
};

static Button* s_buttons[BUTTON_COUNT];

static int s_size = 8;

static bool s_running = true;

static bool s_ws = true;

static bool s_pbd = true;

Vector get_mouse_world_position()
{
	Vector mouse_world = vector_scale(s_mouse_position, s_main_camera.max);

	mouse_world = vector_subtract_xy(mouse_world, s_main_camera.max.x / 2, s_main_camera.max.y / 2);

	mouse_world = vector_add_xy(mouse_world, 0, 300);

	mouse_world = vector_divide(mouse_world, s_zoom);

	mouse_world = vector_subtract(mouse_world, s_view_position);

	return mouse_world;
}

double get_wsize()
{
	return pow(s_size / (double)MAX_SIZE, 1.7);
}

int main()
{
	void* window = NULL;

	bool window_started = false;

	double update_delta = 1.0 / 120.0;

	double render_delta = 1.0 / 60.0;

	double max_latency = 0.1;

	double measure_delta = 1.0;

	double start_time = get_time();

	double last_update_time = start_time;

	double last_render_time = start_time;

	double last_measure_time = start_time;

	int update_count = 0;

	int render_count = 0;

	bool window_paused = false;

	Texture* font_texture = NULL;

	Window_Event event = { 0 };

	config_set_value(CONFIG_KEY_WINDOW_TITLE, "Demo Game");

	config_set_value(CONFIG_KEY_WINDOW_CLASS, "DemoGameWindowClass");

	config_set_value(CONFIG_KEY_FOLDER_NAME, "DemoGame");

	window_create(1280, 720);

	s_zoom = 0.7;

	s_mouse_down = -1;

	s_view_position.x += 180;

	s_view_position.y -= 190;

	config_set_value(CONFIG_KEY_TOUCH_ZOOM_ENABLED, (void*)true);

	s_buttons[BUTTON_PAUSE] = button_create("PAUSE", vector_create(0, 0), vector_create(140, 50));

	s_buttons[BUTTON_STEP] = button_create("STEP", vector_create(0, 0), vector_create(140, 50));
	
	s_buttons[BUTTON_RESTART] = button_create("RESTART", vector_create(0, 0), vector_create(140, 50));

	s_buttons[BUTTON_SIZE] = button_create("RESIZE", vector_create(0, 0), vector_create(140, 50));

	s_buttons[BUTTON_WS] = button_create("WS", vector_create(0, 0), vector_create(60, 50));

	s_buttons[BUTTON_WS]->toggle = true;

	s_buttons[BUTTON_WS]->clicked = s_ws;

	s_buttons[BUTTON_PBD] = button_create("PBD", vector_create(0, 0), vector_create(60, 50));

	s_buttons[BUTTON_PBD]->toggle = true;

	s_buttons[BUTTON_PBD]->clicked = s_pbd;

	while (window_is_open())
	{
		double current_time = get_time();

		if (current_time > last_update_time + update_delta)
		{
			input_update();

			while (window_poll_event(&event))
			{
				switch (event.type)
				{
					case WINDOW_EVENT_RESUMED:
					{
						if (window_started)
						{
							window_paused = false;
						}

						break;
					}
					case WINDOW_EVENT_PAUSED:
					{
						if (window_started)
						{
							window_paused = true;
						}

						break;
					}
					case WINDOW_EVENT_WINDOW_CREATED:
					{
						window = event.state_event.window;

						graphics_init(window);

						texture_create_from_file(&font_texture, "font.png");

						graphics_set_font(font_texture);

						if (!window_started)
						{
							window_started = true;

							initialize();
						}

						break;
					}
					case WINDOW_EVENT_WINDOW_DESTROYED:
					{
						graphics_uninit(window);

						window = NULL;

						break;
					}
					case WINDOW_EVENT_TOUCH_MOVE:
					{
						if (window != NULL)
						{
							double width = window_get_width(window);

							double height = window_get_height(window);

							Touch* touch = input_get_touch(event.touch_event.index);

							touch->x = event.touch_event.x / width;

							touch->y = 1 - event.touch_event.y / height;

							if (event.touch_event.index == s_mouse_down)
							{
								Vector mouse_position = vector_create(touch->x, touch->y);

								Vector mouse_delta = vector_subtract(mouse_position, s_mouse_position);

								s_mouse_position = mouse_position;

								bool movement_disabled = false;

								for (int i = 0; i < BUTTON_COUNT; i++)
								{
									movement_disabled |= s_buttons[i]->held;
								}

								if (!movement_disabled)
								{
									Vector delta = vector_divide(vector_scale(mouse_delta, s_main_camera.max), s_zoom);

									if (s_grabbed_body != NULL)
									{
										Vector mouse_world = get_mouse_world_position();

										if (s_grabbed_body->type != PHYSICS_BODY_TYPE_DYNAMIC || !s_running)
										{
											s_grabbed_body->position = vector_add(s_grabbed_body->position, mouse_world);

											s_grabbed_body->position = vector_subtract(s_grabbed_body->position, s_mouse_body->position );
										
											s_grabbed_body->world_transform_is_dirty = true;
										}

										s_mouse_body->position = mouse_world;

										s_mouse_body->world_transform_is_dirty = true;
									}
									else
									{
										s_view_position = vector_add(s_view_position, delta);
									}
								}
							}
						}

						break;
					}
					case WINDOW_EVENT_TOUCH_DOWN:
					{
						if (window != NULL)
						{
							double width = window_get_width(window);

							double height = window_get_height(window);

							Touch* touch = input_get_touch(event.touch_event.index);

							touch->x = event.touch_event.x / width;

							touch->y = 1 - event.touch_event.y / height;

							touch->down = true;

							s_mouse_down = event.touch_event.index;

							s_mouse_position = vector_create(touch->x, touch->y);

							s_grabbed_body = NULL;

							Vector mouse_world = get_mouse_world_position();

							for (List_Node* node = s_world->collider_list.first; node != NULL; node = node->next)
							{
								Physics_Collider* collider = node->item;

								if (shape_test_point(collider->world_shape, mouse_world))
								{
									s_grabbed_body = collider->body;

									if (s_mouse_body == NULL)
									{
										s_mouse_body = physics_body_create(s_world, PHYSICS_BODY_TYPE_STATIC);
									}

									if (s_mouse_joint != NULL)
									{
										physics_joint_destroy(s_mouse_joint);

										s_mouse_joint = NULL;
									}

									s_mouse_body->position = mouse_world;

									s_mouse_body->world_transform_is_dirty = true;

									if (s_grabbed_body->type == PHYSICS_BODY_TYPE_DYNAMIC)
									{
										s_mouse_joint = physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, s_mouse_body, mouse_world, s_grabbed_body, mouse_world);
									}
									
									break;
								}
							}
						}

						break;
					}
					case WINDOW_EVENT_TOUCH_UP:
					{
						if (window != NULL)
						{
							double width = window_get_width(window);

							double height = window_get_height(window);

							Touch* touch = input_get_touch(event.touch_event.index);

							touch->x = event.touch_event.x / width;

							touch->y = 1 - event.touch_event.y / height;

							touch->down = false;

							if (event.touch_event.index == s_mouse_down)
							{
								s_mouse_down = -1;

								s_grabbed_body = NULL;
							}
						}

						break;
					}
					case WINDOW_EVENT_TOUCH_LEAVE:
					{
						if (event.touch_event.index == s_mouse_down)
						{
							s_mouse_down = -1;

							s_grabbed_body = NULL;
						}

						break;
					}
					case WINDOW_EVENT_KEY_DOWN:
					{
						bool* key = input_get_key(event.key_event.key);

						*key = true;

						break;
					}
					case WINDOW_EVENT_KEY_UP:
					{
						bool* key = input_get_key(event.key_event.key);

						*key = false;

						break;
					}
					case WINDOW_EVENT_ZOOM:
					{
						s_zoom *= pow(1.1, event.zoom_event.amount);

						break;
					}
				}
			}

			if (window != NULL)
			{
				s_width = window_get_width(window);

				s_height = window_get_height(window);

				update(update_delta);
			}

			last_update_time = fmax(last_update_time + update_delta, current_time - max_latency);

			update_count++;
		}

		if (current_time > last_render_time + render_delta)
		{
			if (window != NULL)
			{
				graphics_clear(&(Color){ 0.0, 0.0, 0.0, 1.0 });

				render();

				graphics_display();
			}

			last_render_time = current_time;

			render_count++;
		}

		if (current_time > last_measure_time + measure_delta)
		{
			s_update_fps = update_count / (current_time - last_measure_time);

			s_render_fps = render_count / (current_time - last_measure_time);

			update_count = 0;

			render_count = 0;

			last_measure_time = current_time;
		}

		double sleep_duration = fmin(last_update_time + update_delta - current_time, last_render_time + render_delta - current_time);

		sleep(sleep_duration);
	}

	if (s_world != NULL)
	{
		physics_world_destroy(s_world);
	}

	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		button_destroy(s_buttons[i]);
	}

	return 0;
}

void initialize()
{
	random_set_seed(1);

	if (s_world != NULL)
	{
		s_mouse_joint = NULL;

		s_mouse_body = NULL;

		physics_world_destroy(s_world);
	}

	s_world = physics_world_create();

	s_world->gravity.y = -800;

	double wsize2 = get_wsize();

	double wsize = sqrt(wsize2);

	Physics_Body* ground = physics_body_create(s_world, PHYSICS_BODY_TYPE_STATIC);

	Shape* top = create_rect_shape(vector_create(-500*wsize, 1500*wsize-10), vector_create(500*wsize, 1500*wsize+10));

	physics_collider_create(ground, move_shape(top), 1.0);

	Shape* bottom = create_rect_shape(vector_create(-500*wsize, -10), vector_create(500*wsize, 10));

	physics_collider_create(ground, move_shape(bottom), 1.0);

	Shape* left_wall = create_rect_shape(vector_create(-500*wsize-10, 0), vector_create(-500*wsize+10, 1500*wsize));

	physics_collider_create(ground, move_shape(left_wall), 1.0);

	Shape* right_wall = create_rect_shape(vector_create(500*wsize-10, 0), vector_create(500*wsize+10, 1500*wsize));
	
	physics_collider_create(ground, move_shape(right_wall), 1.0);

	for (List_Node* collider_node = ground->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
	{
		Physics_Collider* collider = collider_node->item;

		collider->static_friction = 1;

		collider->dynamic_friction = 1;
	}

	for (int i = 0; i < 2500 * pow(wsize2, 1.2); i++)
	{
		Physics_Body* b = physics_body_create(s_world, PHYSICS_BODY_TYPE_DYNAMIC);

		b->position = vector_create(random_real_in_range(-450*wsize, 450*wsize), random_real_in_range(100*wsize, 1000*wsize));

		b->angle = 1;

		Physics_Collider* c;

		if (random_int_below(2) == 0)
		{
			double r = random_real_in_range(5, 15);

			Shape* s = shape_create_circle(vector_create(0, 0), r);

			c = physics_collider_create(b, move_shape(s), 1.0);
		}
		else
		{
			double w = random_real_in_range(10, 20);

			double h = random_real_in_range(10, 20);

			Shape* s = create_rect_shape(vector_create(-w / 2, -h / 2), vector_create(w / 2, h / 2));

			c = physics_collider_create(b, move_shape(s), 1.0);
		}

		c->static_friction = 1;

		c->dynamic_friction = 1;
	}

	if(true)
	{
		double l = 10, h = 2, r = 20;

		Vector v = vector_create(0, (1400+r)*wsize-r);

		Vector vv = vector_create(0, 0);

		Physics_Body* q = ground;

		for (int i = 0; i < 30 * wsize2; i++)
		{
			Physics_Body* b = physics_body_create(s_world, PHYSICS_BODY_TYPE_DYNAMIC);

			b->position = vector_add(v, vector_create(l / 2, 0));

			Shape* s = create_rect_shape(vector_create(-l / 2, -h / 2), vector_create(l / 2, h / 2));

			Physics_Collider* c = physics_collider_create(b, move_shape(s), 1.0);

			c->filter_group = -1;

			if (q == ground)
			{
				physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, q, v, b, v);
			}
			else
			{
				physics_joint_create_local(PHYSICS_JOINT_TYPE_PIN, q, vector_create(l / 2, 0), b, vector_create(-l / 2, 0));
			}

			physics_body_update_world_transform(b);

			q = b;

			vv = vector_add(v, vector_create(l, 0));

			v = vector_add(v, vector_create(l * 1, 0));
		}

		if(true)
		{
			Physics_Body* b = physics_body_create(s_world, PHYSICS_BODY_TYPE_DYNAMIC);

			b->position = vector_add(v, vector_create(r, 0));

			b->angle = M_PI / 2;

			Shape* s = shape_create_circle(vector_create(0, 0), r);

			Physics_Collider* c = physics_collider_create(b, move_shape(s), 1.0);

			(void)c;

			physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, q, vv, b, v);
		}
	}

	for(int t=0;t<3 *1;t++)
	{
		double offset_x = -1000.0*wsize;
		double offset_y = 1300*wsize - t * 550.0*wsize;

		// --- Rotating Square Container ---
		Physics_Body* container = physics_body_create(s_world, PHYSICS_BODY_TYPE_KINEMATIC);
		container->position = vector_create(offset_x, offset_y); // Center of the container
		container->angular_velocity = (1.0+t*1.5); // Rotate at 1 rad/s

		double size = 300.0*wsize;
		double thickness = 10.0;

		// Build the 4 walls of the square container
		Shape* wall_bottom = create_rect_shape(vector_create(-size / 2, -size / 2 - thickness), vector_create(size / 2, -size / 2));
		Shape* wall_top = create_rect_shape(vector_create(-size / 2, size / 2), vector_create(size / 2, size / 2 + thickness));
		Shape* wall_left = create_rect_shape(vector_create(-size / 2 - thickness, -size / 2), vector_create(-size / 2, size / 2));
		Shape* wall_right = create_rect_shape(vector_create(size / 2, -size / 2), vector_create(size / 2 + thickness, size / 2));

		physics_collider_create(container, move_shape(wall_bottom), 1.0);
		physics_collider_create(container, move_shape(wall_top), 1.0);
		physics_collider_create(container, move_shape(wall_left), 1.0);
		physics_collider_create(container, move_shape(wall_right), 1.0);

		for (List_Node* node = container->collider_list.first; node != NULL; node = node->next)
		{
			Physics_Collider* c = node->item;
			c->static_friction = 1.0;
			c->dynamic_friction = 1.0;
		}

		// --- 200 Dynamic Objects inside the container ---
		for (int i = 0; i < 200 * wsize2; i++)
		{
			Physics_Body* b = physics_body_create(s_world, PHYSICS_BODY_TYPE_DYNAMIC);

			// Spawn specifically within the container area
			b->position = vector_create(
				random_real_in_range(offset_x - size / 2, offset_x + size / 2),
				random_real_in_range(offset_y - size / 2, offset_y + size / 2)
			);

			b->angle = random_real_in_range(0, 6.28);

			Physics_Collider* c;
			if (random_int_below(2) == 0)
			{
				double r = random_real_in_range(5, 10);
				Shape* s = shape_create_circle(vector_create(0, 0), r);
				c = physics_collider_create(b, move_shape(s), 1.0);
			}
			else
			{
				double w = random_real_in_range(8, 15);
				double h = random_real_in_range(8, 15);
				Shape* s = create_rect_shape(vector_create(-w / 2, -h / 2), vector_create(w / 2, h / 2));
				c = physics_collider_create(b, move_shape(s), 1.0);
			}

			c->static_friction = 0.25;
			c->dynamic_friction = 0.25;
		}
	}
}

void update(double delta_time)
{
	bool step = s_running;

	step |= input_is_key_pressed(WINDOW_KEY_RIGHT);

	step |= input_is_key_down(WINDOW_KEY_SPACE);

	if (s_buttons[BUTTON_STEP]->clicked)
	{
		step = true;

		s_running = false;
	}

	if (input_is_key_pressed(WINDOW_KEY_ENTER) || s_buttons[BUTTON_PAUSE]->clicked)
	{
		s_running ^= true;

		s_buttons[BUTTON_PAUSE]->text = s_running ? "PAUSE" : "RESUME";
	}

	if (s_buttons[BUTTON_RESTART]->clicked)
	{
		initialize();
	}

	if (s_buttons[BUTTON_SIZE]->clicked)
	{
		double size1 = get_wsize();

		s_size++;

		if (s_size > MAX_SIZE)
		{
			s_size = 1;
		}

		double d = sqrt(get_wsize()) / sqrt(size1);

		s_zoom /= d;

		s_view_position = vector_multiply(s_view_position, d);

		initialize();
	}

	if (s_grabbed_body == NULL && s_mouse_joint != NULL)
	{
		physics_joint_destroy(s_mouse_joint);

		s_mouse_joint = NULL;
	}

	s_ws = s_buttons[BUTTON_WS]->clicked;

	s_pbd = s_buttons[BUTTON_PBD]->clicked;

	s_ws ^= input_is_key_pressed('W');

	s_pbd ^= input_is_key_pressed('P');

	s_buttons[BUTTON_WS]->clicked = s_ws;

	s_buttons[BUTTON_PBD]->clicked = s_pbd;

	if (step)
	{
		double time_before = get_time();

		physics_world_step(s_world, delta_time, s_ws, s_pbd);

		s_step_time = get_time() - time_before;

		s_delta_time = delta_time;
	}
	else
	{
		for (List_Node* body_node = s_world->body_list.first; body_node != NULL; body_node = body_node->next)
		{
			Physics_Body* body = body_node->item;

			physics_body_update_world_transform(body);
		}
	}

	s_total_error = 0;

	for (int i = 0; i < s_world->collision_count; i++)
	{
		if (s_world->collisions[i].body_1->type == PHYSICS_BODY_TYPE_STATIC && s_world->collisions[i].body_2->type == PHYSICS_BODY_TYPE_STATIC)
		{
			continue;
		}
			
		s_total_error += fabs(s_world->collisions[i].collision.depth);
	}

	for (List_Node* joint_node = s_world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
	{
		Physics_Joint* joint = joint_node->item;

		Vector displacement = vector_subtract(joint->world_anchor_2, joint->world_anchor_1);

		s_total_error += vector_length(displacement);
	}

	s_total_velocity = 0;

	for (List_Node* body_node = s_world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		Physics_Body* body = body_node->item;

		s_total_velocity += vector_length(body->linear_velocity);

		s_total_velocity += fabs(body->angular_velocity);
	}

	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		button_update(s_buttons[i]);
	}
}

void render()
{
	s_main_camera = (Rect){ 0, 0, fmax(s_width / s_height, 1) * 1100, fmax(s_height / s_width, 1) * 1100 };

	graphics_set_camera(&s_main_camera);

	graphics_save_transform();

	graphics_translate(vector_create(s_main_camera.max.x / 2, s_main_camera.max.y / 2));

	graphics_translate(vector_create(0, -300));

	graphics_scale_uniformly(s_zoom);

	graphics_translate(s_view_position);

	draw_physics_world(s_world);

	graphics_load_transform();

	graphics_set_camera(&(Rect){ 0, 0, s_width, s_height });

	s_buttons[BUTTON_PAUSE]->position = vector_create(s_width - 90, s_height - 55);

	s_buttons[BUTTON_STEP]->position = vector_create(s_width - 90, s_height - 125);

	s_buttons[BUTTON_RESTART]->position = vector_create(s_width - 90, s_height - 195);

	s_buttons[BUTTON_SIZE]->position = vector_create(s_width - 90, s_height - 265);

	s_buttons[BUTTON_WS]->position = vector_create(s_width - 130, s_height - 335);

	s_buttons[BUTTON_PBD]->position = vector_create(s_width - 50, s_height - 335);

	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		button_render(s_buttons[i]);
	}

	set_texture_and_color(NULL, &(Color){ 1.0, 1.0, 0.0, 1.0 });

	int h = 5, s = 20, m = 10;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "BC: %i", s_world->body_count);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "SC: %i", s_world->collider_count);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "JC: %i", s_world->joint_count);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "CC: %i", s_world->collision_count);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "ST: %.3f", s_step_time * 1e3);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "DT: %.3f", s_delta_time * 1e3);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "TT: %.3f", s_world->elapsed_time);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "E: %.2f", s_total_error);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, s_height - h - s, 0, s_height - h }, ALIGNMENT_LEFT, "V: %.0f", s_total_velocity);

	h += s + m;

	graphics_draw_format_in_rect(&(Rect) { 5, 0, 0, 20 }, ALIGNMENT_LEFT, "%3d %2d", lround(s_update_fps), lround(s_render_fps));
}
