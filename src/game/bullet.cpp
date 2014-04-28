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

bullet::bullet(bullet_type bul_type) : entity(ET_BULLET), _bul_type(bul_type) {
	_flags |= FLAG_CUSTOM_RENDER;
	_bb.max = vec2(4.0f / 16.0f, 4.0f / 16.0f);
	_draw_off = vec2(0.0f, -1.0f / 16.0f);
	_sprite = (bul_type == BT_NORMAL) ? 84 : 91;
	_time = 0;
	_anim = 0;
}

bullet::~bullet() {
}

void bullet::tick(game* g) {
	if (++_time > 60) {
		for(int i = 0; i < 3; i++) {
			colour c(1.0f, 1.0f, 1.0f, 1.0f);
			add_particle(centre() + _draw_off + g_game_rand.sv2rand(vec2(0.1f)), vec2(0.0f, 0.02f), c, 0.1f, 0.1f, 0.0f, 10);
		}

		destroy();
	}

	_anim++;
}

void bullet::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _time = 100;
	if (clipped & CLIPPED_XN) _time = 100;
	if (clipped & CLIPPED_YP) _time = 100;
	if (clipped & CLIPPED_YN) _time = 100;
}

void bullet::post_tick(game* g) {
	if (entity* e = find_entity(g, centre(), 0.4f)) {
		int dmg = 2;

		switch(_bul_type) {
			case BT_PLAYER:
				dmg = g->_plr_dmg;
				if (g_game_rand.rand(0, 4) == 0) dmg *= 2;
			break;

			case BT_SUPER:
				dmg = 8;
				if (g_game_rand.rand(0, 5) == 0) dmg *= 2;
			break;
		}

		((bug*)e)->on_attacked(g, dmg);
		destroy();
	}
}

void bullet::render(game* g) {
	int spr = _sprite;

	if (_bul_type == BT_PLAYER)
		spr = 85 + (_anim % 4);

	draw_tile(centre(), 1.0f, rotation_of(_vel), colour(), spr, _draw_flags);
}