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

bug::bug() : entity(ET_BUG), _damage(), _on_ground() {
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

	ivec2 tp = ic + dir;

	if (tile* t = g->get(tp.x, tp.y)) {
		if ((t->type == TT_SOLID) || (t->type == TT_TURRET)) {
			if (++t->damage >= t->max_damage()) {
				if (t->type == TT_SOLID) {
					t->damage = 0;
					t->type = TT_EMPTY;
				}
				else if (t->type == TT_TURRET) {
					if (t->owner) {
						t->owner->destroy();
					}

					t->damage = 0;
					t->type = TT_EMPTY;
					t->owner = 0;
				}
			}
			else {
				if (t->owner) t->owner->on_attacked(g);
			}
		}
	}

	_on_ground = false;

	avoid_others(g, this);

	if (player* p = g->_player) {
		if (length_sq(p->centre() - centre()) < square(0.4f)) {
			for(int i = 0; i < 10; i++) {
				colour c(1.0f, 0.2f, 0.2f, 1.0f);
				add_particle(centre() + _draw_off + g_game_rand.sv2rand(vec2(0.3f)), vec2(0.0f, 0.02f), c, 0.3f, 0.3f, 0.0f, 10);
			}

			p->_health--;
			SoundPlay(kSid_Switch, 1.5f, 1.5f);
			destroy();
		}
	}
}

void bug::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _vel.x = min(_vel.x, 0.0f);
	if (clipped & CLIPPED_XN) _vel.x = max(_vel.x, 0.0f);
	if (clipped & CLIPPED_YP) { _vel.y = min(_vel.y, 0.0f); _on_ground = true; }
	if (clipped & CLIPPED_YN) _vel.y = max(_vel.y, 0.0f);
}

void bug::post_tick(game* g) {
}