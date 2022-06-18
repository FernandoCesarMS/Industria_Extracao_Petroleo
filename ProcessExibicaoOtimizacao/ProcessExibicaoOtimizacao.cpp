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

HANDLE hEventExibicaoOtimizacao;
HANDLE hEventEsc;

int onOffExibicao = ATIVADO;

DWORD WINAPI WaitExibicaoOtimizacaoEvent(LPVOID);

int main()
{
    SetConsoleTitle(L"Industria de Extracao de Petroleo - Exibicao de Dados de Otimizacao - Ana Goncalves e Fernando Silva");
    HANDLE hThreads[1];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;
    char caractereDigitado;


    hEventExibicaoOtimizacao = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de otimização");
    hEventEsc = CreateEvent(NULL, FALSE, FALSE, L"EscEvento");

    hThreads[0] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoOtimizacaoEvent,
        (LPVOID)0,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[0]) printf("Thread 1 criada. ID = %d\n", dwThreadID);

    std::cout << "Exibicao de dados de processo liberada" << std::endl;

    do {
        caractereDigitado = _getch(); //Lê um caractere
        if (caractereDigitado == 't') {
            PulseEvent(hEventExibicaoOtimizacao);
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
    CloseHandle(hEventExibicaoOtimizacao);
    Sleep(1000);
    return 0;
}

DWORD WINAPI WaitExibicaoOtimizacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventExibicaoOtimizacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
        if (onOffExibicao == DESATIVADO && ret == 0) {
            std::cout << "Thread " << id << " de exibicao de dados de processo esta desbloqueada!" << std::endl;
            onOffExibicao = ATIVADO;
        }
        else if (ret == 0) {
            std::cout << "Thread " << id << " de exibicao de dados de processo foi bloqueada! Aguardando desbloqueamento" << std::endl;
            onOffExibicao = DESATIVADO;
        }
    } while (nTipoEvento == 0);

    std::cout << "Thread " << id << " terminando" << std::endl;

    _endthreadex(0);

    return (0);
}