#include "Pch.h"
#include "Common.h"
#include "Game.h"

const wchar_t* gAppName = L"LD29 - Tunnel Defense";

random g_game_rand(1);
game g_game;
bool g_title = true;

void GameInit() {
	g_game_rand = random(GetTickCount());

	g_game._player = spawn_entity(&g_game, new player(), vec2(10.0f, 9.5f));

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			if (tile* t = g_game.get(i, j)) {
				if (j < 10) {
					t->type =  ((i == 0) || (i == MAP_WIDTH - 1)) ? TT_VOID : TT_EMPTY;
				}
				else if (j < (MAP_HEIGHT - 2)) {
					t->type = ((i == 0) || (i == MAP_WIDTH - 1)) ? TT_WALL : TT_SOLID;

					if (g_game_rand.rand(0, 10) == 0) {
						t->ore = 1;
					}
				}
				else
					t->type = TT_WALL;
			}
		}
	}
}

float mix(bool a, bool b, bool c, bool d) {
	return (a || b || c || d) ? 1.0f : 0.0f;
}

void update_search(game* g, ivec2 start) {
	u32 open[MAP_WIDTH * MAP_HEIGHT];
	int num_open = 0;

	int sx = clamp(start.x, 0, MAP_WIDTH - 1);
	int sy = clamp(start.y, 0, MAP_HEIGHT - 1);

	open[num_open++] = (sy * MAP_WIDTH) + sx;

	for(int i = 0; i < (MAP_WIDTH * MAP_HEIGHT); i++)
		g->_map[i].search = 0xFFFFFFFF;

	if (tile* t = g->get(sx, sy))
		t->search = 0;

	const int COST_SHIFT = 12;
	const int COST_MASK = (1 << COST_SHIFT) - 1;

	tile* tiles = g->_map;

	while(num_open > 0) {
		std::pop_heap(&open[0], &open[num_open], [](u32 a, u32 b) { return b < a; });

		int c = open[--num_open];
		int x = (c & COST_MASK) % MAP_WIDTH;
		int y = (c & COST_MASK) / MAP_WIDTH;
		u32 cost = c >> COST_SHIFT;

		int xd[4] = { -1, 1, 0, 0 };
		int yd[4] = { 0, 0, -1, 1 };

		for(int j = 0; j < 4; j++) {
			int nx = x + xd[j];
			int ny = y + yd[j];
			int new_cost = cost + 1;

			if ((nx < 0) || (ny < 0) || (nx >= MAP_WIDTH) || (ny >= MAP_HEIGHT))
				continue;

			tile* t = &tiles[(ny * MAP_WIDTH) + nx];

			if (t->search != 0xFFFFFFFF)
				continue;

			if (t->type == TT_TURRET) new_cost += 50;
			else if (t->type != TT_EMPTY) new_cost += 100;

			t->search = new_cost;

			open[num_open++] = ((ny * MAP_WIDTH) + nx) | (new_cost << COST_SHIFT);

			std::push_heap(&open[0], &open[num_open], [](u32 a, u32 b) { return b < a; });
		}
	}
}

