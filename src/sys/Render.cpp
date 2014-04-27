#include "Pch.h"
#include "Common.h"
#include "quad_ps.h"
#include "quad_vs.h"
#include "fs_reduce_ps.h"
#include "fs_blur_x_ps.h"
#include "fs_blur_y_ps.h"
#include "fs_combine_ps.h"
#include "tex_ps.h"
#include "solid_vs.h"
#include "solid_ps.h"

random g_render_rand(1);

gpu::VertexBuffer* gRectVb;
gpu::VertexBuffer* gFontVb;
gpu::VertexBuffer* gTriVb;
gpu::VertexBuffer* gFsQuadVb;
gpu::ShaderDecl* gRectDecl;

gpu::ShaderDecl* gReduceDecl;
gpu::ShaderDecl* gBlurXDecl;
gpu::ShaderDecl* gBlurYDecl;
gpu::ShaderDecl* gCombineDecl;
gpu::ShaderDecl* gTexDecl;
gpu::ShaderDecl* gSolidDecl;

const int kMaxRectVerts = 64 * 1024;
Vertex gRectVerts[kMaxRectVerts];
int gRectVertCount;

const int kMaxFontVerts = 2 * 1024;
Vertex gFontVerts[kMaxRectVerts];
int gFontVertCount;

const int kMaxTriVerts = 64 * 1024;
Vertex gTriVerts[kMaxTriVerts];
int gTriVertCount;

vec4 gCam;
mat44 gProj;
mat44 gCam3d;

void set_camera(vec2 centre, float width) {
	float ratio = g_WinSize.y / (float)g_WinSize.x;
	gCam = vec4(-centre, 1.0f / vec2(width, -width * ratio));
}

void set_camera_3d(mat44 camera, float width) {
	float ratio = g_WinSize.y / (float)g_WinSize.x;

	gCam3d = camera;
	gProj = frustum(-width, width, -width * ratio, width * ratio, 1.0f, 64.0f);
}

vec2 to_game(vec2 screen) {
	return screen / vec2(gCam.z, -gCam.w) - vec2(gCam.x, gCam.y);
}

void draw_tri(vec2 p0, vec2 p1, vec2 p2, colour col) {
	draw_tri(p0, p1, p2, col, col, col);
}

void draw_tri(vec2 p0, vec2 p1, vec2 p2, colour c0, colour c1, colour c2) {
	if ((gRectVertCount + 6) > kMaxRectVerts)
		return;

	c0 *= colour(c0.r, c0.g, c0.b, 1.0f);
	c1 *= colour(c1.r, c1.g, c1.b, 1.0f);
	c2 *= colour(c2.r, c2.g, c2.b, 1.0f);

	Vertex* v = &gRectVerts[gRectVertCount];

	gRectVertCount += 3;

	const float uv = 248.0f / 256.0f;

	v->x = p0.x;	v->y = p0.y;	v->u = uv;	v->v = uv;	v->r = c0.r;	v->g = c0.g;	v->b = c0.b;	v->a = c0.a;	v++;
	v->x = p1.x;	v->y = p1.y;	v->u = uv;	v->v = uv;	v->r = c1.r;	v->g = c1.g;	v->b = c1.b;	v->a = c1.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->u = uv;	v->v = uv;	v->r = c2.r;	v->g = c2.g;	v->b = c2.b;	v->a = c2.a;	v++;
}

void draw_rect(vec2 p0, vec2 p1, colour col) {
	draw_rect(p0, p1, col, col, col, col);
}

void draw_rect(vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3) {
	draw_quad(p0, vec2(p1.x, p0.y), vec2(p0.x, p1.y), p1, c0, c1, c2, c3);
}

