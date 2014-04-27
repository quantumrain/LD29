#include "Pch.h"
#include "Common.h"
#include "Game.h"

const wchar_t* gAppName = L"LD29 - Beneath the Surface";

random g_game_rand(1);
game g_game;

void GameInit() {
	g_game._player = spawn_entity(&g_game, new player(), vec2(10.0f, 9.5f));

	for(int j = 10; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			if (tile* t = g_game.get(i, j)) {
				t->type = TT_SOLID;
			}
		}
	}
}

float mix(bool a, bool b, bool c, bool d) {
	return (a || b || c || d) ? 1.0f : 0.0f;
}

void GameUpdate() {
	game* g = &g_game;

	vec2 target_cam_pos;

	if (player* p = g->_player) {
		target_cam_pos = vec2(MAP_WIDTH * 0.5f, p->centre().y);

		float dy = target_cam_pos.y - g->_target_cam_y;

		float hyst = 1.25f;

		if (fabsf(dy) > hyst) {
			if (dy > 0.0f)
				g->_target_cam_y += dy - hyst;
			else
				g->_target_cam_y += dy + hyst;
		}

		target_cam_pos.y = g->_target_cam_y;
	}

	g->_cam_pos = lerp(g->_cam_pos, target_cam_pos, 0.2f);

	set_camera(g->_cam_pos, 10.0f);

	tick_entities(g);
	purge_entities(g);

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

			if (t == TT_EMPTY) {
				int hash = ((i * 7) ^ (j * 3)) + (i + j);
				hash ^= ((hash >> 3) * 9);

				int tile_num = 132 + (hash % 4);

				draw_tile(vec2((float)i, (float)j), vec2(i + 1.0f, j + 1.0f), bk_colour, tile_num, 0);
			}
		}
	}

	draw_rect(vec2(0.0f, 0.0f), vec2((float)MAP_WIDTH, sky_yb), colour(0.0f, 1.0f), colour(0.0f, 1.0f), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

	random sr(1);

	for(int i = 0; i < 300; i++) {
		vec2 pos = sr.v2rand(vec2(0.0f, 2.0f), vec2((float)MAP_WIDTH, sky_y));
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

			if (t == TT_EMPTY) {
				float f = 1.0f / 2.0f;
				float a = 0.5f;

				bool xn = g->get_tile(i - 1, j) == TT_SOLID;
				bool xp = g->get_tile(i + 1, j) == TT_SOLID;
				bool yn = g->get_tile(i, j - 1) == TT_SOLID;
				bool yp = g->get_tile(i, j + 1) == TT_SOLID;

				if (xn) draw_rect(vec2((float)i, (float)j), vec2((float)i + f, (float)j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));
				if (xp) draw_rect(vec2((float)i + 1, (float)j), vec2((float)i + 1 - f, (float)j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));

				if (yn) draw_rect(vec2((float)i, (float)j), vec2((float)i + 1, (float)j + f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (yp) draw_rect(vec2((float)i, (float)j + 1), vec2((float)i + 1, (float)j + 1 - f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

				if (!xn && !yn && (g->get_tile(i - 1, j - 1) == TT_SOLID)) draw_tri(vec2((float)i, (float)j),			vec2((float)i + f, (float)j),			vec2((float)i, (float)j + f),			colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yn && (g->get_tile(i + 1, j - 1) == TT_SOLID)) draw_tri(vec2((float)i + 1, (float)j),		vec2((float)i + 1 - f, (float)j),		vec2((float)i + 1, (float)j + f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xn && !yp && (g->get_tile(i - 1, j + 1) == TT_SOLID)) draw_tri(vec2((float)i, (float)j + 1),		vec2((float)i + f, (float)j + 1),		vec2((float)i, (float)j + 1 - f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yp && (g->get_tile(i + 1, j + 1) == TT_SOLID)) draw_tri(vec2((float)i + 1, (float)j + 1),	vec2((float)i + 1 - f, (float)j + 1),	vec2((float)i + 1, (float)j + 1 - f),	colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
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
			}
		}
	}

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			int t = g->get_tile(i, j);

			if (t == TT_EMPTY) {
				float f = 1.0f / 8.0f;
				float a = 1.0f / 16.0f;

				bool xn = g->get_tile(i - 1, j) == TT_SOLID;
				bool xp = g->get_tile(i + 1, j) == TT_SOLID;
				bool yn = g->get_tile(i, j - 1) == TT_SOLID;
				bool yp = g->get_tile(i, j + 1) == TT_SOLID;

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
		}
	}

	draw_entities(g);
}