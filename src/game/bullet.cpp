#include "Pch.h"
#include "Common.h"
#include "Game.h"

entity* find_entity(game* g, vec2 p, float radius) {
	entity* best = 0;
	float best_d = square(radius);

	for(uint32_t i = 0; i < g->_entities.size(); i++) {
		entity* e = g->_entities[i];

		if (e->_flags & entity::FLAG_DESTROYED) continue;
		if (e->_type != ET_BUG) continue;

		float d = length_sq(e->centre() - p);

		if (d < best_d) {
			best = e;
			best_d = d;
		}
	}

	return best;
}

bullet::bullet() : entity(ET_BULLET) {
	_flags |= FLAG_CUSTOM_RENDER;
	_bb.max = vec2(4.0f / 16.0f, 4.0f / 16.0f);
	_draw_off = vec2(0.0f, -1.0f / 16.0f);
	_sprite = 84;
	_time = 0;
}

bullet::~bullet() {
}

void bullet::tick(game* g) {
	if (++_time > 60) destroy();
}

void bullet::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _time = 100;
	if (clipped & CLIPPED_XN) _time = 100;
	if (clipped & CLIPPED_YP) _time = 100;
	if (clipped & CLIPPED_YN) _time = 100;
}

void bullet::post_tick(game* g) {
	if (entity* e = find_entity(g, centre(), 0.4f)) {
		((bug*)e)->on_attacked(g);
		destroy();
	}
}

void bullet::render(game* g) {
	draw_tile(centre(), 1.0f, rotation_of(_vel), colour(), 84, _draw_flags);
}