#include "Pch.h"
#include "Common.h"
#include "Game.h"

entity* find_entity_target(game* g, vec2 p, float radius) {
	float rsq = square(radius);

	entity* best = 0;
	u32 best_s = 0xFFFFFFFF;

	for(uint32_t i = 0; i < g->_entities.size(); i++) {
		entity* e = g->_entities[i];

		if (e->_flags & entity::FLAG_DESTROYED) continue;
		if (e->_type != ET_BUG) continue;

		vec2 c = e->centre();

		if (length_sq(c - p) < rsq) {
			ivec2 tp = to_ivec2(c);
			u32 s = g->get_score(tp.x, tp.y);

			if (s < best_s) {
				if (!raycast(g, p, c)) {
					best = e;
					best_s = s;
				}
			}
		}
	}

	return best;
}

turret::turret() : entity(ET_TURRET) {
	_flags |= FLAG_CUSTOM_RENDER;
	_bb.max = vec2(0.95f, 0.95f);
	_draw_off = vec2(0.0f, 0.0f);
	_sprite = 80;
	_rot = g_game_rand.sfrand(PI);
	_rot_v = 0.0f;
	_rot_t = g_game_rand.rand(2, 6);
	_recoil = 0.0f;
}

turret::~turret() {
}

void turret::spawned(game* g) {
	ivec2 p = to_ivec2(centre());

	if (tile* t = g->get(p.x, p.y)) {
		if (t->owner != 0)
			destroy();
		else {
			t->owner = this;
			t->type = TT_TURRET;
		}
	}
}

void turret::tick(game* g) {
	entity* target = find_entity_target(g, centre(), 8.0f);

	if (_reload > 0) _reload--;
	_recoil *= 0.75f;

	if (target) {
		_rot_v = 0.0f;
		_rot_t = 0;

		float rt = rotation_of(target->centre() - centre());
		float rd = normalise_radians(rt - _rot);

		_rot = normalise_radians(_rot + clamp(rd * 0.5f, -0.3f, 0.3f));

		if (_reload <= 0) {
			if (fabsf(rd) < 0.2f) {
				if (entity* e = spawn_entity(g, new bullet, centre())) {
					e->_vel = rotation(rt) * 0.5f;
					_reload = 20;
					_recoil = 1.0f;
					SoundPlay(kSid_TurretFire, g_game_rand.frand(0.9f, 1.1f), g_game_rand.frand(0.2f, 0.4f));
				}
			}
		}
	}
	else {
		_rot = normalise_radians(_rot + _rot_v);

		if (--_rot_t <= 0) {
			_rot_v = g_game_rand.sfrand(0.1f);
			_rot_t = g_game_rand.rand(20, 40);
		}
	}

	if (_flash_t > 0) _flash_t--;
}

void turret::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _vel.x = min(_vel.x, 0.0f);
	if (clipped & CLIPPED_XN) _vel.x = max(_vel.x, 0.0f);
	if (clipped & CLIPPED_YP) _vel.y = min(_vel.y, 0.0f);
	if (clipped & CLIPPED_YN) _vel.y = max(_vel.y, 0.0f);
}

void turret::on_attacked(game* g) {
	if (_flash_t <= 0) {
		_flash_t = 6;
		SoundPlay(kSid_TurretHurt, g_game_rand.frand(0.9f, 1.1f), g_game_rand.frand(0.6f, 0.7f));
	}
}

void turret::post_tick(game* g) {
}

void turret::render(game* g) {
	int f = ((_flash_t == 5) || (_flash_t == 6)) ? 2 : 0;

	draw_tile(centre(), 0.5f, colour(), _sprite + f, _draw_flags);

	vec2 d(rotation(_rot));

	draw_tile(centre() + d * ((3.0f / 16.0f) - _recoil * 0.2f), 1.0f, _rot, colour(), 81 + f, _draw_flags);
}