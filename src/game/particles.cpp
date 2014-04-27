#include "Pch.h"
#include "Common.h"
#include "Game.h"

struct particle {
	aabb2 bb;
	vec2 vel;
	colour c;
	float s0, s1, s2;
	int t;
	int max_t;

	particle() : t(), max_t() { }
};

array<particle> g_particles;

void add_particle(vec2 pos, vec2 vel, colour c, float s0, float s1, float s2, int max_t) {
	if (particle* p = g_particles.push_back(particle())) {
		p->bb.min = pos - 0.01f;
		p->bb.max = pos + 0.01f;
		p->vel = vel;
		p->s0 = s0;
		p->s1 = s1;
		p->s2 = s2;
		p->c = c;
		p->max_t = max_t;
	}
}

void update_particles(game* g) {
	for(u32 i = 0; i < g_particles.size(); ) {
		particle* p = &g_particles[i];

		if (++p->t >= p->max_t) {
			g_particles.swap_erase(i);
			continue;
		}

		int clipped = 0;

		p->bb = cast_aabb(g, p->bb, p->vel, &clipped);

		if (clipped & CLIPPED_X) p->vel.x = -p->vel.x;
		if (clipped & CLIPPED_Y) p->vel.y = -p->vel.y;

		i++;
	}
}

void draw_particles(game* g) {
	for(u32 i = 0; i < g_particles.size(); i++) {
		particle* p = &g_particles[i];
		float f = (float)p->t / (float)p->max_t;
		vec2 pos = (p->bb.min + p->bb.max) * 0.5f;
		float size = 0.0f;

		if (f < 0.5f) size = lerp(p->s0, p->s1, f / 0.5f);
		else size = lerp(p->s1, p->s2, (f - 0.5f) / 0.5f);

		size *= 0.5f;

		draw_rect(pos - size, pos + size, p->c);
	}
}