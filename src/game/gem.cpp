#include "Pch.h"
#include "Common.h"
#include "Game.h"

gem::gem() : entity(ET_GEM) {
	_bb.max = vec2(4.0f / 16.0f, 4.0f / 16.0f);
	_draw_off = vec2(0.0f, -1.0f / 16.0f);
	_sprite = 64;
}

gem::~gem() {
}

void gem::tick(game* g) {
	_vel.x *= 0.75f;
	_vel.y = (_vel.y * 0.99f) + 0.0075f;
}

void gem::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _vel.x = min(_vel.x, 0.0f);
	if (clipped & CLIPPED_XN) _vel.x = max(_vel.x, 0.0f);
	if (clipped & CLIPPED_YP) _vel.y = min(_vel.y, 0.0f);
	if (clipped & CLIPPED_YN) _vel.y = max(_vel.y, 0.0f);
}

void gem::post_tick(game* g) {
	if (player* p = g->_player) {
		if (length_sq(p->centre() - centre()) < square(0.3f)) {
			p->_money += 1;
			SoundPlay(kSid_Switch, 0.5f, 1.5f);
			destroy();
		}
	}
}