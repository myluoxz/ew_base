#ifndef __H_EW_UI_DATA_NODE__
#define __H_EW_UI_DATA_NODE__

#include "ewc_base/data/data_array.h"
#include "ewa_base/domdata/dobject.h"

EW_ENTER

class DLLIMPEXP_EWC_BASE DataModel;

class DLLIMPEXP_EWC_BASE DataChangedParam
{
public:
	DataChangedParam(const DataModel& m) :model(const_cast<DataModel&>(m))
	{

	}

	DataModel& model;
	DChildrenState state;
};

class DLLIMPEXP_EWC_BASE GLDC;
class DLLIMPEXP_EWC_BASE GLTool;
class DLLIMPEXP_EWC_BASE DataNode;
class DLLIMPEXP_EWC_BASE IWnd_modelview;


class DLLIMPEXP_EWC_BASE GLParam
{
public:

	enum
	{
		FLAG_CAPTURE	=1<<0,
		FLAG_RELEASE	=1<<1,
		FLAG_REFRESH	=1<<2,
		FLAG_CACHING	=1<<3,
	};

	enum
	{
		BTN_IS_DOWN		= 1 << 0,
		BTN_IS_MOVED	= 1 << 1,
		IMAGE_CACHED	= 1 << 2,
	};
};

class DLLIMPEXP_EWC_BASE GLToolData : public ObjectData
{
public:

	virtual int OnDraging(GLTool&);
	virtual int OnBtnDown(GLTool&);

	virtual int OnBtnDClick(GLTool&);

	virtual int OnBtnUp(GLTool&);
	virtual int OnBtnCancel(GLTool&);

	virtual int OnWheel(GLTool&);
};



class DLLIMPEXP_EWC_BASE GLTool : public Object
{
public:


	int type;
	int btn_id;

	double wheel;

	vec2i v2size;
	vec2i v2pos0,v2pos1, v2pos2;

	LitePtrT<IWnd_modelview> pview;
	DataPtrT<GLToolData> pdata;

	BitFlags flags;


	void CaptureMouse();
	void ReleaseMouse();

	DataNode* HitTest(int x, int y);

	void ContextMenu(const String&){}

	void OnMouseEvent(wxMouseEvent& evt);

	void Cancel();

	GLTool();

protected:

	bool UpdateToolData();

	void HandleValue(int ret);

};

class DLLIMPEXP_EWC_BASE DataNode : public Object
{
public:

	enum
	{
		FLAG_TOUCHED	=1<<0,
		FLAG_ATTRIBUTED	=1<<1,
		FLAG_IS_GROUP	=1<<2,
		FLAG_MAX		=1<<3,
	};

	DataNode(DataNode* p = NULL, const String& n = "");
	~DataNode();


	virtual bool UpdateLabel(){ return false; }

	virtual void OnChanged(DataChangedParam&);

	virtual void TouchNode(DataChangedParam&, unsigned);

	virtual void DoRender(GLDC& dc);

	virtual void DoUpdateAttribute(GLDC& dc);

	DataModel* GetModel();
	DataNode* GetRoot();

	virtual DObject* GetItem(){ return NULL; }


	virtual DataPtrT<GLToolData> GetToolData();

	DataNode* parent;
	String name;
	String label;
	DataNodeArray subnodes;
	BitFlags flags;
	int depth;

	DECLARE_OBJECT_INFO(DataNode, ObjectInfo);

protected:
	virtual DataModel* DoGetModel(){ return NULL; }
};

class DLLIMPEXP_EWC_BASE DataNodeSymbol : public DataNode
{
public:
	DataNodeSymbol(DataNode* n, DObject* p);

	DataPtrT<DObject> value;

	bool UpdateLabel();

	virtual void OnChanged(DataChangedParam&);

	const String& GetObjectName() const;

	void TouchNode(DataChangedParam&,unsigned);


	virtual DObject* GetItem()
	{
		return value.get();
	}

};


class DLLIMPEXP_EWC_BASE DataNodeVariant : public DataNode
{
public:

	DataNodeVariant(DataNode* p, const std::pair<String, Variant>& v);

	Variant value;

	bool UpdateLabel();
	void OnChanged(DataChangedParam&);

	void TouchNode(DataChangedParam&, unsigned);

};

template<typename T>
class DLLIMPEXP_EWC_BASE DataNodeSymbolT;

class DLLIMPEXP_EWC_BASE DataNodeCreator : public NonCopyableAndNonNewable
{
public:

	static DataNodeCreator& current();

	static DataNodeSymbol* Create(DataNode* n, DObject* p);

	static DataNodeVariant* Create(DataNode* p, const std::pair<String, Variant>& v);

	template<typename T>
	static void Register()
	{
		current().hmap[&T::sm_info] = &_DoCreateDataNode<T>;
	}

	template<typename T1,typename T2>
	static void Register2()
	{
		current().hmap[&T1::sm_info] = &_DoCreateDataNode<T2>;
	}

private:

	template<typename T>
	static DataNodeSymbol* _DoCreateDataNode(DataNode* n, DObject* p)
	{
		return new DataNodeSymbolT<T>(n, p);
	}

	typedef DataNodeSymbol* (*data_node_ctor)(DataNode*, DObject*);
	indexer_map<ObjectInfo*, data_node_ctor> hmap;
};



EW_LEAVE
#endif

