
#include "ewa_base/util/json.h"
#include "ewa_base/scripting/callable_array.h"
#include "ewa_base/util/strlib.h"
#include "ewa_base/basic/lookuptable.h"
#include "ewa_base/basic/codecvt.h"
EW_ENTER


class JsonParser
{
public:

	typedef CallableWrapT<VariantTable> table_type;
	typedef CallableWrapT<arr_xt<Variant> > array_type;

	bool unescape_value;

	const char* pchar;


	Variant parse(const String& json)
	{
		pchar = json.c_str();
		try
		{
			Variant value;
			if(parse_value(value))
			{
				return value;
			}
		}
		catch(...)
		{

		}

		return Variant();

	}

	void skip_comment_1()
	{
		if (pchar[0] == '/'&&pchar[1] == '/')
		{
			for (pchar += 2; *pchar; pchar++)
			{
				if (pchar[0] == '\n')
				{
					pchar += 1;
					break;
				}
			}
		}
	}

	void skip_comment_2()
	{
		if (pchar[0] == '/'&&pchar[1] == '*')
		{
			for (pchar += 2; *pchar; pchar++)
			{
				if (pchar[0] == '*' && pchar[1] == '/')
				{
					pchar += 2;
					break;
				}
			}
		}
	}

	void skip_blank()
	{
		for (;;)
		{
			while (*pchar == ' ' || *pchar == '\r' || *pchar == '\n' || *pchar == '\t') pchar++;
			if (pchar[0] == '/')
			{
				if (pchar[1] == '/') { skip_comment_1(); continue; }
				if (pchar[1] == '*') { skip_comment_2(); continue; }
			}
			break;
		}
	}

	bool match(char ch)
	{
		skip_blank();
		if (*pchar != ch)
		{
			return false;
		}
		pchar++;
		return true;
	}

	String tmp_value;
	double tmp_number;

	bool parse_value(Variant& ptr)
	{

		skip_blank();
		switch (*pchar)
		{
		case 'n':
		case 't':
		case 'f':

			read_chunk(tmp_value);
			if (tmp_value == "null")
			{
				ptr.clear();
			}
			else if (tmp_value == "true")
			{
				ptr.reset(true);
			}
			else if (tmp_value == "false")
			{
				ptr.reset(false);
			}
			else
			{
				return false;
			}
			break;
		case '"':
			if (!read_string(tmp_value))
			{
				return false;
			}
			ptr.reset(tmp_value);
			break;
		case '{':
		{
			ptr.reset(new table_type);
			return parse_object(((table_type*)ptr.kptr())->value);
		}
		case '[':
		{
			ptr.reset(new array_type);
			return parse_array(((array_type*)ptr.kptr())->value);
		}
		case 0:
			return false;

		default:
			if ((*pchar<'0' || *pchar>'9') && *pchar != '.')
			{
				return false;
			}

			read_chunk(tmp_value);
			if (!tmp_value.ToNumber(&tmp_number))
			{
				return false;
			}
			ptr.reset(tmp_number);
			break;
		}

		return true;
	}

	bool read_chunk(String& name)
	{
		skip_blank();
		const char* p1 = pchar;

		while (
			(*pchar >= '0'&&*pchar <= '9') ||
			(*pchar >= 'a'&&*pchar <= 'z') ||
			(*pchar >= 'A'&&*pchar <= 'Z') ||
			*pchar == '_' || *pchar == '.' || *pchar == '+' || *pchar == '-'
			) pchar++;

		name.assign(p1, pchar);
		return true;

	}

	bool read_name(String& name)
	{
		skip_blank();
		if (*pchar == '\"')
		{
			return read_string(name);
		}

		return read_chunk(name);
	}

	bool read_string(String& name)
	{
		if (!match('"'))
		{
			return false;
		}

		if (pchar[0] == '"'&&pchar[1] == '"')
		{
			pchar += 2;
			const char* p1 = pchar;
			while (1)
			{
				while (*pchar != '"'&&*pchar != 0) pchar++;

				if (*pchar == 0)
				{
					return false;
				}

				if (pchar[1] == '"'&&pchar[2] == '"')
				{
					name.assign(p1, pchar);
					pchar += 3;
					return true;
				}
				else pchar++;
			}

			return false;
		}

		const char* p1 = pchar;

		int flag = 0;

		while (*pchar != '"'&&*pchar != 0)
		{
			if (*pchar == '\\')
			{
				if (pchar[1] == 'u')
				{
					flag |= 1;
				}
				else
				{
					flag |= 2;
				}
				pchar += 2;

			}
			else
			{
				pchar++;
			}
		}

		if (*pchar != '"')
		{
			return false;
		}

		if (flag == 0)
		{
			name.assign(p1, pchar++);
			if(unescape_value) name=string_unescape(name);
			return true;
		}


		StringBuffer<wchar_t> sb;
		for (const char* p = p1; p < pchar;)
		{
			if (*p == '\\')
			{
				switch (p[1])
				{
				case 'r':
					sb.push_back('\r');
					break;
				case 'n':
					sb.push_back('\n');
					break;
				case 't':
					sb.push_back('\t');
					break;
				case '/':
				case '\\':
				case '\"':
				case '\'':
					sb.push_back(p[1]);
					break;
				case 'u':
				{
					p += 2;
					wchar_t val;

					unsigned char c1 = lookup_table<lkt_number16b>::test(p[0]);
					if (c1 == 0xFF) break;
					val = c1;
					unsigned char c2 = lookup_table<lkt_number16b>::test(p[1]);
					if (c2 == 0xFF) break;
					val = (val << 4) + c2;
					unsigned char c3 = lookup_table<lkt_number16b>::test(p[2]);
					if (c3 == 0xFF) break;
					val = (val << 4) + c3;
					unsigned char c4 = lookup_table<lkt_number16b>::test(p[3]);
					if (c4 == 0xFF) break;
					val = (val << 4) + c4;

					sb.append(val);
					p += 4;
					continue;
				}


				break;
				default:
					return false;

				}
				p += 2;
			}
			else
			{
				sb.append(*p++);
			}
		}

		pchar++;

		StringBuffer<char> char_sb;
		IConv::unicode_to_utf8(char_sb, sb.data(), sb.size());

		name = char_sb;

		return true;
	}

