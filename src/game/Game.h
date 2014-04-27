#ifndef GAME_H
#define GAME_H

extern random g_game_rand;

struct game;

struct entity {
	entity();
	virtual ~entity();

	void destroy();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void post_tick(game* g);

	vec2 centre() const { return (_bb.min + _bb.max) * 0.5f; }

	enum {
		FLAG_DESTROYED = 0x1
	};

	uint32_t _flags;
	aabb2 _bb;
	vec2 _vel;
	vec2 _draw_off;
	uint8_t _sprite;
	uint8_t _draw_flags;
};

struct player : entity {
	player();
	virtual ~player();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void post_tick(game* g);

	bool _on_ground;
	int _last_clipped;
	bool _jump_latch;
	int _jump_ground;
	int _jump_launch;
	int _wall_left;
	int _wall_right;
	int _wall_dragging;
	int _ground_time;
	bool _fire_latch;
	int _fire_time;
	ivec2 _fire_target;
	int _anim;
	bool _up_latch;
	bool _down_latch;
};

struct bug : entity {
	bug();
	virtual ~bug();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void post_tick(game* g);

	bool _on_ground;
};

enum tile_type {
	TT_EMPTY,
	TT_VOID,
	TT_SOLID,
	TT_WALL
};

struct tile {
	u8 type;
	u8 damage;
	u8 ore;
	u32 search;

	tile() : type(TT_EMPTY), damage(), ore(), search() { }

	int max_damage() { return ore ? 2 : 1; }
	bool is_solid() { return (type == TT_SOLID) || (type == TT_WALL); }
};

const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 60;

struct game {
	game() : _player(), _cam_pos(MAP_WIDTH * 0.5f, 8.5f), _target_cam_y(8.5f), _spawn_time(350) { }

	tile _map[MAP_WIDTH * MAP_HEIGHT];
	list<entity> _entities;
	player* _player;
	vec2 _cam_pos;
	float _target_cam_y;

	int _spawn_time;

	int get_tile(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? _map[MAP_WIDTH * y + x].type : TT_VOID; }
	tile* get(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? &_map[MAP_WIDTH * y + x] : 0; }

	u32 get_score(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? _map[MAP_WIDTH * y + x].search : 0xFFFFFFFF; }


	bool is_solid(int x, int y) {
		int t =  get_tile(x, y);
		return (t == TT_SOLID) || (t == TT_WALL);
	}
};

void tick_entities(game* game);
void purge_entities(game* game);
void draw_entities(game* game);

entity* spawn_entity(game* game, entity* ent, vec2 pos);
template<typename T> T* spawn_entity(game* game, T* ent, vec2 pos) { return (T*)spawn_entity(game, (entity*)ent, pos); }

enum {
	CLIPPED_XP = 1,
	CLIPPED_XN = 2,
	CLIPPED_X = CLIPPED_XP | CLIPPED_XN,
	CLIPPED_YP = 4,
	CLIPPED_YN = 8,
	CLIPPED_Y = CLIPPED_YP | CLIPPED_YN
};

aabb2 cast_aabb(game* g, aabb2 self, vec2 delta, int* clipped);

void add_particle(vec2 pos, vec2 vel, colour c, float s0, float s1, float s2, int max_t);
void update_particles(game* g);
void draw_particles(game* g);

void avoid_others(game* g, entity* self);

#endif // GAME_H