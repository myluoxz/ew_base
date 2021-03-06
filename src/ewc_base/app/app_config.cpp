#include "ewc_base/app/app_config.h"
#include "ewc_base/app/app.h"


EW_ENTER

bool AppConfig::Load(const String& s)
{
	SerializerStream ar;

	if (!ar.openuri(s.empty()?s_file:s))
	{
		return false;
	}

	try
	{
		ar.reader().load(values);
		return true;
	}
	catch(...)
	{
		return false;
	}

}

bool AppConfig::Save(const String& s)
{
	SerializerStream ar;

	if (!ar.openuri(s.empty() ? s_file : s, FLAG_FILE_WC))
	{
		return false;
	}

	try
	{
		ar.writer().save(values);
		return true;
	}
	catch(...)
	{
		return false;
	}


}

void AppConfig::CfgUpdate(int lv,const String& s,int32_t& v,int v1,int v2)
{
	int64_t val;
	if(lv>0 && GetValue<int64_t>(s,val) && val>=v1 && val<=v2)
	{
		v=val;
	}
	else
	{
		SetValue<int64_t>(s,v);
	}
}

void AppConfig::CfgUpdate(int lv,const String& s,arr_1t<String>& v)
{
	String z;

	if(lv<0)
	{
		z=string_join(v.begin(),v.end(),",");
	}

	CfgUpdate(lv,s,z);

	if(lv>0)
	{
		v=string_split(z,",");
	}
	
}

void AppConfig::CfgUpdate(int lv,const String& s,int32_t& v)
{
	int64_t val;
	if(lv>0 && GetValue<int64_t>(s,val))
	{
		v=val;
	}
	else
	{
		SetValue<int64_t>(s,v);
	}
}

void AppConfig::CfgUpdate(int lv,const String& s,String& v)
{
	if(lv>0 && GetValue<String>(s,v))
	{

	}
	else
	{
		SetValue<String>(s,v);
	}
}

void AppConfig::CfgUpdate(int lv,const String& s,BitFlags& v,int m)
{
	int64_t val;
	if(lv>0 && GetValue<int64_t>(s,val))
	{
		v.set(m,val!=0);
	}
	else
	{
		SetValue<int64_t>(s,v.get(m)?1:0);
	}
}

EW_LEAVE
