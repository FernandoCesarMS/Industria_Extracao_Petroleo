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

HANDLE hEventExibicaoAlarmes;
HANDLE hEventEsc;

int onOffExibicao = ATIVADO;

DWORD WINAPI WaitExibicaoAlarmesEvent(LPVOID);
void split(std::string str, char separator);

std::string strings[8];
bool isFirstTime = TRUE;

int main()
{
    SetConsoleTitle(L"Industria de Extracao de Petroleo - Exibicao de Dados de Otimizacao - Ana Goncalves e Fernando Silva");
    HANDLE hThreads[1];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;
    char caractereDigitado;
    
    hEventExibicaoAlarmes = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de otimização");
    hEventEsc = CreateEvent(NULL, FALSE, FALSE, L"EscEvento");

    hThreads[0] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoAlarmesEvent,
        (LPVOID)0,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[0]) printf("Thread 1 criada. ID = %d\n", dwThreadID);

    std::cout << "Exibicao de dados de processo liberada" << std::endl;

    do {
        caractereDigitado = _getch(); //Lê um caractere
        if (caractereDigitado == 't') {
            PulseEvent(hEventExibicaoAlarmes);
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
    CloseHandle(hEventExibicaoAlarmes);
    Sleep(1000);
    return 0;
}

DWORD WINAPI WaitExibicaoAlarmesEvent(LPVOID id) {
    HANDLE Events[2] = { hEventExibicaoAlarmes, hEventEsc };
    std::string inicio, inicioAnterior;
    int codeOfPreviousOtimizationPrintedInConsole = 0;
    DWORD ret;
    DWORD dwordStatus;
    DWORD dwordBytesLidos;
    int nTipoEvento;
    HANDLE handleDiscFile;
    char buffer[1023] = "";
    std::string previousBuffer = "";
    FILE* input = fopen(".\\ProcessExibicaoOtimizacao\\ExibicaoOtimizacao.txt", "w");
    handleDiscFile = CreateFile(L".\\ProcessExibicaoOtimizacao\\ExibicaoOtimizacao.txt",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    int quant = 0;
    do {
        ret = WaitForMultipleObjects(2, Events, FALSE, 1000);
        dwordStatus = ReadFile(handleDiscFile, &buffer, sizeof(buffer), &dwordBytesLidos, NULL);
        if (buffer != previousBuffer) {
            split(buffer, '|');
            if (codeOfPreviousOtimizationPrintedInConsole < stoi(strings[0])) {
                std::string saida = "NSEQ:" + strings[0] + " SP (TEMP):" + strings[3] + "C SP (PRE):" + strings[2] + "psi VOL:" + strings[4] + "m3 " + strings[5];
                std::cout << saida << std::endl;
                codeOfPreviousOtimizationPrintedInConsole = stoi(strings[0]);
                quant++;
            }
        }
        else {
            quant++;
        }
        
        previousBuffer = buffer;
        if (quant >= 25) {
            handleDiscFile = CreateFile(L".\\ProcessExibicaoOtimizacao\\ExibicaoOtimizacao.txt",
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            quant = 0;
        }
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