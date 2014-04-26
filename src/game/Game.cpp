#include "Pch.h"
#include "Common.h"
#include "Game.h"

random g_game_rand(1);

const wchar_t* gAppName = L"AppTitle";

void GameInit() {
}

void GameUpdate() {
	static float a;

	a += (gKeyDown[KEY_LEFT] != 0) * 0.1f;
	a -= (gKeyDown[KEY_RIGHT] != 0) * 0.1f;

	set_camera(vec2(), 10.0f);
	set_camera_3d(camera_look_at(vec3(cosf(a), 1, sinf(a)) * 10.0f, vec3(0, 0, 0), vec3(0, 1, 0)), 1.0f);

	draw_string(vec2(0.0f, 0.0f), vec2(0.1f), TEXT_CENTRE, colour(), "Test");

	g_game_rand.reseed(1);

	for(int i = 0; i < 10; i++) {
		draw_tri_3d(g_game_rand.v3rand(5.0f), g_game_rand.v3rand(5.0f), g_game_rand.v3rand(5.0f), colour(g_game_rand.frand(1), g_game_rand.frand(1), g_game_rand.frand(1), 1.0f));
	}
}