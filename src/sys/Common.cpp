#include "pch.h"
#include "common.h"

#define STBI_NO_STDIO
#include "stb_image.c"

extern HWND gMainWnd;
extern const wchar_t* gAppName;

void DebugLn(const char* txt, ...)
{
	char buf[512];

	va_list ap;

	va_start(ap, txt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, txt, ap);
	va_end(ap);

	OutputDebugStringA(buf);
	OutputDebugStringA("\r\n");
}

void Panic(const char* msg)
{
	char buf[256];
	WideCharToMultiByte(CP_UTF8, 0, gAppName, -1, buf, 256, 0, FALSE);

	MessageBoxA(gMainWnd, msg, buf, MB_ICONERROR | MB_OK);
	ExitProcess(0);
}

int hash(const uint8_t* data, int size) {
	int h = 5381;

	while(size-- > 0)
		h = h * 33 + *data;

	return h;
}

float gaussian(float n, float theta) {
	return ((1.0f / sqrtf(2.0f * 3.14159f * theta)) * expf(-(n * n) / (2.0f * theta * theta)));   
}

bool overlaps_rect(vec2 c0, vec2 s0, vec2 c1, vec2 s1)
{
	vec2 delta = c1 - c0;
	vec2 size = (s0 + s1) * 0.5f;
	
	return (fabsf(delta.x) < size.x) && (fabsf(delta.y) < size.y);
}

bool load_file(file_buf* fb, const char* path) {
	fb->destroy();

	HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	DWORD size = GetFileSize(h, 0);

	if (size < (64 * 1024 * 1024)) {
		if ((fb->data = new uint8_t[size]) != 0) {
			fb->size = (int)size;
			DWORD bytes = 0;
			if (!ReadFile(h, fb->data, size, &bytes, 0) || (bytes != size)) {
				fb->destroy();
			}
		}
	}

	CloseHandle(h);

	return fb->data != 0;
}

gpu::Texture2d* load_texture(const char* path) {
	file_buf fb;

	if (!load_file(&fb, path))
		return 0;

	int width = 0, height = 0;
	uint8_t* data = stbi_load_from_memory(fb.data, fb.size, &width, &height, 0, 4);

	gpu::Texture2d* tex = gpu::CreateTexture2d(width, height, gpu::FMT_RGBA, data);

	free(data);

	return tex;
}

// random - based on tinymt64

#define MAT1 0xFA051F40ULL
#define MAT2 0xFFD0FFF4ULL
#define TMAT 0x58D02FFEFFBFFFBCULL

random::random(uint64_t seed) { reseed(seed); }

void random::reseed(uint64_t seed) {
	_a = seed ^ (MAT1 << 32);
	_b = MAT2 ^ TMAT;

	_b ^= 1 + TMAT * (_a ^ (_a >> 62));
	_a ^= 2 + TMAT * (_b ^ (_b >> 62));
	_b ^= 3 + TMAT * (_a ^ (_a >> 62));
	_a ^= 4 + TMAT * (_b ^ (_b >> 62));
	_b ^= 5 + TMAT * (_a ^ (_a >> 62));
	_a ^= 6 + TMAT * (_b ^ (_b >> 62));
	_b ^= 7 + TMAT * (_a ^ (_a >> 62));
	_a ^= 8 + TMAT * (_b ^ (_b >> 62));
}

uint64_t random::rand64() {
	uint64_t x = (_a & 0x7FFFFFFFFFFFFFFFULL) ^ _b;

	x ^= x << 12;
	x ^= x >> 32;
	x ^= x << 32;
	x ^= x << 11;

	_a = _b ^ (-(int64_t)(x & 1)) & MAT1;
	_b = x ^ (-(int64_t)(x & 1)) & (MAT2 << 32);

	uint64_t y = (_a + _b) ^ (_a >> 8);

	return y ^ (-(int64_t)(y & 1)) & 0x58D02FFEFFBFFFBCULL;
}

double random::frand64() {
	return rand64() * (1.0 / 18446744073709551616.0);
}

int random::rand(int min, int max) {
	if (min >= max)
		return min;

	return min + (rand64() % (max - min + 1)); // TODO: biased
}

float random::frand(float max)							{ return (float)(frand64() * max); }
float random::frand(float min, float max)				{ return (float)(min + frand64() * (max - min)); }
float random::sfrand(float range)						{ return (float)(frand64() * range * 2.0f - range); }

vec2 random::v2rand(const vec2& max)					{ return vec2(frand(max.x), frand(max.y)); }
vec2 random::v2rand(const vec2& min, const vec2& max)	{ return vec2(frand(min.x, max.x), frand(min.y, max.y)); }
vec2 random::sv2rand(const vec2& range)					{ return vec2(sfrand(range.x), sfrand(range.y)); }

vec3 random::v3rand(const vec3& max)					{ return vec3(frand(max.x), frand(max.y), frand(max.z)); }
vec3 random::v3rand(const vec3& min, const vec3& max)	{ return vec3(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z)); }
vec3 random::sv3rand(const vec3& range)					{ return vec3(sfrand(range.x), sfrand(range.y), sfrand(range.z)); }

vec4 random::v4rand(const vec4& max)					{ return vec4(frand(max.x), frand(max.y), frand(max.z), frand(max.w)); }
vec4 random::v4rand(const vec4& min, const vec4& max)	{ return vec4(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z), frand(min.w, max.w)); }
vec4 random::sv4rand(const vec4& range)					{ return vec4(sfrand(range.x), sfrand(range.y), sfrand(range.z), sfrand(range.w)); }