void draw_quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3) {
	if ((gRectVertCount + 6) > kMaxRectVerts)
		return;

	c0 *= colour(c0.r, c0.g, c0.b, 1.0f);
	c1 *= colour(c1.r, c1.g, c1.b, 1.0f);
	c2 *= colour(c2.r, c2.g, c2.b, 1.0f);
	c3 *= colour(c3.r, c3.g, c3.b, 1.0f);

	Vertex* v = &gRectVerts[gRectVertCount];

	gRectVertCount += 6;

	const float uv = 248.0f / 256.0f;

	// t0
	v->x = p0.x;	v->y = p0.y;	v->u = uv;	v->v = uv;	v->r = c0.r;	v->g = c0.g;	v->b = c0.b;	v->a = c0.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->u = uv;	v->v = uv;	v->r = c2.r;	v->g = c2.g;	v->b = c2.b;	v->a = c2.a;	v++;
	v->x = p1.x;	v->y = p1.y;	v->u = uv;	v->v = uv;	v->r = c1.r;	v->g = c1.g;	v->b = c1.b;	v->a = c1.a;	v++;

	// t1
	v->x = p1.x;	v->y = p1.y;	v->u = uv;	v->v = uv;	v->r = c1.r;	v->g = c1.g;	v->b = c1.b;	v->a = c1.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->u = uv;	v->v = uv;	v->r = c2.r;	v->g = c2.g;	v->b = c2.b;	v->a = c2.a;	v++;
	v->x = p3.x;	v->y = p3.y;	v->u = uv;	v->v = uv;	v->r = c3.r;	v->g = c3.g;	v->b = c3.b;	v->a = c3.a;
}

void draw_font_rect(vec2 p0, vec2 p1, vec2 uv0, vec2 uv1, colour col) {
	if ((gFontVertCount + 6) > kMaxFontVerts)
		return;

	col *= colour(col.r, col.g, col.b, 1.0f);

	Vertex* v = &gFontVerts[gFontVertCount];

	gFontVertCount += 6;

	// t0
	v->x = p0.x;	v->y = p0.y;	v->u = uv0.x;	v->v = uv0.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p0.x;	v->y = p1.y;	v->u = uv0.x;	v->v = uv1.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p1.x;	v->y = p0.y;	v->u = uv1.x;	v->v = uv0.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;

	// t1
	v->x = p1.x;	v->y = p0.y;	v->u = uv1.x;	v->v = uv0.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p0.x;	v->y = p1.y;	v->u = uv0.x;	v->v = uv1.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p1.x;	v->y = p1.y;	v->u = uv1.x;	v->v = uv1.y;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;
}

void draw_tri_3d(vec3 p0, vec3 p1, vec3 p2, colour col) {
	if ((gTriVertCount + 3) > kMaxTriVerts)
		return;

	col *= colour(col.r, col.g, col.b, 1.0f);

	Vertex* v = &gTriVerts[gTriVertCount];

	gTriVertCount += 3;

	// t0
	v->x = p0.x;	v->y = p0.y;	v->z = p0.z;	v->u = 0.0f;	v->v = 0.0f;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p1.x;	v->y = p1.y;	v->z = p1.z;	v->u = 0.0f;	v->v = 0.0f;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->z = p2.z;	v->u = 0.0f;	v->v = 0.0f;	v->r = col.r;	v->g = col.g;	v->b = col.b;	v->a = col.a;
}

