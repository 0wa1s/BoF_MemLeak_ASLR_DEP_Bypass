/*
	Live Server on port 8888
*/
#include "StdAfx.h"
#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#include<windows.h>
#define _WIN32_WINNT_WIN7                   0x0601

#pragma comment(lib,"ws2_32.lib") //Winsock Library

// junk asm instructions for completing missing rop gadgets
int foo() {
	__asm
	{
		PUSH eax
		mov eax,1
		CMP eax,2
		JG FOOBAR_1
		JMP OTHERLB
		OTHERLB:
		ROR EAX,8
		JMP ENDLB

		FOOBAR_1:
		JMP ESP
		RETN
		PUSHAD
		RETN
		POP EAX
		POP ECX
		RETN
		ADD ESP,200
		POP EAX
		POP ECX
		RETN
		POP EAX
		POP ECX
		RETN
		PUSH EAX
		PUSH ECX
		PUSH EDX
		PUSH 0x40
		POP ECX
		MOV EDX, ECX
		RETN
		
		FOOBAR_2:
		PUSH 0xFF20
		POP EAX
		MOV EAX, 0xFF20
		JMP [EAX]
		RETN
		PUSH 0xFF20
		MOV ESI,dword ptr[EAX]
		RETN


		ENDLB:
		NOP
		POP EAX

		
	}
	return 0;
}

int bar(int x) {

	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	unsigned char payload[] = {
		0x90,		// NOP
		0x90,		// NOP
		0x90,		// INT3
		0xc3		// RET
	};
	unsigned int payload_len = 4;

	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	RtlMoveMemory(exec_mem, payload, payload_len);

	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;

	LPVOID ptr = VirtualAlloc(NULL, 3000, MEM_RESERVE, PAGE_READWRITE); //reserving memory
	ptr = VirtualAlloc(ptr, 3000, MEM_COMMIT, PAGE_READWRITE); //commiting memory
	memset(ptr, 'a', 3000);
	VirtualFree(ptr, 0, MEM_RELEASE); //releasing memory
	return 0;

}

int copy(char input_1[], char input_2[]) {
	strcpy(input_1, input_2);
	return 0;
}

int main(int argc, char *argv[])
{
	foo();
	bar(2);

	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
	char *message;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);

	while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");

		
			//Reply to the client
			char message_1[] = "Input#1: ";
			char message_2[] = "Input#2: ";
			char input_1[16] = "";
			char input_2[32] = "";
			char data[200] = "";
			

			// send first line msg to get user input_1
			send(new_socket, message_1, strlen(message_1), 0);
			
			// get user input_1
			recv(new_socket, input_1, 128, 0);
			// debug console print
			printf(input_1);
			printf("\nData received...\nWriting Data to file....");

			// write data to file			
			FILE * fp;
			fp = fopen("file.txt", "wb+");
//orignal file print code
			fprintf(fp, input_1);

/*
// start new code
			va_list aptr;
			va_start(aptr, input_1);
			vsnprintf(data, sizeof(data) , input_1, aptr);
			va_end(aptr);
// end new code
			printf(data);
*/			fclose(fp);

			printf("\nData written to file....");
			printf("\nReading file....");

			fp = fopen("file.txt", "rb");
			
			fgets(data, 200, fp);
			printf("\nData written to file:-\r\n");

//			printf(data);
			fclose(fp);
			printf("\nSending file contents to socket!");

			send(new_socket, data, strlen(data), 0);

			// send second line msg to get user input_2
			send(new_socket, message_2, strlen(message_2), 0);

			// get user input_2
			recv(new_socket, input_2, 1024, 0);
			printf("\nSize of Input_2: %d", strlen(input_2));
			strcpy(input_1, input_2);
			

			int crash = 0;
			int e=10;
			try 
			{
				crash = copy(input_1, input_2);
				if(crash == e)
				{
					throw e;
				}
				
			}

			catch(long h)
			{
				printf("Exception#3! copy error");
				for(int k=0; k<5; k++)
					int i=k^2;
			}
		
			catch(char h)
			{
				printf("Exception#3! copy error");
				for(int k=0; k<5; k++)
					int i=k^2;
			}
			
			
		

			printf("\nCopy Completed!\n");
			//getchar();
		
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return 1;
	}

	closesocket(s);
	WSACleanup();

	return 0;
}	
