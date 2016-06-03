#include "ewa_base/basic/process.h"

EW_ENTER



class DLLIMPEXP_EWA_BASE StreamDataPipe : public SerializerReader
{
public:

	virtual int recv(char* buf,int len)
	{
		if(flags.get(FLAG_READER_FAILBIT)) return -1;

		DWORD nRead(0);
		if(::ReadFile(hReader,buf,len,&nRead,NULL)==FALSE)
		{
			flags.add(FLAG_READER_FAILBIT);
			System::CheckError("File::Read Error");
			return -1;
		}
		return nRead;
	}

	virtual int send(const char* buf,int len)
	{
		if(flags.get(FLAG_WRITER_FAILBIT)) return -1;

		DWORD nWrite(0);
		if(::WriteFile(hWriter,buf,len,&nWrite,NULL)==FALSE)
		{
			flags.add(FLAG_WRITER_FAILBIT);
			System::CheckError("File::Write Error");
			return -1;
		}
		return nWrite;
	}

	KO_Handle<KO_Policy_handle> hReader, hWriter;
};

class ProcessImpl
{
public:
	BitFlags flags;

	KO_Handle<KO_Policy_handle> hReader, hWriter;

	Stream hStream;

	PROCESS_INFORMATION pi;

	ProcessImpl()
	{
		ZeroMemory(&pi,sizeof(pi));
	}


	void Kill(int r)
	{
		if(pi.hProcess==NULL) return;
		::TerminateProcess(pi.hProcess,r);
	}


	bool Redirect(KO_Handle<KO_Policy_handle>& h)
	{
		if(pi.hProcess!=NULL)
		{
			System::LogTrace("Process::Redirect failed, process already created!");
			return false;
		}
		hWriter=h;
		hReader.close();
		return true;
	}

	bool Redirect()
	{
		if(pi.hProcess!=NULL) return false;

		hReader.close();
		hWriter.close();
		hStream.close();

		AutoPtrT<StreamDataPipe> hPipe(new StreamDataPipe);

		HANDLE h1,h2;
		if(::CreatePipe(&h1,&h2,NULL,0))
		{
			hPipe->hReader.reset(h1);
			hWriter.reset(h2);	
		}
		else
		{
			return false;
		}

		if(::CreatePipe(&h1,&h2,NULL,0))
		{
			hReader.reset(h1);
			hPipe->hWriter.reset(h2);		
		}
		else
		{
			hPipe->hReader.close();
			hWriter.close();
		}

		hStream.assign(hPipe.release(),NULL);
		return true;
	}

	~ProcessImpl()
	{
		Close();
	}

	bool WaitFor(int ms)
	{
		if(pi.hProcess==NULL) return true;

		DWORD rc=::WaitForSingleObject(pi.hProcess,ms);
		if(rc==WAIT_TIMEOUT)
		{
			return false;
		}
		return true;
	}

	void Wait()
	{
		if(pi.hProcess==NULL) return;
		::WaitForSingleObject(pi.hProcess,INFINITE);
	}

	bool GetExitCode(int* code)
	{
		DWORD code2;
		if(GetExitCodeProcess(pi.hProcess,&code2))
		{
			if(code) *code=code2;
			return true;
		}
		return false;
	}

	void Close()
	{
		hReader.close();
		hWriter.close();
		hStream.close();
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
		pi.hThread=NULL;
		pi.hProcess=NULL;
	}

	bool Execute(const String& s)
	{
		if(pi.hProcess!=NULL) return false;

		STARTUPINFOW si= {sizeof(STARTUPINFO)};

		StringBuffer<wchar_t> sb(s);
		sb.push_back(0);


		HANDLE h1=KO_Policy_handle::duplicate(hReader,TRUE);
		HANDLE h2=KO_Policy_handle::duplicate(hWriter,TRUE);

		si.hStdOutput = h2;
		si.hStdError = h2;
		si.hStdInput = h1;

		si.dwFlags = (h1!=NULL||h2!=NULL)?STARTF_USESTDHANDLES:0;
		
		bool flag=::CreateProcessW(NULL,sb.data(),NULL,NULL,(si.dwFlags&STARTF_USESTDHANDLES)?TRUE:FALSE,0,NULL,NULL,&si,&pi)!=FALSE;

		::CloseHandle(h1);
		::CloseHandle(h2);

		if(!flag)
		{
			System::LogTrace("Process::Exectue:%s FAILED",s);
			return false;
		}

		return true;

	}


};

Process::Process()
{

}


bool Process::Redirect()
{
	if(!impl.ok()) impl.reset(new ProcessImpl);
	return ((ProcessImpl*)impl)->Redirect();
}

bool Process::Redirect(KO_Handle<KO_Policy_handle> h)
{
	if(!impl.ok()) impl.reset(new ProcessImpl);
	return ((ProcessImpl*)impl)->Redirect(h);
}

bool Process::Execute(const String& cmd)
{
	if(!impl.ok()) impl.reset(new ProcessImpl);
	return ((ProcessImpl*)impl)->Execute(cmd);
}

Stream Process::GetStream()
{
	if(!impl.ok()) return Stream();
	return ((ProcessImpl*)impl)->hStream;
}


void Process::Wait()
{
	if(!impl.ok()) return;
	((ProcessImpl*)impl)->Wait();
}

bool Process::GetExitCode(int* code)
{
	if(!impl.ok()) return false;
	return ((ProcessImpl*)impl)->GetExitCode(code);
}

bool Process::WaitFor(int ms)
{
	if(!impl.ok()) return true;
	return ((ProcessImpl*)impl)->WaitFor(ms);
}

void Process::Close()
{
	impl.close();
}

void Process::Kill(int r)
{
	if(!impl.ok()) return;
	((ProcessImpl*)impl)->Kill(r);
}

template<>
void KO_Policy_pointer<ProcessImpl>::destroy(type& o)
{
	delete o;
	o=NULL;
}

template class KO_Policy_pointer<ProcessImpl>;


EW_LEAVE