void GameUpdate() {
	game* g = &g_game;

	if (is_key_pressed(KEY_RESET)) {
		memset(g->_map, 0, sizeof(g->_map));

		g->_entities.free();
		g->_player = 0;

		g->_cam_pos = vec2(MAP_WIDTH * 0.5f, 8.5f);
		g->_target_cam_y = 8.5f;
		g->_spawn_time = 800;
		g->_diff = 1.0f;

		GameInit();

		g_title = true;
	}

	if (g_title) {
		set_camera(vec2(), 10.0f);

		float ratio = g_WinSize.y / (float)g_WinSize.x;
		vec2 orig(-10.0f, -10.0f * ratio);

		draw_string(vec2(0.0f, -4.0f), 0.15f, TEXT_CENTRE, colour(0.5f, 0.5f, 0.8f, 1.0f), "Tunnel Defense");
		draw_string(vec2(0.0f, -2.5f), 0.05f, TEXT_CENTRE, colour(0.3f, 0.3f, 0.6f, 1.0f), "LD29 - Beneath the Surface");

		float y = 0.5f;

		draw_string(vec2(0.0f, y), 0.05f, TEXT_CENTRE, colour(0.3f, 0.6f, 0.3f, 1.0f), "Move + Aim - \001\002\003\004"); y += 0.5f;
		draw_string(vec2(0.0f, y), 0.05f, TEXT_CENTRE, colour(0.3f, 0.6f, 0.3f, 1.0f), "Jump - Z"); y += 0.5f;
		draw_string(vec2(0.0f, y), 0.05f, TEXT_CENTRE, colour(0.3f, 0.6f, 0.3f, 1.0f), "Zap Block - X"); y += 0.5f;
		draw_string(vec2(0.0f, y), 0.05f, TEXT_CENTRE, colour(0.3f, 0.6f, 0.3f, 1.0f), "Place Turret - C"); y += 0.75f;
		draw_string(vec2(0.0f, y), 0.05f, TEXT_CENTRE, colour(0.15f, 0.3f, 0.15f, 1.0f), "Alternate Controls - WASD, I, O, P"); y += 0.5f;

		draw_string(vec2(0.0f, 4.0f), 0.05f, TEXT_CENTRE, colour(0.5f, 0.5f, 0.5f, 1.0f), "press SPACE to START");


		if (is_key_pressed(KEY_FIRE) || is_key_pressed(KEY_ALT_FIRE)) g_title = false;

		return;
	}

	if (player* p = g->_player) {
		vec2 target_cam_pos = vec2(MAP_WIDTH * 0.5f, p->centre().y);

		float dy = target_cam_pos.y - g->_target_cam_y;

		float hyst = 1.25f;

		if (fabsf(dy) > hyst) {
			if (dy > 0.0f)
				g->_target_cam_y += dy - hyst;
			else
				g->_target_cam_y += dy + hyst;
		}

		target_cam_pos.y = g->_target_cam_y;

		update_search(g, to_ivec2(p->centre()));

		g->_cam_pos = lerp(g->_cam_pos, target_cam_pos, 0.2f);
	}

	set_camera(g->_cam_pos, 15.0f);

	if (--g->_spawn_time <= 0) {
		vec2 pos(g_game_rand.frand(1.0f, MAP_WIDTH - 1.0f), 2.0f);

		if (bug* e = spawn_entity(&g_game, new bug(), pos)) {
		}

		g->_diff += 0.02f;
		g->_spawn_time = 120 - (int)(g->_diff * 10.0f);
	}

	tick_entities(g);
	purge_entities(g);
	update_particles(g);

	colour sky0(0.4f, 0.7f, 1.0f, 1.0f);
	colour sky1(0.45f, 0.75f, 1.0f, 1.0f);
	colour sky2(0.3f, 0.6f, 0.9f, 1.0f);
	colour sky3(0.35f, 0.65f, 0.9f, 1.0f);
	
	sky0 *= colour(0.65f, 0.0f);
	sky1 *= colour(0.65f, 0.0f);
	sky2 *= colour(0.35f, 1.0f);
	sky3 *= colour(0.35f, 1.0f);

	float sky_space = 4.5f;
	float sky_top = 4.75f;
	float sky_y = 10.0f;
	float sky_yf = 10.5f;
	float sky_yb = 11.0f;

	colour master_colour(0.6f, 0.9f, 0.1f, 1.0f);
	colour tile_colour(master_colour); tile_colour *= colour(1.0f, 1.0f);
	colour edge_colour(master_colour);
	colour bk_colour(master_colour); bk_colour *= colour(0.2f, 1.0f);

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			int t = g->get_tile(i, j);

			if (t == TT_EMPTY || t == TT_TURRET) {
				int hash = ((i * 7) ^ (j * 3)) + (i + j);
				hash ^= ((hash >> 3) * 9);

				int tile_num = 132 + (hash % 4);

				draw_tile(vec2((float)i, (float)j), vec2(i + 1.0f, j + 1.0f), bk_colour, tile_num, 0);
			}
		}
	}

	draw_rect(vec2(0.0f, 0.0f), vec2((float)MAP_WIDTH, sky_yb), colour(0.0f, 1.0f), colour(0.0f, 1.0f), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

	random sr(1);

	for(int i = 0; i < 600; i++) {
		vec2 pos = sr.v2rand(vec2(0.0f, 0.0f), vec2((float)MAP_WIDTH, sky_y));
		float f = sr.frand();
		float s = lerp(0.01f, 0.02f, f);
		colour c(lerp(0.3f, 2.0f, f) * square(1.0f - square(pos.y / sky_y)), 1.0f);

		draw_rect(pos - s, pos + s, c);
	}

	draw_rect(vec2(0.0f, sky_space), vec2((float)MAP_WIDTH, sky_top), colour(0.0f, 1.0f), colour(0.0f, 1.0f), sky0, sky1);
	draw_rect(vec2(0.0f, sky_top), vec2((float)MAP_WIDTH, sky_y), sky0, sky1, sky2, sky3);
	draw_rect(vec2(0.0f, sky_y), vec2((float)MAP_WIDTH, sky_yf), sky2, sky3, colour(0.0f, 1.0f), colour(0.0f, 1.0f));
	draw_rect(vec2(0.0f, sky_yf), vec2((float)MAP_WIDTH, sky_yb), colour(0.0f, 1.0f), colour(0.0f, 1.0f), colour(0.0f), colour(0.0f));

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			int t = g->get_tile(i, j);

			if (t == TT_EMPTY || t == TT_VOID || t == TT_TURRET) {
				float f = 1.0f / 2.0f;
				float a = 0.5f;

				bool xn = g->is_roughable(i - 1, j);
				bool xp = g->is_roughable(i + 1, j);
				bool yn = g->is_roughable(i, j - 1);
				bool yp = g->is_roughable(i, j + 1);

				if (xn) draw_rect(vec2((float)i, (float)j), vec2((float)i + f, (float)j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));
				if (xp) draw_rect(vec2((float)i + 1, (float)j), vec2((float)i + 1 - f, (float)j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));

				if (yn) draw_rect(vec2((float)i, (float)j), vec2((float)i + 1, (float)j + f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (yp) draw_rect(vec2((float)i, (float)j + 1), vec2((float)i + 1, (float)j + 1 - f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

				if (!xn && !yn && g->is_roughable(i - 1, j - 1)) draw_tri(vec2((float)i, (float)j),			vec2((float)i + f, (float)j),			vec2((float)i, (float)j + f),			colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yn && g->is_roughable(i + 1, j - 1)) draw_tri(vec2((float)i + 1, (float)j),		vec2((float)i + 1 - f, (float)j),		vec2((float)i + 1, (float)j + f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xn && !yp && g->is_roughable(i - 1, j + 1)) draw_tri(vec2((float)i, (float)j + 1),		vec2((float)i + f, (float)j + 1),		vec2((float)i, (float)j + 1 - f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yp && g->is_roughable(i + 1, j + 1)) draw_tri(vec2((float)i + 1, (float)j + 1),	vec2((float)i + 1 - f, (float)j + 1),	vec2((float)i + 1, (float)j + 1 - f),	colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
			}
			else {
				bool c0 = g->get_tile(i - 1, j - 1) == TT_EMPTY;
				bool c1 = g->get_tile(i,     j - 1) == TT_EMPTY;
				bool c2 = g->get_tile(i + 1, j - 1) == TT_EMPTY;

				bool c3 = g->get_tile(i - 1, j    ) == TT_EMPTY;
				bool c4 = g->get_tile(i,     j    ) == TT_EMPTY;
				bool c5 = g->get_tile(i + 1, j    ) == TT_EMPTY;

				bool c6 = g->get_tile(i - 1, j + 1) == TT_EMPTY;
				bool c7 = g->get_tile(i,     j + 1) == TT_EMPTY;
				bool c8 = g->get_tile(i + 1, j + 1) == TT_EMPTY;

				int hash = ((i * 7) ^ (j * 3)) + (i + j);
				hash ^= ((hash >> 3) * 9);

				int tile_num = 128 + (hash % 4);

				//tile_colour *= colour(0.75f, 1.0f);

				draw_tile(vec2((float)i, (float)j), vec2((float)i + 1, (float)j + 1),
						tile_colour * colour(mix(c0, c1, c3, c4), 1.0f),
						tile_colour * colour(mix(c1, c2, c4, c5), 1.0f),
						tile_colour * colour(mix(c3, c4, c6, c7), 1.0f),
						tile_colour * colour(mix(c4, c5, c7, c8), 1.0f),
					tile_num, (c0 + c8) < (c2 + c6) ? DT_ALT_TRI : 0);

				if (tile* d = g->get(i, j)) {
					if (d->ore) {
						int flags = 0;

						flags |= (hash & 16) ? DT_FLIP_X : 0;
						flags |= (hash & 32) ? DT_FLIP_Y : 0;

						draw_tile(vec2((float)i, (float)j), vec2((float)i + 1, (float)j + 1), colour(0.75f, 0.2f, 0.2f, 1.0f), 160 + (hash % 3), flags); 
					}
				}
			}
		}
	}

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			if (!g->is_roughable(i, j)) {
				float f = 1.0f / 8.0f;
				float a = 1.0f / 16.0f;

				bool xn = g->is_roughable(i - 1, j);
				bool xp = g->is_roughable(i + 1, j);
				bool yn = g->is_roughable(i, j - 1);
				bool yp = g->is_roughable(i, j + 1);

				int edge_tile = ((i + j) & 1) ? 144 : 145;

				if (xn) {
					draw_tile(vec2((float)i - 1 + a, (float)j), vec2((float)i - 0 + a, (float)j + 1), tile_colour, edge_tile + 2, DT_ROT_270 | DT_FLIP_X);
					draw_tile(vec2((float)i - 1 + a, (float)j), vec2((float)i - 0 + a, (float)j + 1), edge_colour, edge_tile, DT_ROT_270 | DT_FLIP_X);
				}

				if (xp) {
					draw_tile(vec2((float)i + 1 - a, (float)j), vec2((float)i + 2 - a, (float)j + 1), tile_colour, edge_tile + 2, DT_ROT_270);
					draw_tile(vec2((float)i + 1 - a, (float)j), vec2((float)i + 2 - a, (float)j + 1), edge_colour, edge_tile, DT_ROT_270);
				}

				if (yn) {
					draw_tile(vec2((float)i, (float)j - 1 + a), vec2((float)i + 1, (float)j - 0 + a), tile_colour, edge_tile + 2, DT_FLIP_Y);
					draw_tile(vec2((float)i, (float)j - 1 + a), vec2((float)i + 1, (float)j - 0 + a), edge_colour, edge_tile, DT_FLIP_Y);
				}

				if (yp) {
					draw_tile(vec2((float)i, (float)j + 1 - a), vec2((float)i + 1, (float)j + 2 - a), tile_colour, edge_tile + 2, 0);
					draw_tile(vec2((float)i, (float)j + 1 - a), vec2((float)i + 1, (float)j + 2 - a), edge_colour, edge_tile, 0);
				}
			}

			/*if (tile* t = g->get(i, j)) {
				float s = clamp(t->search / 30.0f, 0.01f, 0.45f);
				float l = 0.5f - s;
				float h = 0.5f + s;
				draw_rect(vec2(i+l, j+l), vec2(i+h, j+h), colour(0.25f, 0.0f));
			}*/
		}
	}

	draw_entities(g);
	draw_particles(g);

	if (player* p = g->_player) {
		float ratio = g_WinSize.y / (float)g_WinSize.x;

		vec2 orig(-15.0f, -15.0f * ratio); orig += g->_cam_pos;

		draw_string(orig + vec2(0.2f, 0.2f), 0.02f, TEXT_LEFT, colour(0.6f, 1.0f), "Health");

		for(int i = 0; i < 8; i++) {
			vec2 pt = orig + vec2(i * 0.4f, 0.0f) + vec2(0.2f, 0.4f);

			float f = clamp(p->_flash_health / 2.0f, 0.0f, 1.0f);

			draw_rect(pt - vec2(0.025f), pt + vec2(0.325f, 0.425f), colour(f, f, f, 1.0f));
			draw_rect(pt, pt + vec2(0.3f, 0.4f), (i < p->_health) ? colour(0.7f, 0.3f, 0.1f, 1.0f) : colour(0.2f, 0.2f, 0.2f, 1.0f));
		}

		draw_string(orig + vec2(0.2f, 1.0f), 0.02f, TEXT_LEFT, colour(0.6f, 1.0f), "Metal");

		for(int i = 0; i < 8; i++) {
			vec2 pt = orig + vec2(i * 0.4f, 0.0f) + vec2(0.2f, 1.2f);

			float f = clamp(p->_flash_money / 2.0f, 0.0f, 1.0f);

			draw_rect(pt - vec2(0.025f), pt + vec2(0.325f, 0.425f), colour(f, f, f, 1.0f));
			draw_rect(pt, pt + vec2(0.3f, 0.4f), (i < p->_money) ? colour(0.2f, 0.2f, 0.6f, 1.0f) : colour(0.2f, 0.2f, 0.2f, 1.0f));
		}
	}
}