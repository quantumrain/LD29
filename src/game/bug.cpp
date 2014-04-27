#include "Pch.h"
#include "Common.h"
#include "Game.h"

ivec2 determine_dir(game* g, ivec2 p) {
	u32 c = g->get_score(p.x, p.y);
	u32 u = g->get_score(p.x, p.y - 1);
	u32 d = g->get_score(p.x, p.y + 1);
	u32 l = g->get_score(p.x - 1, p.y);
	u32 r = g->get_score(p.x + 1, p.y);

	if ((c < u) && (c < d) && (c < l) && (c < r)) return ivec2();

	if (u < d) {
		if (l < r)
			return u < l ? ivec2(0, -1) : ivec2(-1, 0);
		else
			return u < r ? ivec2(0, -1) : ivec2(1, 0);
	}
	else {
		if (l < r)
			return d < l ? ivec2(0, 1) : ivec2(-1, 0);
		else
			return d < r ? ivec2(0, 1) : ivec2(1, 0);
	}
}

bug::bug() {
	_bb.max = vec2(10.0f / 16.0f, 6.0f / 16.0f);
	_sprite = 48;
	_draw_off = vec2(0.0f, -5.0f / 16.0f);
}

bug::~bug() {
}

void bug::tick(game* g) {
	_vel.x *= 0.75f;
	_vel.y = (_vel.y * 0.99f) + 0.0075f;

	vec2 c = centre();
	ivec2 ic = to_ivec2(c);
	vec2 d = to_vec2(ic) + vec2(0.5f) - c;
	ivec2 dir = determine_dir(g, ic);

	_vel += to_vec2(dir) * 0.01f;

	if (dir.x == 0) _vel.x += clamp(d.x * 0.05f, -0.01f, 0.01f);

	if (dir.x < 0) _draw_flags = DT_FLIP_X;
	else if (dir.x > 0) _draw_flags = 0;

	if (_on_ground)
		_vel.y -= 0.05f;

	_on_ground = false;

	avoid_others(g, this);
}

void bug::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _vel.x = min(_vel.x, 0.0f);
	if (clipped & CLIPPED_XN) _vel.x = max(_vel.x, 0.0f);
	if (clipped & CLIPPED_YP) { _vel.y = min(_vel.y, 0.0f); _on_ground = true; }
	if (clipped & CLIPPED_YN) _vel.y = max(_vel.y, 0.0f);
}

void bug::post_tick(game* g) {
}