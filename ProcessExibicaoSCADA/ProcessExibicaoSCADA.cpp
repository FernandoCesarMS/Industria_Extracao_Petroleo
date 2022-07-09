#define WIN32_LEAN_AND_MEAN 
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>				//_getch
#include <process.h>			//_beginthreadex() e _endthreadex()
#include <locale>
#include <string.h>  
#include <ctime>
#include <chrono>
#include <iomanip> 
#include <format>
#include <memory>
#include <string>
#include <stdexcept>

#pragma warning(disable:4996)

#define	ESC				0x1B			//Tecla para encerrar o programa
#define ATIVADO 1
#define DESATIVADO 0

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

HANDLE hEventExibicaoSCADA;
HANDLE hEventEsc;

int onOffExibicao = ATIVADO;
std::string strings[8];

void split(std::string str, char separator);
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);
DWORD WINAPI WaitExibicaoSCADAEvent(LPVOID);

int main()
{
    SetConsoleTitle(L"Industria de Extracao de Petroleo - Exibicao de Dados de Processo - Ana Goncalves e Fernando Silva");
    HANDLE hThreads[1];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;
    char caractereDigitado;

    hEventExibicaoSCADA = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de Processo");
    hEventEsc = CreateEvent(NULL, FALSE, FALSE, L"EscEvento");

    hThreads[0] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoSCADAEvent,
        (LPVOID)0,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[0]) printf("Thread 1 criada. ID = %d\n", dwThreadID);

    std::cout << "Exibicao de dados de processo liberada" << std::endl;

    do {
        caractereDigitado = _getch(); //Lê um caractere
        if (caractereDigitado == 'r') {
            PulseEvent(hEventExibicaoSCADA);
        }
        else if (caractereDigitado == ESC) {
            PulseEvent(hEventEsc);
        }
    } while (caractereDigitado != ESC);

    dwRet = WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 9; i++) {
        GetExitCodeThread(hThreads[i], &dwExitCode);
        CloseHandle(hThreads[i]);
    }

    CloseHandle(hEventEsc);
    CloseHandle(hEventExibicaoSCADA);
    Sleep(1000);
    return 0;
}

DWORD WINAPI WaitExibicaoSCADAEvent(LPVOID id) {
    HANDLE hSlots;

    BOOL bReadFile;
    DWORD dwNoBytesRead;
    char szReadFileBuffer[1023];
    DWORD dwReadFileBufferSize = sizeof(szReadFileBuffer);

    hSlots = CreateMailslot(
        L"\\\\.\\mailslot\\TAREFASCADA",
        0,
        MAILSLOT_WAIT_FOREVER,
        NULL
    );
    HANDLE Events[2] = { hEventExibicaoSCADA, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        bReadFile = ReadFile(
            hSlots,
            szReadFileBuffer,
            dwReadFileBufferSize,
            &dwNoBytesRead,
            NULL
        );
        split(szReadFileBuffer, '|');
        std::string saida = strings[6] + " NSEQ:" + strings[0] + "  PR(T):" + strings[2] + "psi TEMP:" + strings[3] + "C PR(G):" + strings[4] + "psi NIVEL:" + strings[5] + "cm";
        ret = WaitForMultipleObjects(2, Events, FALSE, 500);
        std::cout << saida << std::endl;
    } while (ret != 0);

    std::cout << "Thread " << id << " terminando" << std::endl;

    _endthreadex(0);

    return (0);
}

int len(std::string str) {
    int length = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        length++;
    }

    return length;
}

void split(std::string str, char separator) {
    int currIndex = 0, i = 0, startIndex = 0, endIndex = 0;

    while (i <= len(str)) {
        if (str[i] == separator || i == len(str)) {
            endIndex = i;
            std::string subStr = "";
            subStr.append(str, startIndex, endIndex - startIndex);
            strings[currIndex] = subStr;
            currIndex += 1;
            startIndex = endIndex + 1;
        }
        i++;
    }
}

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}