#ifndef RENDER_H

namespace gpu {
	enum tex_format {
		FMT_RGBA
	};

	struct VertexBuffer;
	struct ShaderDecl;
	struct Texture2d;
	struct DepthBuffer;

	VertexBuffer* CreateVertexBuffer(int elementSize, int elementCount);
	void DestroyVertexBuffer(VertexBuffer* vb);
	void* Map(VertexBuffer* vb);
	void Unmap(VertexBuffer* vb);

	ShaderDecl* CreateShaderDecl(const BYTE* vertexShader, int vertexShaderLength, const BYTE* pixelShader, int pixelShaderLength);
	void DestroyShaderDecl(ShaderDecl* decl);

	Texture2d* CreateTexture2d(int width, int height, tex_format fmt, uint8_t* initial_data);
	void DestroyTexture2d(Texture2d* tex);
	void SetSampler(int slot, bool tex_clamp, bool bilin);
	void SetTexture(int slot, Texture2d* tex);

	DepthBuffer* CreateDepth(int width, int height);
	void DestroyDepth(DepthBuffer* dss);

	void Init();
	void Shutdown();

	void Clear(uint32_t col);
	void SetViewport(ivec2 pos, ivec2 size, vec2 depth);
	void SetRenderTarget(Texture2d* tex);
	void SetDepthTarget(DepthBuffer* dss);
	void SetDefaultRenderTarget();
	void SetPsConst(int slot, vec4 v);
	void SetVsConst(int slot, vec4 v);
	void SetVsConst(int slot, mat44 m);
	void SetDepthMode(bool depth_compare);
	void Draw(ShaderDecl* decl, VertexBuffer* vb, int count, bool alpha_blend, bool as_lines);
}

struct Vertex {
	float x, y, z;
	float u, v;
	float r, g, b, a;
};

void set_camera(vec2 centre, float width);
void set_camera_3d(mat44 camera, float width);
vec2 to_game(vec2 screen);
void draw_rect(vec2 p0, vec2 p1, colour c);
void draw_rect(vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3);
void draw_quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3);
void draw_font_rect(vec2 p0, vec2 p1, vec2 uv0, vec2 uv1, colour col);
void draw_tri_3d(vec3 p0, vec3 p1, vec3 p2, colour col);

enum font_flags {
	TEXT_LEFT = 1,
	TEXT_RIGHT = 2,
	TEXT_CENTRE = 3
};

float measure_char(int c);
float measure_string(const char* txt);
void draw_char(vec2 pos, vec2 scale, int sprite, colour col);
void draw_string(vec2 pos, vec2 scale, int flags, colour col, const char* txt, ...);

void RenderInit();
void RenderShutdown();

#endif // RENDER_H