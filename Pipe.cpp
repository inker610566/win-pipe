#include<iostream>
#include<string>
#include<Windows.h>
#include<TCHAR.h>
using namespace std;

void startProcess(TCHAR * arg_list, HANDLE p_stdin, HANDLE p_stdout, HANDLE p_stderr, PROCESS_INFORMATION& pi);
void createSimplePipe(HANDLE& r, HANDLE& w);

int _tmain(int argc, TCHAR *argv[]) 
{
	if(argc != 3)
	{
		cerr<<"\
Require specify exactly 2 parameters\n\
USAGE:\n\
	Pipe.exe [client] [server]\n\
Example:\n\
	Pipe.exe client.exe server.exe\n\
	Pipe.exe \"java -cp C:\\eclipse\\workspace\\client\\bin MyClassName\" server.exe\n\
";
		return 0;
	}
	PROCESS_INFORMATION client, server;
	HANDLE p1_w, p1_r, p2_w, p2_r;
	// client -> p1 -> server -> p2 -> client

	createSimplePipe(p1_r, p1_w);
	createSimplePipe(p2_r, p2_w);

	startProcess(argv[1], p2_r, p1_w, GetStdHandle(STD_ERROR_HANDLE), client);
	startProcess(argv[2], p1_r, p2_w, GetStdHandle(STD_ERROR_HANDLE), server);
	WaitForSingleObject(client.hProcess, INFINITE);
	WaitForSingleObject(server.hProcess, INFINITE);
	return 0;
}

// work like unix popen
void startProcess(TCHAR *arg_list, HANDLE p_stdin, HANDLE p_stdout, HANDLE p_stderr, PROCESS_INFORMATION& pi)
{
	STARTUPINFO si;

	// make sure stdin/stdout/stderr is inheritable
	HANDLE hs[3] = {p_stdin, p_stdout, p_stderr};

	for(int i = 0 ; i < 3 ; i ++)
	{
		if (!SetHandleInformation(hs[i], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
		{
			//cerr<<"Can't change inherit attribute"<<endl;
			//exit(1);
		}
	}
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	si.hStdInput = p_stdin;
	si.hStdOutput = p_stdout;
	si.hStdError = p_stderr;
	si.dwFlags |= STARTF_USESTDHANDLES;
	ZeroMemory(&pi,sizeof(pi));
	
	if(!CreateProcess(
	NULL,
	arg_list,
	NULL,
	NULL,
	TRUE,
	0,
	NULL,
	NULL,
	&si,
	&pi))
	{
		/*exeÀÉ¸ô®|³]¸m¿ù»~*/
		cerr<<"wrong path!!"<<endl;
		exit(1);
		//return ;
	}
		
	for(int i = 0 ; i < 3 ; i ++)
	{
		if (!SetHandleInformation(hs[i], HANDLE_FLAG_INHERIT, 0))
		{
			//cerr<<"Can't change inherit attribute"<<endl;
			//exit(1);
		}
	}
	//WaitForSingleObject(pi.hProcess,INFINITE);
		
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
		
}

void createSimplePipe(HANDLE& r, HANDLE& w)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = FALSE; 
	saAttr.lpSecurityDescriptor = NULL; 
	if(!CreatePipe(&r, &w, &saAttr, 0))
	{
		cerr<<"Can't create pipe"<<endl;
		exit(1);
	}
}