void draw_tile(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3, int tile_num, int flags) {
	if ((gRectVertCount + 6) > kMaxRectVerts)
		return;

	c0 *= colour(c0.r, c0.g, c0.b, 1.0f);
	c1 *= colour(c1.r, c1.g, c1.b, 1.0f);
	c2 *= colour(c2.r, c2.g, c2.b, 1.0f);
	c3 *= colour(c3.r, c3.g, c3.b, 1.0f);

	Vertex* v = &gRectVerts[gRectVertCount];

	gRectVertCount += 6;

	int tx = tile_num & 15;
	int ty = tile_num >> 4;

	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
	vec2 uv3;

	const float a = 0.00001f / 16.0f; // UUURGH
	const float b = 15.99999f / 16.0f;

	if ((flags & 1) == 0) {
		uv0 = vec2((tx + a) / 16.0f, (ty + a) / 16.0f);
		uv1 = vec2((tx + b) / 16.0f, (ty + a) / 16.0f);
		uv2 = vec2((tx + a) / 16.0f, (ty + b) / 16.0f);
		uv3 = vec2((tx + b) / 16.0f, (ty + b) / 16.0f);
	} else {
		uv0 = vec2((tx + a) / 16.0f, (ty + b) / 16.0f);
		uv1 = vec2((tx + a) / 16.0f, (ty + a) / 16.0f);
		uv2 = vec2((tx + b) / 16.0f, (ty + b) / 16.0f);
		uv3 = vec2((tx + b) / 16.0f, (ty + a) / 16.0f);
	}

	if (flags & 2) {
		swap(uv0, uv2);
		swap(uv1, uv3);
		swap(uv0, uv1);
		swap(uv2, uv3);
	}

	if (flags & DT_FLIP_X) {
		swap(uv0, uv1);
		swap(uv2, uv3);
	}

	if (flags & DT_FLIP_Y) {
		swap(uv0, uv2);
		swap(uv1, uv3);
	}

	// t0
	v->x = p0.x;	v->y = p0.y;	v->u = uv0.x;	v->v = uv0.y;	v->r = c0.r;	v->g = c0.g;	v->b = c0.b;	v->a = c0.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->u = uv2.x;	v->v = uv2.y;	v->r = c2.r;	v->g = c2.g;	v->b = c2.b;	v->a = c2.a;	v++;
	v->x = p1.x;	v->y = p1.y;	v->u = uv1.x;	v->v = uv1.y;	v->r = c1.r;	v->g = c1.g;	v->b = c1.b;	v->a = c1.a;	v++;

	// t1
	v->x = p1.x;	v->y = p1.y;	v->u = uv1.x;	v->v = uv1.y;	v->r = c1.r;	v->g = c1.g;	v->b = c1.b;	v->a = c1.a;	v++;
	v->x = p2.x;	v->y = p2.y;	v->u = uv2.x;	v->v = uv2.y;	v->r = c2.r;	v->g = c2.g;	v->b = c2.b;	v->a = c2.a;	v++;
	v->x = p3.x;	v->y = p3.y;	v->u = uv3.x;	v->v = uv3.y;	v->r = c3.r;	v->g = c3.g;	v->b = c3.b;	v->a = c3.a;
}

void draw_tile(vec2 c, float s, float rot, colour col, int tile_num, int flags) {
	vec2 cx(cosf(rot) * s * 0.5f, sinf(rot) * s * 0.5f);
	vec2 cy(perp(cx));
	draw_tile(c - cx - cy, c + cx - cy, c - cx + cy, c + cx + cy, col, col, col, col, tile_num, flags);
}

void draw_tile(vec2 c, float s, colour col, int tile_num, int flags) {
	vec2 cx(s, 0.0f);
	vec2 cy(0.0f, s);
	draw_tile(c - cx - cy, c + cx - cy, c - cx + cy, c + cx + cy, col, col, col, col, tile_num, flags);
}

void draw_tile(vec2 p0, vec2 p1, colour col, int tile_num, int flags) {
	draw_tile(p0, vec2(p1.x, p0.y), vec2(p0.x, p1.y), p1, col, col, col, col, tile_num, flags);
}

struct bloom_level {
	ivec2 size;
	gpu::Texture2d* reduce;
	gpu::Texture2d* blur_x;
	gpu::Texture2d* blur_y;

	bloom_level() : reduce(), blur_x(), blur_y() { }
	~bloom_level() { destroy(); }

	void destroy() {
		gpu::DestroyTexture2d(reduce);
		gpu::DestroyTexture2d(blur_x);
		gpu::DestroyTexture2d(blur_y);

		reduce = 0;
		blur_x = 0;
		blur_y = 0;
	}
};

