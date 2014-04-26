#ifndef COMMON_H
#define COMMON_H

const float DT = 1.0f / 60.0f;
const float PI = 3.14159f;

template<typename T> T Max(T a, T b) { return (b < a) ? a : b; }
template<typename T> T Min(T a, T b) { return (a < b) ? a : b; }
template<typename T> T Clamp(T v, T low, T high) { return (v < low) ? low : ((v > high) ? high : v); }
template<typename T> T Lerp(T a, T b, float t) { return a + (b - a) * t; }
inline float Square(float f) { return f * f; }
template<typename T> void Swap(T& a, T& b) { T t = b; b = a; a = t; }

#include "vec.h"
#include "render.h"

struct random {
	random(uint64_t seed);
	void reseed(uint64_t seed);

	uint64_t rand64();
	double frand64();

	int rand(int min, int max);

	float frand(float max = 1.0f);
	float frand(float min, float max);
	float sfrand(float range = 1.0f);

	vec2 v2rand(const vec2& max);
	vec2 v2rand(const vec2& min, const vec2& max);
	vec2 sv2rand(const vec2& range);

	vec3 v3rand(const vec3& max);
	vec3 v3rand(const vec3& min, const vec3& max);
	vec3 sv3rand(const vec3& range);

	vec4 v4rand(const vec4& max);
	vec4 v4rand(const vec4& min, const vec4& max);
	vec4 sv4rand(const vec4& range);

	uint64_t _a, _b;
};

int hash(const uint8_t* data, int size);
float gaussian(float n, float theta);
bool overlaps_rect(vec2 c0, vec2 s0, vec2 c1, vec2 s1);

enum key_press {
	KEY_NONE,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_FIRE,
	KEY_ALT_FIRE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_RESET,
	KEY_CHEAT,
	KEY_MODE,
	KEY_MAX
};

extern ivec2 g_WinSize;
extern int gKey;
extern DWORD gKeyDown[KEY_MAX];
extern ivec2 gMousePos;
extern int gMouseButtons;

// Debug

void DebugLn(const char* txt, ...);
void Panic(const char* msg);

// Sound

enum SoundId {
	kSid_Dit,
	kSid_Buzz,
	kSid_Switch,
	kSid_Win,
	kSid_Max
};

void SoundInit();
void SoundShutdown();
void SoundPlay(SoundId sid, float freq, float volume);

// File

struct file_buf {
	uint8_t* data;
	int size;

	file_buf() : data(), size() { }
	~file_buf() { destroy(); }

	void destroy() { delete [] data; data = 0; size = 0; }
};

bool load_file(file_buf* fb, const char* path);
gpu::Texture2d* load_texture(const char* path);

#endif // COMMON_H