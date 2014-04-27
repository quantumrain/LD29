#include "Pch.h"
#include "Common.h"
#include "Game.h"

const wchar_t* gAppName = L"LD29 - Beneath the Surface";

random g_game_rand(1);
game g_game;

void GameInit() {
	g_game._player = spawn_entity(&g_game, new player(), vec2(10.0f, 9.0f));

	for(int j = 10; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			if (tile* t = g_game.get(i, j)) {
				t->type = TT_SOLID;
			}
		}
	}
}

void GameUpdate() {
	game* g = &g_game;

	vec2 target_cam_pos;

	static float target_cam_y;

	if (player* p = g->_player) {
		target_cam_pos = vec2(MAP_WIDTH * 0.5f, p->centre().y);

		float dy = target_cam_pos.y - target_cam_y;

		float hyst = 1.25f;

		if (fabsf(dy) > hyst) {
			if (dy > 0.0f)
				target_cam_y += dy - hyst;
			else
				target_cam_y += dy + hyst;
		}

		target_cam_pos.y = target_cam_y;
	}

	g->_cam_pos = lerp(g->_cam_pos, target_cam_pos, 0.2f);

	set_camera(g->_cam_pos, 10.0f);

	tick_entities(g);
	purge_entities(g);

	colour sky0(0.4f, 0.7f, 1.0f, 1.0f);
	colour sky1(0.45f, 0.75f, 1.0f, 1.0f);
	colour sky2(0.3f, 0.6f, 0.9f, 1.0f);
	colour sky3(0.35f, 0.65f, 0.9f, 1.0f);

	sky2 = sky2 * colour(0.75f, 1.0f);
	sky3 = sky3 * colour(0.75f, 1.0f);

	float sky_y = 10.0f;
	float sky_yf = 12.5f;
	float sky_yb = 16.0f;

	vec2 shift(0.0f, g->_cam_pos.y * 0.2f);

	for(int j = 0; j <= MAP_HEIGHT/2; j++) {
		for(int i = 0; i <= MAP_WIDTH/2; i++) {
			float x = i * 2 - 0.5f + shift.x;
			float y = j * 2 - 0.5f + shift.y;

			draw_tile(vec2(x, y), vec2(x + 2, y + 2), colour(0.15f, 0.15f, 0.35f, 1.0f), 128, 0);
		}
	}

	draw_rect(vec2(0.0f, sky_y), vec2(MAP_WIDTH, sky_yb), colour(0.0f, 1.0f), colour(0.0f, 1.0f), colour(0.0f), colour(0.0f));
	draw_rect(vec2(), vec2(MAP_WIDTH, sky_y), sky0, sky1, sky2, sky3);
	draw_rect(vec2(0.0f, sky_y), vec2(MAP_WIDTH, sky_yf), sky2, sky3, colour(0.0f), colour(0.0f));

	for(int j = 0; j < MAP_HEIGHT; j++) {
		for(int i = 0; i < MAP_WIDTH; i++) {
			int t = g->get_tile(i, j);

			if (t == TT_EMPTY) {
				float f = 1.0f / 8.0f;
				float a = 0.25f;

				bool xn = g->get_tile(i - 1, j) == TT_SOLID;
				bool xp = g->get_tile(i + 1, j) == TT_SOLID;
				bool yn = g->get_tile(i, j - 1) == TT_SOLID;
				bool yp = g->get_tile(i, j + 1) == TT_SOLID;

				if (xn) draw_rect(vec2(i, j), vec2(i + f, j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));
				if (xp) draw_rect(vec2(i + 1, j), vec2(i + 1 - f, j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));

				if (yn) draw_rect(vec2(i, j), vec2(i + 1, j + f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (yp) draw_rect(vec2(i, j + 1), vec2(i + 1, j + 1 - f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

				if (!xn && !yn && (g->get_tile(i - 1, j - 1) == TT_SOLID)) draw_tri(vec2(i, j),			vec2(i + f, j),			vec2(i, j + f),			colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yn && (g->get_tile(i + 1, j - 1) == TT_SOLID)) draw_tri(vec2(i + 1, j),		vec2(i + 1 - f, j),		vec2(i + 1, j + f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xn && !yp && (g->get_tile(i - 1, j + 1) == TT_SOLID)) draw_tri(vec2(i, j + 1),		vec2(i + f, j + 1),		vec2(i, j + 1 - f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yp && (g->get_tile(i + 1, j + 1) == TT_SOLID)) draw_tri(vec2(i + 1, j + 1),	vec2(i + 1 - f, j + 1),	vec2(i + 1, j + 1 - f),	colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
			}
			else {
				draw_tile(vec2(i, j), vec2(i + 1, j + 1), colour(), 128, 0);

				float f = 1.0f / 16.0f;
				float a = 0.85f;

				bool xn = g->get_tile(i - 1, j) == TT_EMPTY;
				bool xp = g->get_tile(i + 1, j) == TT_EMPTY;
				bool yn = g->get_tile(i, j - 1) == TT_EMPTY;
				bool yp = g->get_tile(i, j + 1) == TT_EMPTY;

				if (xn) draw_rect(vec2(i, j), vec2(i + f, j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));
				if (xp) draw_rect(vec2(i + 1, j), vec2(i + 1 - f, j + 1), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, a), colour(0.0f, 0.0f));

				if (yn) draw_rect(vec2(i, j), vec2(i + 1, j + f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (yp) draw_rect(vec2(i, j + 1), vec2(i + 1, j + 1 - f), colour(0.0f, a), colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));

				if (!xn && !yn && (g->get_tile(i - 1, j - 1) == TT_EMPTY)) draw_tri(vec2(i, j),			vec2(i + f, j),			vec2(i, j + f),			colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yn && (g->get_tile(i + 1, j - 1) == TT_EMPTY)) draw_tri(vec2(i + 1, j),		vec2(i + 1 - f, j),		vec2(i + 1, j + f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xn && !yp && (g->get_tile(i - 1, j + 1) == TT_EMPTY)) draw_tri(vec2(i, j + 1),		vec2(i + f, j + 1),		vec2(i, j + 1 - f),		colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
				if (!xp && !yp && (g->get_tile(i + 1, j + 1) == TT_EMPTY)) draw_tri(vec2(i + 1, j + 1),	vec2(i + 1 - f, j + 1),	vec2(i + 1, j + 1 - f),	colour(0.0f, a), colour(0.0f, 0.0f), colour(0.0f, 0.0f));
			}
		}
	}

	draw_entities(g);
}