#include "Pch.h"
#include "Common.h"
#include "Game.h"

player::player() : _on_ground(false), _last_clipped(), _jump_ground(), _ground_time(), _anim() {
	_bb.max = vec2(4.0f / 16.0f, 12.0f / 16.0f);
	_draw_off = vec2(0.0f, -2.0f / 16.0f);
}

player::~player() {
}

void player::tick(game* g) {
	_vel.x *= _on_ground ? 0.5f : 0.75f;
	_vel.y = (_vel.y * 0.98f) + 0.0075f;

	if (_on_ground)	_ground_time++;
	else			_ground_time = 0;

	if (_ground_time > 2)		_jump_ground = 8;
	else if (_jump_ground > 0)	_jump_ground--;

	bool is_moving = false;
	bool is_firing = (_ground_time > 2) && is_key_pressed(KEY_FIRE);

	if (is_firing) {
		if (is_key_pressed(KEY_LEFT)) _draw_flags = DT_FLIP_X;
		if (is_key_pressed(KEY_RIGHT)) _draw_flags = 0;

		ivec2 p = to_ivec2(centre());

		if (is_key_pressed(KEY_DOWN))
			p.y++;
		else
			p.x += (_draw_flags & DT_FLIP_X) ? -1 : 1;

		if (tile* t = g->get(p.x, p.y)) {
			t->type = TT_EMPTY;
		}
	}
	else {
		float move_speed = _on_ground ? 0.05f : 0.025f;

		if (is_key_pressed(KEY_LEFT)) {
			_draw_flags = DT_FLIP_X;
			is_moving = true;
			_vel.x -= move_speed;

			if (_last_clipped & CLIPPED_XN) {
				if (_vel.y > 0.0f)
					_vel.y *= 0.95f;
			}
		}

		if (is_key_pressed(KEY_RIGHT)) {
			_draw_flags = 0;
			is_moving = true;
			_vel.x += move_speed;

			if (_last_clipped & CLIPPED_XP) {
				if (_vel.y > 0.0f)
					_vel.y *= 0.95f;
			}
		}

		if (is_key_pressed(KEY_UP)) {
			if (_jump_ground > 0) {
				_vel.y = -0.205f;
				_jump_ground = 0;
			}
		}
	}

	_anim++;

	if (_on_ground) {
		if (is_firing) {
			_sprite = 8 + ((_anim / 4) % 3);
		}
		else {
			if (is_moving) {
				_sprite = 4 + ((_anim / 6) % 4);
			}
			else {
				_sprite = ((_anim / 5) % 4);
			}
		}
	}
	else {
		_sprite = 6;
	}

	_on_ground = false;
	_last_clipped = 0;
}

void player::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) _vel.x = min(_vel.x, 0.0f);
	if (clipped & CLIPPED_XN) _vel.x = max(_vel.x, 0.0f);

	if (clipped & CLIPPED_YP) {
		_vel.y = min(_vel.y, 0.0f);
		_on_ground = true;
	}

	if (clipped & CLIPPED_YN) {
		if (_vel.y < 0.0f)
			_vel.y += 0.01f;
	}

	_last_clipped = clipped;
}

void player::post_tick(game* g) {
}