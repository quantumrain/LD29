#ifndef GAME_H
#define GAME_H

extern random g_game_rand;

struct game;

enum entity_type {
	ET_PLAYER,
	ET_BUG,
	ET_GEM,
	ET_TURRET,
	ET_BULLET
};

struct entity {
	entity(entity_type type);
	virtual ~entity();

	void destroy();

	virtual void spawned(game* g);
	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void on_attacked(game* g);
	virtual void post_tick(game* g);
	virtual void render(game* g);

	vec2 centre() const { return (_bb.min + _bb.max) * 0.5f; }

	enum {
		FLAG_DESTROYED = 0x1,
		FLAG_CUSTOM_RENDER = 0x2
	};

	uint32_t _flags;
	entity_type _type;
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
	int _money;
	bool _place_latch;
	int _health;
	int _flash_money;
	int _flash_health;
};

struct bug : entity {
	bug();
	virtual ~bug();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void on_attacked(game* g);
	virtual void post_tick(game* g);

	int _damage;
	bool _on_ground;
	int _flash_t;
};

struct gem : entity {
	gem();
	virtual ~gem();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void post_tick(game* g);
};

struct turret : entity {
	turret();
	virtual ~turret();

	virtual void spawned(game* g);
	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void on_attacked(game* g);
	virtual void post_tick(game* g);
	virtual void render(game* g);

	float _rot;
	float _rot_v;
	int _rot_t;
	int _flash_t;
	int _reload;
	float _recoil;
};

struct bullet : entity {
	bullet();
	virtual ~bullet();

	virtual void tick(game* g);
	virtual void on_hit_wall(game* g, int clipped);
	virtual void post_tick(game* g);
	virtual void render(game* g);

	int _time;
};

enum tile_type {
	TT_EMPTY,
	TT_VOID,
	TT_SOLID,
	TT_WALL,
	TT_TURRET
};

struct tile {
	u8 type;
	u16 damage;
	u8 ore;
	u32 search;

	entity* owner;

	tile() : type(TT_EMPTY), damage(), ore(), search(), owner() { }

	int max_damage() { return ore ? 200 : 100; }
	bool is_solid() { return (type == TT_SOLID) || (type == TT_WALL) || (type == TT_TURRET); }
};

const int MAP_WIDTH = 31;
const int MAP_HEIGHT = 80;

struct game {
	game() : _player(), _cam_pos(MAP_WIDTH * 0.5f, 8.5f), _target_cam_y(8.5f), _spawn_time(800), _diff(1.0f) { }

	tile _map[MAP_WIDTH * MAP_HEIGHT];
	list<entity> _entities;
	player* _player;
	vec2 _cam_pos;
	float _target_cam_y;

	float _diff;
	int _spawn_time;

	int get_tile(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? _map[MAP_WIDTH * y + x].type : TT_VOID; }
	tile* get(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? &_map[MAP_WIDTH * y + x] : 0; }

	u32 get_score(int x, int y) { return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT) ? _map[MAP_WIDTH * y + x].search : 0xFFFFFFFF; }


	bool is_solid(int x, int y) {
		int t =  get_tile(x, y);
		return (t == TT_SOLID) || (t == TT_WALL) || (t == TT_TURRET);
	}

	bool is_raycast_solid(int x, int y) {
		int t =  get_tile(x, y);
		return (t == TT_SOLID) || (t == TT_WALL);
	}

	bool is_roughable(int x, int y) {
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
bool raycast(game* g, vec2 from, vec2 to);
bool is_obstructed(game* g, const aabb2& bb);

void add_particle(vec2 pos, vec2 vel, colour c, float s0, float s1, float s2, int max_t);
void update_particles(game* g);
void draw_particles(game* g);

void avoid_others(game* g, entity* self);

#endif // GAME_H