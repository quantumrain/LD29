#ifndef VEC_H
#define VEC_H

struct vec2 {
	float x, y;

	vec2(float xy = 0) : x(xy), y(xy) { }
	vec2(float x_, float y_) : x(x_), y(y_) { }
};

struct vec3 {
	float x, y, z;

	vec3(float xyz = 0) : x(xyz), y(xyz), z(xyz) { }
	vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) { }
};

struct vec4 {
	float x, y, z, w;

	vec4(float xyzw = 0) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) { }
	vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) { }
	vec4(vec2 xy, float z_, float w_) : x(xy.x), y(xy.y), z(z_), w(w_) { }
	vec4(vec2 xy, vec2 zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) { }
	vec4(vec3 xyz, float w_) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_) { }
};

struct ivec2 {
	int x, y;

	ivec2(int xy = 0) : x(xy), y(xy) { }
	ivec2(int x_, int y_) : x(x_), y(y_) { }
};

struct mat44 {
	vec4 x, y, z, w;

	mat44() : x(1.0f, 0.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f, 0.0f), z(0.0f, 0.0f, 1.0f, 0.0f), w(0.0f, 0.0f, 0.0f, 1.0f) { }
	mat44(const vec4& x_, const vec4& y_, const vec4& z_, const vec4& w_) : x(x_), y(y_), z(z_), w(w_) { }
	mat44(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) : x(m00, m01, m02, m03), y(m10, m11, m12, m13), z(m20, m21, m22, m23), w(m30, m31, m32, m33) { }
};

inline vec2 operator+(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline vec2 operator-(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline vec2 operator*(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline vec2 operator/(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x / rhs.x, lhs.y / rhs.y); }

inline vec2 operator-(const vec2& rhs) { return vec2(-rhs.x, -rhs.y); }

inline vec2& operator+=(vec2& lhs, const vec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline vec2& operator-=(vec2& lhs, const vec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
inline vec2& operator*=(vec2& lhs, const vec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
inline vec2& operator/=(vec2& lhs, const vec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

inline vec3 operator+(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline vec3 operator-(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
inline vec3 operator*(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline vec3 operator/(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }

inline vec3 operator-(const vec3& rhs) { return vec3(-rhs.x, -rhs.y, -rhs.z); }

inline vec3& operator+=(vec3& lhs, const vec3& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs; }
inline vec3& operator-=(vec3& lhs, const vec3& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs; }
inline vec3& operator*=(vec3& lhs, const vec3& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; return lhs; }
inline vec3& operator/=(vec3& lhs, const vec3& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; return lhs; }

inline vec4 operator+(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
inline vec4 operator-(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
inline vec4 operator*(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
inline vec4 operator/(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }

inline vec4 operator-(const vec4& rhs) { return vec4(-rhs.x, -rhs.y, -rhs.z, -rhs.w); }

inline vec4& operator+=(vec4& lhs, const vec4& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w; return lhs; }
inline vec4& operator-=(vec4& lhs, const vec4& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; lhs.w -= rhs.w; return lhs; }
inline vec4& operator*=(vec4& lhs, const vec4& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; lhs.w *= rhs.w; return lhs; }
inline vec4& operator/=(vec4& lhs, const vec4& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; lhs.w /= rhs.w; return lhs; }

inline ivec2 operator+(const ivec2& lhs, const ivec2& rhs) { return ivec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline ivec2 operator-(const ivec2& lhs, const ivec2& rhs) { return ivec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline ivec2 operator*(const ivec2& lhs, const ivec2& rhs) { return ivec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline ivec2 operator/(const ivec2& lhs, const ivec2& rhs) { return ivec2(lhs.x / rhs.x, lhs.y / rhs.y); }

inline ivec2 operator-(const ivec2& rhs) { return ivec2(-rhs.x, -rhs.y); }

inline ivec2& operator+=(ivec2& lhs, const ivec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline ivec2& operator-=(ivec2& lhs, const ivec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
inline ivec2& operator*=(ivec2& lhs, const ivec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
inline ivec2& operator/=(ivec2& lhs, const ivec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

inline vec4 operator*(const mat44& a, const vec4& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w); }
inline mat44 operator*(const mat44& a, const mat44& b) { return mat44(a * b.x, a * b.y, a * b.z, a * b.w); }

inline bool operator==(ivec2& lhs, ivec2& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(ivec2& lhs, ivec2& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }

inline vec2 to_vec2(ivec2 v) { return vec2((float)v.x, (float)v.y); }
inline vec2 rotation(float a) { return vec2(cosf(a), sinf(a)); }

inline float sum(const vec2& v) { return v.x + v.y; }
inline float sum(const vec3& v) { return v.x + v.y + v.z; }
inline float sum(const vec4& v) { return v.x + v.y + v.z + v.w; }

inline float dot(const vec2& v0, const vec2& v1) { return sum(v0 * v1); }
inline float dot(const vec3& v0, const vec3& v1) { return sum(v0 * v1); }
inline float dot(const vec4& v0, const vec4& v1) { return sum(v0 * v1); }

inline float length_sq(const vec2& v) { return sum(v * v); }
inline float length_sq(const vec3& v) { return sum(v * v); }
inline float length_sq(const vec4& v) { return sum(v * v); }

inline float length(const vec2& v) { return sqrtf(sum(v * v)); }
inline float length(const vec3& v) { return sqrtf(sum(v * v)); }
inline float length(const vec4& v) { return sqrtf(sum(v * v)); }

inline vec2 normalise(const vec2& v) { return v * vec2(1.0f / length(v)); }
inline vec3 normalise(const vec3& v) { return v * vec3(1.0f / length(v)); }
inline vec4 normalise(const vec4& v) { return v * vec4(1.0f / length(v)); }

inline float cross(const vec2& v0, const vec2& v1) { return v0.x * v1.y - v1.x * v0.y; }
inline vec3 cross(const vec3& v0, const vec3& v1) { return vec3(v0.y * v1.z - v1.y * v0.z, v0.z * v1.x - v1.z * v0.x, v0.x * v1.y - v1.x * v0.y); }

mat44 frustum(float left, float right, float bottom, float top, float z_near, float z_far);
mat44 ortho(float left, float right, float bottom, float top, float z_near, float z_far);
mat44 camera_look_at(const vec3& eye, const vec3& at, const vec3& up);
mat44 inverse(const mat44& m);

mat44 scale(const vec3& s);
mat44 rotate_x(float theta);
mat44 rotate_y(float theta);
mat44 rotate_z(float theta);
mat44 translate(const vec3& v);

struct colour
{
	float r, g, b, a;

	colour(float rgba = 1) : r(rgba), g(rgba), b(rgba), a(rgba) { }
	colour(float rgb, float a) : r(rgb), g(rgb), b(rgb), a(a) { }
	colour(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) { }
};

inline colour operator+(const colour& lhs, const colour& rhs) { return colour(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a); }
inline colour operator-(const colour& lhs, const colour& rhs) { return colour(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a); }
inline colour operator*(const colour& lhs, const colour& rhs) { return colour(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a); }
inline colour operator/(const colour& lhs, const colour& rhs) { return colour(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a); }

#endif // VEC_H