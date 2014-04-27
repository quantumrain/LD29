#include "Pch.h"
#include "Common.h"
#include "Game.h"

const int JUMP_GROUND_GRACE = 8;
const int TIME_TILL_GROUNDED = 4;
const int JUMP_LAUNCH_TIME = 8;
const int FIRE_TIME = 15;

player::player() : _on_ground(false), _last_clipped(), _jump_latch(), _jump_ground(), _jump_launch(), _wall_left(), _wall_right(), _wall_dragging(), _ground_time(), _fire_latch(), _fire_time(), _anim(), _up_latch(), _down_latch() {
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

	if (_fire_time <= 0) {
		if (is_key_pressed(KEY_LEFT)) _draw_flags = DT_FLIP_X;
		if (is_key_pressed(KEY_RIGHT)) _draw_flags = 0;
	}

	ivec2 tile_target = to_ivec2(centre() + vec2((_draw_flags & DT_FLIP_X) ? -0.1f : 0.1f, 0.0f));

	bool is_left_right = is_key_pressed(KEY_LEFT) || is_key_pressed(KEY_RIGHT);

	if (!is_key_pressed(KEY_UP)) _up_latch = false;
	if (!is_key_pressed(KEY_DOWN) || (_ground_time <= TIME_TILL_GROUNDED)) _down_latch = false;

	if (is_key_pressed(KEY_UP) && !is_left_right && !_up_latch)
		tile_target.y--;
	else if (is_key_pressed(KEY_DOWN) && !is_left_right && !_down_latch)
		tile_target.y++;
	else
		tile_target.x += (_draw_flags & DT_FLIP_X) ? -1 : 1;

	if (g->is_solid(tile_target.x, tile_target.y)) {
		if (is_key_pressed(KEY_LEFT) && (_last_clipped & CLIPPED_XN)) _fire_latch = false;
		if (is_key_pressed(KEY_RIGHT) && (_last_clipped & CLIPPED_XP)) _fire_latch = false;
		if (is_key_pressed(KEY_UP) && !_up_latch) _fire_latch = false;
		if (is_key_pressed(KEY_DOWN) && !_down_latch) _fire_latch = false;
	}

	if ((_ground_time > TIME_TILL_GROUNDED) && is_key_pressed(KEY_FIRE)) {
		if (!_fire_latch && _fire_time <= 0) {
			_fire_time = FIRE_TIME;
			_fire_latch = true;
			_fire_target = tile_target;

			if (is_key_pressed(KEY_UP)) _up_latch = true;
			if (is_key_pressed(KEY_DOWN)) _down_latch = true;
		}
	}
	else
		_fire_latch = false;

	if (_ground_time <= TIME_TILL_GROUNDED)
		_fire_time = 0;

	if (_fire_time > 0) {

		if (g->is_solid(_fire_target.x, _fire_target.y)) {
			colour c(0.0f, 1.0f, 1.0f, 1.0f);

			c *= colour(g_game_rand.frand(), 1.0f);
			
			add_particle(to_vec2(_fire_target) + g_game_rand.v2rand(vec2(0.1f), vec2(0.9f)), g_game_rand.sv2rand(vec2(0.0f)), c, 0.0f, 0.15f, 0.15f, 10);
		}

		if (--_fire_time == 0) {
			if (tile* t = g->get(_fire_target.x, _fire_target.y)) {
				if (t->type == TT_SOLID) {
					if (++t->damage >= t->max_damage()) {
						t->type = TT_EMPTY;
						t->damage = 0;

						for(int i = 0; i < 10; i++) {
							colour c(0.0f, 0.0f, 0.0f, 1.0f);
							add_particle(to_vec2(_fire_target) + g_game_rand.v2rand(vec2(0.1f), vec2(0.9f)), vec2(0.0f, 0.02f), c, 0.3f, 0.3f, 0.0f, 10);
						}
					}
					else {
						if (is_key_pressed(KEY_FIRE))
							_fire_time = FIRE_TIME;
					}
				}
			}
		}
	}
	else {
		if (is_key_pressed(KEY_ALT_FIRE)) {
			if (_jump_ground > 0) {
				_vel.y = -0.075f;
				_jump_ground = 0;
				_jump_launch = JUMP_LAUNCH_TIME;
				_jump_latch = true;
				_on_ground = false;
			}
			else if (!_on_ground && !_jump_latch) {
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

		float move_speed = _on_ground ? 0.025f : 0.005f;

		if (is_key_pressed(KEY_LEFT)) {
			is_moving = true;
			_vel.x -= move_speed;

			if (_last_clipped & CLIPPED_XN) {
				if (_vel.y > 0.0f)
					_wall_dragging = 3;
			}
		}

		if (is_key_pressed(KEY_RIGHT)) {
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

	if (is_key_pressed(KEY_ALT_FIRE)) {
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
		if (_fire_time > 0) {
			int yd = _fire_target.y - fast_floor(centre().y);

			if (yd < 0)
				_sprite = 32 + ((_anim / 4) % 3);
			else if (yd > 0)
				_sprite = 23 + ((_anim / 4) % 3);
			else
				_sprite = 8 + ((_anim / 4) % 3);
		}
		else {
			if (is_moving) {
				_sprite = 4 + ((_anim / 6) % 4);
			}
			else {
				if (is_key_pressed(KEY_UP) && !is_left_right && !_up_latch)
					_sprite = 26 + ((_anim / 5) % 4);
				else if (is_key_pressed(KEY_DOWN) && !is_left_right && !_down_latch)
					_sprite = 19 + ((_anim / 5) % 4);
				else
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