	bool parse_array(array_type::type& obj)
	{
		if (!match('[')) return false;
		if (match(']')) return true;

		arr_1t<Variant> values;
		while (1)
		{
			values.push_back(NULL);

			if (!parse_value(values.back()))
			{
				return false;
			}

			bool flag = match(',');

			if (match(']'))
			{
				obj.assign(values.begin(), values.end());
				return true;
			}

			if (!flag) break;

		}

		return false;
	}


	bool parse_object(table_type::type& obj)
	{
		if (!match('{')) return false;
		if (match('}')) return true;

		while (1)
		{

			String name;
			if (!read_name(name))
			{
				return false;
			}

			if (!match(':'))
			{
				return false;
			}

			if (!parse_value(obj[name]))
			{
				return false;
			}

			bool flag = match(';') || match(',');

			if (match('}'))
			{
				return true;
			}

			if (!flag) break;

		}

		return false;

	}


};

Variant parse_json(const String& json,bool unescape_value)
{
	JsonParser parser;
	parser.unescape_value=unescape_value;

	return parser.parse(json);
}



void variant_to_json(const Variant& json,JsonWriter& writer);

template<unsigned N>
class variant_to_json_dispatch
{
public:

	template<typename T>
	static void g(const T& value,JsonWriter& writer)
	{
		writer.sb<<"\"[unknown object]\"";
	}

	static void g(const dcomplex& value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void g(int64_t value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void g(CallableData* value,JsonWriter& writer)
	{
		if(!value)
		{
			writer.sb<<"null";
		}
		else if(!value->ToJson(writer))
		{
			writer.sb<<"\"[unknown object]\"";
		}
	}

	static void g(double value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void g(bool value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void g(const String& value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void g(const Variant& value,JsonWriter& writer)
	{
		variant_to_json(value,writer);
	}

	static void g(const VariantTable& value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	template<typename T>
	static void g(const arr_xt<T>& value,JsonWriter& writer)
	{
		writer.WriteValue(value);
	}

	static void value(const Variant& v,JsonWriter& writer)
	{
		typedef typename flag_type<N>::type type;
		g(variant_handler<type>::raw(v),writer);
	}
};


JsonWriter::JsonWriter(StringBuffer<char>& s):sb(s)
{

}

void JsonWriter::WriteValue(const Variant& value)
{
	typedef void (*fn)(const Variant&,JsonWriter&);
	typedef lookup_table_4bit<variant_to_json_dispatch,fn> lk;
	lk::test(value.type())(value,*this);
}

void JsonWriter::WriteValue(const String& value)
{
	sb<<"\""<<string_escape(value)<<"\"";
}

void JsonWriter::WriteValue(double value)
{
	sb<<value;
}

void JsonWriter::WriteValue(const dcomplex& value)
{
	sb<<"\"("<<value.real()<<","<<value.imag()<<")\"";
}

void JsonWriter::WriteValue(int64_t value)
{
	sb<<value;
}

void JsonWriter::WriteValue(bool value)
{
	sb<<(value?"true":"false");
}

void JsonWriter::WriteValue(const VariantTable& value)
{
	sb<<"{"<<"\r\n";
	{
		LockState<String> lock(tb,tb+"\t");
		for(size_t i=0;i<value.size();i++)
		{
			WriteName(value.get(i).first);
			WriteValue(value.get(i).second);
			if(i+1<value.size()) sb<<",";
			sb<<"\r\n";
		}
	}
	sb<<tb<<"}";
}

void JsonWriter::WriteName(const String& name)
{
	sb<<tb<<"\""<<name<<"\":";
}

void to_json(const Variant& json,StringBuffer<char>& sb)
{
	JsonWriter writer(sb);
	writer.WriteValue(json);
}

EW_LEAVE
