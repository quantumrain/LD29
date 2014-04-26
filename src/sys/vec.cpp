#include "pch.h"
#include "common.h"

mat44 frustum(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float a = (right + left) / (right - left);
	float b = (top + bottom) / (top - bottom);
	float c = z_far / (z_far - z_near);
	float d = (z_near * z_far) / (z_near - z_far);

	float xx = (2.0f * z_near) / (right - left);
	float yy = (2.0f * z_near) / (top - bottom);

	return mat44(
			vec4(xx,	0.0f,	a,		0.0f),
			vec4(0.0f,	yy,		b,		0.0f),
			vec4(0.0f,	0.0f,	c,		1.0f),
			vec4(0.0f,	0.0f,	d,		0.0f)
		);
}

mat44 ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float tx = (right + left) / (left - right);
	float ty = (top + bottom) / (bottom - top);
	float tz = z_near / (z_near - z_far);
		
	float xx = 2.0f / (right - left);
	float yy = 2.0f / (top - bottom);
	float zz = 1.0f / (z_far - z_near);

	return mat44(
			vec4(xx,	0.0f,	0.0f,	0.0f),
			vec4(0.0f,	yy,		0.0f,	0.0f),
			vec4(0.0f,	0.0f,	zz,		0.0f),
			vec4(tx,	ty,		tz,		1.0f)
		);
}

mat44 camera_look_at(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 z = normalise(at - eye);
	vec3 x = normalise(cross(up, z));
	vec3 y = cross(z, x);

	return mat44(
			vec4(x.x, y.x, z.x, 0.0f),
			vec4(x.y, y.y, z.y, 0.0f),
			vec4(x.z, y.z, z.z, 0.0f),
			vec4(-dot(x, eye), -dot(y, eye), -dot(z, eye), 1.0f)
		);
}

mat44 inverse(const mat44& m)
{
	float c0 = m.x.x * m.y.y - m.x.y * m.y.x;
	float c1 = m.x.x * m.y.z - m.x.z * m.y.x;
	float c2 = m.x.x * m.y.w - m.x.w * m.y.x;
	float c3 = m.x.y * m.y.z - m.x.z * m.y.y;
	float c4 = m.x.y * m.y.w - m.x.w * m.y.y;
	float c5 = m.x.z * m.y.w - m.x.w * m.y.z;
	float d0 = m.z.x * m.w.y - m.z.y * m.w.x;
	float d1 = m.z.x * m.w.z - m.z.z * m.w.x;
	float d2 = m.z.x * m.w.w - m.z.w * m.w.x;
	float d3 = m.z.y * m.w.z - m.z.z * m.w.y;
	float d4 = m.z.y * m.w.w - m.z.w * m.w.y;
	float d5 = m.z.z * m.w.w - m.z.w * m.w.z;

	float i = 1.0f / (+ c0 * d5 - c1 * d4 + c2 * d3 + c3 * d2 - c4 * d1 + c5 * d0);

	return mat44(
			(+ m.y.y * d5 - m.y.z * d4 + m.y.w * d3) * i, (- m.x.y * d5 + m.x.z * d4 - m.x.w * d3) * i, (+ m.w.y * c5 - m.w.z * c4 + m.w.w * c3) * i, (- m.z.y * c5 + m.z.z * c4 - m.z.w * c3) * i,
			(- m.y.x * d5 + m.y.z * d2 - m.y.w * d1) * i, (+ m.x.x * d5 - m.x.z * d2 + m.x.w * d1) * i, (- m.w.x * c5 + m.w.z * c2 - m.w.w * c1) * i, (+ m.z.x * c5 - m.z.z * c2 + m.z.w * c1) * i,
			(+ m.y.x * d4 - m.y.y * d2 + m.y.w * d0) * i, (- m.x.x * d4 + m.x.y * d2 - m.x.w * d0) * i, (+ m.w.x * c4 - m.w.y * c2 + m.w.w * c0) * i, (- m.z.x * c4 + m.z.y * c2 - m.z.w * c0) * i,
			(- m.y.x * d3 + m.y.y * d1 - m.y.z * d0) * i, (+ m.x.x * d3 - m.x.y * d1 + m.x.z * d0) * i, (- m.w.x * c3 + m.w.y * c1 - m.w.z * c0) * i, (+ m.z.x * c3 - m.z.y * c1 + m.z.z * c0) * i
		);
}

mat44 scale(const vec3& s)
{
	return mat44(
			vec4(s.x,	0.0f,	0.0f,	0.0f),
			vec4(0.0f,	s.y,	0.0f,	0.0f),
			vec4(0.0f,	0.0f,	s.z,	0.0f),
			vec4(0.0f,	0.0f,	0.0f,	1.0f)
		);
}

mat44 rotate_x(float theta)
{
	float c = cosf(theta);
	float s = sin(theta);

	return mat44(
			vec4(1.0f,	0.0f,	0.0f,	0.0f),
			vec4(0.0f,	c,		s,		0.0f),
			vec4(0.0f,	-s,		c,		0.0f),
			vec4(0.0f,	0.0f,	0.0f,	1.0f)
		);
}

mat44 rotate_y(float theta)
{
	float c = cosf(theta);
	float s = sin(theta);

	return mat44(
			vec4(c,		0.0f,	-s,		0.0f),
			vec4(0.0f,	1.0f,	0.0f,	0.0f),
			vec4(s,		0.0f,	c,		0.0f),
			vec4(0.0f,	0.0f,	0.0f,	1.0f)
		);
}

mat44 rotate_z(float theta)
{
	float c = cosf(theta);
	float s = sin(theta);

	return mat44(
			vec4(c,		s,		0.0f,	0.0f),
			vec4(-s,	c,		0.0f,	0.0f),
			vec4(0.0f,	0.0f,	1.0f,	0.0f),
			vec4(0.0f,	0.0f,	0.0f,	1.0f)
		);
}

mat44 translate(const vec3& v)
{
	return mat44(
			vec4(1.0f,	0.0f,	0.0f,	0.0f),
			vec4(0.0f,	1.0f,	0.0f,	0.0f),
			vec4(0.0f,	0.0f,	1.0f,	0.0f),
			vec4(v, 1.0f)
		);
}
