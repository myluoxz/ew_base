#ifndef __H_EW_WND_GLCONTEXT__
#define __H_EW_WND_GLCONTEXT__

#include "ewc_base/config.h"
#include "ewa_base/figure/fig.h"

EW_ENTER

class DLLIMPEXP_EWC_BASE DataNode;
class DLLIMPEXP_EWC_BASE DataModel;

class DLLIMPEXP_EWC_BASE GLContext : public NonCopyable
{
public:

	GLContext();
	~GLContext();

	void SetCurrent(wxWindow* w);
	void SwapBuffers();

	static void CheckError();

protected:

	void* m_hWnd;
	void* m_hDC;
	void* m_hGL;

};


class DLLIMPEXP_EWC_BASE GLBuffer
{
public:

	GLBuffer();

	enum
	{
		FLAG_NEED_COLOR=1<<0,
		FLAG_NEED_DEPTH=1<<1,
		FLAG_NEED_ALPHA=1<<2,
	};

	vec2i vpos;
	vec2i size;

	arr_xt<float> depth;
	arr_xt<uint8_t> alpha;

	arr_xt<tiny_storage<uint8_t,3> > color;

	mat4d projmatrix;
	mat4d viewmatrix;

	BitFlags flags;

	void ReadPixels();

	void Blit(wxDC& dc);

protected:

	AutoPtrT<Object> m_pExtraData;

};

class DVec2i : public vec2i
{
public:
	DVec2i(const wxSize& sz);
	DVec2i(const wxPoint& pt);
	DVec2i(const vec2i& o);
};

class GLDC : public GLContext
{
public:

	GLDC();

	enum
	{
		RENDER_CALC_MINSIZE,
		RENDER_SET_REALSIZE,
		RENDER_SOLID,
		RENDER_ALPHA,
		RENDER_LINE,
		RENDER_TEXT,

		RENDER_SELECT,
	};


	class SizeParam
	{
	public:
		box3d b3bbox;
		box3d b3axis;
		mat4d m4data;
	}sz;


	void Light(DLight& light, bool f);

	void Reshape(const DVec2i& s_, const DVec2i& p_ = vec2i(0, 0));

	void Clear(const DColor& c);
	void Clear();

	void LoadIdentity();

	void (*Color)(const DColor& c);

	void PushMatrix();

	void PopMatrix();

	void Blit(wxDC& dc);

	void Mode(int mode);

	int Mode();

	void RenderNode(DataNode* node);
	void RenderModel(DataModel* model);

	void RenderSelect(DataModel* model);

	DataNode* HitTest(unsigned x, unsigned y);

protected:

	arr_1t<int> m_aLights;
	int m_nMode;
	box3d m_b3BBox;
	DColor m_v4BgColor;
	GLBuffer m_aBuffer[4];

	static void _Color1(const DColor& color);
	static void _Color2(const DColor& color);

	indexer_set<DataNode*> m_aNodes;
};



EW_LEAVE

#endif
