#include "Pch.h"
#include "Common.h"
#include "Game.h"

const int JUMP_GROUND_GRACE = 8;
const int TIME_TILL_GROUNDED = 4;
const int JUMP_LAUNCH_TIME = 8;

player::player() : _on_ground(false), _last_clipped(), _jump_latch(), _jump_ground(), _jump_launch(), _wall_left(), _wall_right(), _wall_dragging(), _ground_time(), _anim() {
	_bb.max = vec2(4.0f / 16.0f, 12.0f / 16.0f);
	_draw_off = vec2(0.0f, -2.0f / 16.0f);
}

player::~player() {
}

void player::tick(game* g) {
	_vel.x *= _on_ground ? 0.75f : 0.95f;
	_vel.y = (_vel.y * 0.99f) + 0.0075f;

	if (_on_ground)	_ground_time++;
	else			_ground_time = 0;

	if (_ground_time > TIME_TILL_GROUNDED)	_jump_ground = JUMP_GROUND_GRACE;
	else if (_jump_ground > 0)				_jump_ground--;

	bool is_moving = false;
	bool is_firing = (_ground_time > TIME_TILL_GROUNDED) && is_key_pressed(KEY_FIRE);

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
		if (is_key_pressed(KEY_UP)) {
			if (_on_ground) {
				if (_jump_ground > 0) {
					_vel.y = -0.075f;
					_jump_ground = 0;
					_jump_launch = JUMP_LAUNCH_TIME;
					_jump_latch = true;
					_on_ground = false;
				}
			}
			else {
				if (!_jump_latch) {
					if (_wall_left > 0)			_vel.x += 0.05f;
					else if (_wall_right > 0)	_vel.x -= 0.05f;

					if ((_wall_left > 0) || (_wall_right > 0)) {
						_vel.y = -0.075f;
						_jump_ground = 0;
						_jump_launch = JUMP_LAUNCH_TIME;
						_jump_latch = true;
						_wall_left = 0;
						_wall_right = 0;
						_on_ground = false;
					}
				}
			}
		}

		float move_speed = _on_ground ? 0.025f : 0.005f;

		if (is_key_pressed(KEY_LEFT)) {
			_draw_flags = DT_FLIP_X;
			is_moving = true;
			_vel.x -= move_speed;

			if (_last_clipped & CLIPPED_XN) {
				if (_vel.y > 0.0f)
					_wall_dragging = 3;
			}
		}

		if (is_key_pressed(KEY_RIGHT)) {
			_draw_flags = 0;
			is_moving = true;
			_vel.x += move_speed;

			if (_last_clipped & CLIPPED_XP) {
				if (_vel.y > 0.0f)
					_wall_dragging = 3;
			}
		}
	}

	if (_wall_dragging > 0) {
		if (_vel.y > 0.0f)
			_vel.y *= 0.85f;

		_wall_dragging--;
	}

	if (is_key_pressed(KEY_UP)) {
		if (_jump_launch > 0) {
			_vel.y -= 0.075f / JUMP_LAUNCH_TIME;
			_jump_launch--;
		}
	}
	else {
		_jump_launch = 0;
		_jump_latch = false;
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
		if (_vel.y > 0.0f)
			_sprite = 11 + ((_anim / 5) % 3);
		else
			_sprite = 16 + ((_anim / 5) % 3);
	}

	_on_ground = false;
	_last_clipped = 0;

	if (_wall_left > 0) _wall_left--;
	if (_wall_right > 0) _wall_right--;

	int test_left = 0;
	int test_right = 0;

	cast_aabb(g, _bb, vec2(-0.05f, 0.0f), &test_left);
	cast_aabb(g, _bb, vec2(0.05f, 0.0f), &test_right);

	if (test_left & CLIPPED_XN) _wall_left = 4;
	if (test_right & CLIPPED_XP) _wall_right = 4;
}

void player::on_hit_wall(game* g, int clipped) {
	if (clipped & CLIPPED_XP) {
		_vel.x = min(_vel.x, 0.0f);
		_wall_right = 4;
		_wall_dragging = 3;
	}

	if (clipped & CLIPPED_XN) {
		_vel.x = max(_vel.x, 0.0f);
		_wall_left = 4;
		_wall_dragging = 3;
	}

	if (clipped & CLIPPED_YP) {
		_vel.y = min(_vel.y, 0.0f);
		_on_ground = true;
	}

	if (clipped & CLIPPED_YN) {
		_jump_launch = 0;
		_vel.y = max(_vel.y, 0.0f);
	}

	_last_clipped = clipped;
}

void player::post_tick(game* g) {
}