const int MAX_BLOOM_LEVELS = 5;
bloom_level g_bloom_levels[MAX_BLOOM_LEVELS];
gpu::Texture2d* g_draw_target;
gpu::DepthBuffer* g_depth_target;
gpu::Texture2d* g_font;
gpu::Texture2d* g_sheet;

void RenderInit()
{
	gRectDecl		= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_quad_ps, sizeof(g_quad_ps));
	gReduceDecl		= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_fs_reduce_ps, sizeof(g_fs_reduce_ps));
	gBlurXDecl		= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_fs_blur_x_ps, sizeof(g_fs_blur_x_ps));
	gBlurYDecl		= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_fs_blur_y_ps, sizeof(g_fs_blur_y_ps));
	gCombineDecl	= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_fs_combine_ps, sizeof(g_fs_combine_ps));
	gTexDecl		= gpu::CreateShaderDecl(g_quad_vs, sizeof(g_quad_vs), g_tex_ps, sizeof(g_tex_ps));
	gSolidDecl		= gpu::CreateShaderDecl(g_solid_vs, sizeof(g_solid_vs), g_solid_ps, sizeof(g_solid_ps));

	gRectVb		= gpu::CreateVertexBuffer(sizeof(Vertex), kMaxRectVerts);
	gFontVb		= gpu::CreateVertexBuffer(sizeof(Vertex), kMaxFontVerts);
	gTriVb		= gpu::CreateVertexBuffer(sizeof(Vertex), kMaxTriVerts);
	gFsQuadVb	= gpu::CreateVertexBuffer(sizeof(Vertex), 3);

	ivec2 size(g_WinSize);

	g_draw_target = gpu::CreateTexture2d(size.x, size.y, gpu::FMT_RGBA, 0);
	g_depth_target = gpu::CreateDepth(size.x, size.y);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		bloom_level* bl = g_bloom_levels + i;

		size /= 2;

		bl->size = size;
		bl->reduce = gpu::CreateTexture2d(size.x, size.y, gpu::FMT_RGBA, 0);
		bl->blur_x = gpu::CreateTexture2d(size.x, size.y, gpu::FMT_RGBA, 0);
		bl->blur_y = gpu::CreateTexture2d(size.x, size.y, gpu::FMT_RGBA, 0);
	}

	g_font = load_texture("data\\font.png");
	g_sheet = load_texture("data\\sheet.png");
}

void RenderShutdown()
{
	gpu::DestroyShaderDecl(gRectDecl);
	gpu::DestroyShaderDecl(gReduceDecl);
	gpu::DestroyShaderDecl(gBlurXDecl);
	gpu::DestroyShaderDecl(gBlurYDecl);
	gpu::DestroyShaderDecl(gCombineDecl);
	gpu::DestroyShaderDecl(gTexDecl);
	gpu::DestroyShaderDecl(gSolidDecl);

	gpu::DestroyVertexBuffer(gRectVb);
	gpu::DestroyVertexBuffer(gFontVb);
	gpu::DestroyVertexBuffer(gTriVb);
	gpu::DestroyVertexBuffer(gFsQuadVb);

	gpu::DestroyTexture2d(g_draw_target);
	gpu::DestroyDepth(g_depth_target);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++)
		g_bloom_levels[i].destroy();

	gpu::DestroyTexture2d(g_font);
	gpu::DestroyTexture2d(g_sheet);
}

void RenderPreUpdate()
{
	gRectVertCount = 0;
	gFontVertCount = 0;
	gTriVertCount = 0;
}

