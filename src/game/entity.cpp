#include "Pch.h"
#include "Common.h"
#include "Game.h"

// entity

entity::entity() : _flags(), _sprite(), _draw_flags(), _bb(0.0f, 1.0f) { }
entity::~entity() { }

void entity::destroy() { _flags |= FLAG_DESTROYED; }

void entity::tick(game* g) { }
void entity::on_hit_wall(game* g, int clipped) { }
void entity::post_tick(game* g) { }

// sys

void tick_entities(game* g) {
	for(auto e : g->_entities) {
		if (e->_flags & entity::FLAG_DESTROYED)
			continue;

		e->tick(g);

		int clipped = 0;

		e->_bb = cast_aabb(g, e->_bb, e->_vel, &clipped);

		if (clipped) {
			e->on_hit_wall(g, clipped);
		}

		e->post_tick(g);
	}
}

void purge_entities(game* g) {
	for(uint32_t i = 0; i < g->_entities.size(); ) {
		if (g->_entities[i]->_flags & entity::FLAG_DESTROYED)	g->_entities.swap_erase(i);
		else													i++;
	}
}

void draw_entities(game* g) {
	for(auto e : g->_entities) {
		if (e->_flags & entity::FLAG_DESTROYED)
			continue;

		//draw_rect(e->_bb.min, e->_bb.max, colour(0.2f, 0.2f));

		draw_tile(e->centre() + e->_draw_off, 0.5f, colour(), e->_sprite, e->_draw_flags);
	}
}

entity* spawn_entity(game* g, entity* ent, vec2 pos) {
	if (ent) {
		vec2 s((ent->_bb.max - ent->_bb.min) * 0.5f);

		ent->_bb.min = pos - s;
		ent->_bb.max = pos + s;
	}

	return g->_entities.push_back(ent);
}

// cast_aabb

namespace {
	float clip(float min0, float max0, float min1, float max1, float delta) {
		if ((delta > 0.0f) && (min0 >= max1)) return min(delta, min0 - max1);
		if ((delta < 0.0f) && (max0 <= min1)) return max(delta, max0 - min1);
		return delta;
	}

	float clip_x(aabb2 a, aabb2 b, float vx) { return overlaps_y(a, b) ? clip(a.min.x, a.max.x, b.min.x, b.max.x, vx) : vx; }
	float clip_y(aabb2 a, aabb2 b, float vy) { return overlaps_x(a, b) ? clip(a.min.y, a.max.y, b.min.y, b.max.y, vy) : vy; }
}

aabb2 cast_aabb(game* g, aabb2 self, vec2 delta, int* clipped) {
	aabb2 bounds(expand_toward(self, delta));

	int x0 = fast_floor(bounds.min.x);
	int y0 = fast_floor(bounds.min.y);
	int x1 = fast_floor(bounds.max.x);
	int y1 = fast_floor(bounds.max.y);

	vec2 clipped_delta = delta;

	for(int x = x0; x <= x1; x++) {
		for(int y = y0; y <= y1; y++) {
			if (g->get_tile(x, y) != TT_EMPTY) {
				clipped_delta.x = clip_x(aabb2(vec2((float)x, (float)y), vec2((float)(x + 1), (float)(y + 1))), self, clipped_delta.x);
			}
		}
	}

	self.min.x += clipped_delta.x;
	self.max.x += clipped_delta.x;

	for(int x = x0; x <= x1; x++) {
		for(int y = y0; y <= y1; y++) {
			if (g->get_tile(x, y) != TT_EMPTY) {
				clipped_delta.y = clip_y(aabb2(vec2((float)x, (float)y), vec2((float)(x + 1), (float)(y + 1))), self, clipped_delta.y);
			}
		}
	}

	self.min.y += clipped_delta.y;
	self.max.y += clipped_delta.y;

	if (clipped) {
		*clipped = 0;
		if (delta.x != clipped_delta.x) *clipped |= (delta.x > 0.0f) ? CLIPPED_XP : CLIPPED_XN;
		if (delta.y != clipped_delta.y) *clipped |= (delta.y > 0.0f) ? CLIPPED_YP : CLIPPED_YN;
	}

	return self;
}

// avoid

void avoid_others(game* g, entity* self) {
	for(auto e : g->_entities) {
		if (e == self) continue; // todo: flag for player to avoid avoiding him!

		vec2 delta = self->centre() - e->centre();

		float dist = 0.25f + 0.25f;

		if (length_sq(delta) > square(dist))
			continue;

		float l = length(delta);
		vec2 v = delta / l;

		if (l < 0.00001f)
			v = g_game_rand.sv2rand(1.0f);

		v *= ((dist - l) / dist) * 2.0f;

		self->_vel += v * DT;
		//e->_vel -= v * DT;
	}
}