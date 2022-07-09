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
std::string strings[8];

DWORD WINAPI WaitExibicaoAlarmesEvent(LPVOID);
void split(std::string str, char separator);
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);
std::string getAlarmText();

int main()
{
    SetConsoleTitle(L"Industria de Extracao de Petroleo - Exibicao de Alarmes - Ana Goncalves e Fernando Silva");
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

    std::cout << "Exibicao de dados de alarmes liberada" << std::endl;
    
    do {
        caractereDigitado = _getch(); //Lê um caractere
        if (caractereDigitado == 'l') {
            PulseEvent(hEventExibicaoAlarmes);
        }
        else if (caractereDigitado == ESC) {
            PulseEvent(hEventEsc);
        }
        else if (caractereDigitado == 'z') {
            std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
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
    HANDLE hSlots;

    BOOL bReadFile;
    DWORD dwNoBytesRead;
    char szReadFileBuffer[1023];
    DWORD dwReadFileBufferSize = sizeof(szReadFileBuffer);

    hSlots = CreateMailslot(
        L"\\\\.\\mailslot\\TAREFAALARME",
        0,
        MAILSLOT_WAIT_FOREVER,
        NULL
    );
    HANDLE Events[2] = { hEventExibicaoAlarmes, hEventEsc };
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
        std::string texto = getAlarmText();
        std::string saida = strings[4] + " NSEQ: " + strings[0] + " " + texto + " PRI: " + strings[3];
        ret = WaitForMultipleObjects(2, Events, FALSE, 1000);
        std::cout << saida << std::endl;
    } while (ret != 0);
    CloseHandle(hSlots);

    return (0);
}
std::string getAlarmText() {
    std::string alarms[10] = {
        "VARIACAO NA GRAVIDADE               ",
        "MUDANCA NO CAMPO MAGNETICO          ",
        "PRESENCA DE HIDROCARBONETO DETECTADA",
        "VARIACAO IDENTIFICADA GRAVIMETRO    ",
        "PULSO DE AR DISPARADO PELO HIDROFONE",
        "MAGNETOMETRO DESCALIBRADO           ",
        "DEFEITO NO SISMOLOGO                ",
        "INSTABILIDADE NA TORRE DE PERFURACAO",
        "PETROLEO ENCONTRADO                 ",
        "JAZIDA DE PETROLEO ESVAZIADA        "
    };
    srand((unsigned)time(NULL));
    int randomNumber = rand() % 10;

    return alarms[randomNumber];
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