void do_fullscreen_quad(gpu::ShaderDecl* decl, vec2 size)
{
	if (Vertex* v = (Vertex*)gpu::Map(gFsQuadVb)) {
		float ax = -(1.0f / size.x);
		float ay = (1.0f / size.y);

		v->x = -1.0f + ax;
		v->y = -1.0f + ay;
		v->u = 0.0f;
		v->v = 1.0f;
		v->r = v->g = v->b = v->a = 1.0f;
		v++;

		v->x = -1.0f + ax;
		v->y = 3.0f + ay;
		v->u = 0.0f;
		v->v = -1.0f;
		v->r = v->g = v->b = v->a = 1.0f;
		v++;

		v->x = 3.0f + ax;
		v->y = -1.0f + ay;
		v->u = 2.0f;
		v->v = 1.0f;
		v->r = v->g = v->b = v->a = 1.0f;

		gpu::Unmap(gFsQuadVb);
	}

	gpu::SetVsConst(0, vec4(0, 0, 1.0f, 1.0f));
	gpu::Draw(decl, gFsQuadVb, 3, false, false);
}

void RenderGame()
{
	if (void* data = gpu::Map(gRectVb))
	{
		memcpy(data, gRectVerts, gRectVertCount * sizeof(Vertex));
		gpu::Unmap(gRectVb);
	}

	if (void* data = gpu::Map(gFontVb))
	{
		memcpy(data, gFontVerts, gFontVertCount * sizeof(Vertex));
		gpu::Unmap(gFontVb);
	}

	if (void* data = gpu::Map(gTriVb))
	{
		memcpy(data, gTriVerts, gTriVertCount * sizeof(Vertex));
		gpu::Unmap(gTriVb);
	}

	// draw
	gpu::SetRenderTarget(g_draw_target);
	gpu::SetDepthTarget(g_depth_target);

	gpu::SetViewport(ivec2(0, 0), g_WinSize, vec2(0.0f, 1.0f));
	gpu::Clear(0x000A1419);

	gpu::SetDepthMode(true);
	gpu::SetVsConst(0, gProj * gCam3d);
	gpu::Draw(gSolidDecl, gTriVb, gTriVertCount, true, false);
	gpu::SetDepthMode(false);

	gpu::SetVsConst(0, gCam);
	gpu::SetTexture(0, g_sheet);
	gpu::SetSampler(0, true, false);
	gpu::Draw(gTexDecl, gRectVb, gRectVertCount, true, false);

	gpu::SetTexture(0, g_font);
	gpu::SetSampler(0, true, false);
	gpu::Draw(gTexDecl, gFontVb, gFontVertCount, true, false);

	gpu::SetDepthTarget(0);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		bloom_level* bl = g_bloom_levels + i;

		// reduce
		gpu::SetRenderTarget(bl->reduce);
		gpu::SetViewport(ivec2(), bl->size, vec2(0.0f, 1.0f));
		gpu::SetTexture(0, (i == 0) ? g_draw_target : g_bloom_levels[i - 1].blur_y);
		gpu::SetSampler(0, true, true);
		gpu::SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gReduceDecl, to_vec2(bl->size));

		// blur x
		gpu::SetRenderTarget(bl->blur_x);
		gpu::SetViewport(ivec2(), bl->size, vec2(0.0f, 1.0f));
		gpu::SetTexture(0, bl->reduce);
		gpu::SetSampler(0, true, true);
		gpu::SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gBlurXDecl, to_vec2(bl->size));

		// blur y
		gpu::SetRenderTarget(bl->blur_y);
		gpu::SetViewport(ivec2(), bl->size, vec2(0.0f, 1.0f));
		gpu::SetTexture(0, bl->blur_x);
		gpu::SetSampler(0, true, true);
		gpu::SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gBlurYDecl, to_vec2(bl->size));
	}

	// combine
	gpu::SetDefaultRenderTarget();
	gpu::SetViewport(ivec2(0, 0), g_WinSize, vec2(0.0f, 1.0f));
	gpu::SetPsConst(0, g_render_rand.v4rand(vec4(500.0f), vec4(1500.0f)));
	gpu::SetTexture(0, g_draw_target);
	gpu::SetSampler(0, true, false);
	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		gpu::SetTexture(1 + i, g_bloom_levels[i].blur_y);
		gpu::SetSampler(1 + i, true, true);
	}
	do_fullscreen_quad(gCombineDecl, to_vec2(g_WinSize));
}