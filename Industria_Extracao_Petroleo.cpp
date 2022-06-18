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

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

void* retiradaDadosOtimizacao();
void* retiradaDadosProcesso();
void* retiradaAlarme();
void* exibicaoDadosOtimizacao();
void* exibicaoDadosProcesso();
void* exibicaoAlarme();
void* limpaJanelaConsoleExibicaoAlarmes();
void* encerraTarefas();
void adicionaFinal(std::string);
void adicionaFinalRetirada(std::string);
void printInPrincipalScreen(std::string);
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);

DWORD WINAPI WaitComunicacaoEvent(LPVOID);
DWORD WINAPI WaitRetiradaOtimizacaoEvent(LPVOID);
DWORD WINAPI WaitRetiradaProcessoEvent(LPVOID);
DWORD WINAPI WaitRetiradaAlarmeEvent(LPVOID);
DWORD WINAPI WaitExibicaoOtimizacaoEvent(LPVOID);
DWORD WINAPI WaitExibicaoProcessoEvent(LPVOID);
DWORD WINAPI WaitExibicaoAlarmesEvent(LPVOID);
void comunicacaoOtimizacao(LPVOID);
void comunicacaoSCADA(LPVOID);
void comunicacaoAlarme(LPVOID);

struct Node {
    std::string info;
    struct Node* next;
    Node() {
        info = "";
        next = NULL;
    }
    Node(std::string info, Node* next) {
        this->info = info;
        this->next = next;
    }
};

struct Node* memoriaRAM = NULL;
struct Node* listRetiradaOtimizacao = NULL;
int listSize = 0;
int listSizeRetiradaOtimizacao = 0;

int onOffComunicacao = ATIVADO;
int onOffRetiradaOtimizacao = ATIVADO;
int onOffRetiradaProcesso = ATIVADO;
int onOffRetiradaAlarme = ATIVADO;
int onOffExibicaoOtimizacao = ATIVADO;
int onOffExibicaoProcesso = ATIVADO;
int onOffExibicaoAlarmes = ATIVADO;
int onOffLimpaConsole = ATIVADO;

int nSeqGeral = 1;

//Mutexes
HANDLE hMutexID;
HANDLE hMutexCOUT;

//Eventos
HANDLE hEventComunicacao;
HANDLE hEventRetiradaOtimizacao;
HANDLE hEventRetiradaProcesso;
HANDLE hEventRetiradaAlarmes;
HANDLE hEventExibicaoOtimizacao;
HANDLE hEventExibicaoProcesso;
HANDLE hEventExibicaoAlarmes;
HANDLE hEventLimpaConsole;
HANDLE hEventEsc;
std::string strings[8];

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    SetConsoleTitle(L"Industria de Extracao de Petroleo - Ana Goncalves e Fernando Silva");
    HANDLE hThreads[10];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;
    char caractereDigitado;

    //Criando Mutexes
    hMutexID = CreateMutex(NULL, FALSE, L"Acessa ID");
    hMutexCOUT = CreateMutex(NULL, FALSE, L"Acessa Cout");

    //Criando eventos 
    hEventComunicacao = CreateEvent(NULL, FALSE, FALSE, L"Evento Comunicacao");
    hEventRetiradaOtimizacao = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de otimização");
    hEventRetiradaProcesso = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de processo");
    hEventRetiradaAlarmes = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de alarmes");
    hEventExibicaoOtimizacao = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de otimização");
    hEventExibicaoProcesso = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de processo");
    hEventExibicaoAlarmes = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de alarmes");
    /*hEventLimpaConsole = CreateEvent(NULL, FALSE, FALSE, L"Evento de limpar o console"); */
    hEventEsc = CreateEvent(NULL, FALSE, FALSE, L"EscEvento");

    for (int i = 0; i < 3; i++) {
        hThreads[i] = (HANDLE)_beginthreadex(
            NULL,
            0,
            (CAST_FUNCTION)WaitComunicacaoEvent,
            (LPVOID)i,
            0,
            (CAST_LPDWORD)&dwThreadID
        );
        if (hThreads[i]) printInPrincipalScreen(string_format("Thread %d criada. ID = %d\n", i, dwThreadID));
    }

    hThreads[3] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitRetiradaOtimizacaoEvent,
        (LPVOID)3,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[3]) printInPrincipalScreen(string_format("Thread 3 criada. ID = %d\n", dwThreadID));

    hThreads[4] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitRetiradaProcessoEvent,
        (LPVOID)4,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[4]) printInPrincipalScreen(string_format("Thread 4 criada. ID = %d\n", dwThreadID));

    hThreads[5] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitRetiradaAlarmeEvent,
        (LPVOID)5,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[5]) printInPrincipalScreen(string_format("Thread 5 criada. ID = %d\n", dwThreadID));

    hThreads[6] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoOtimizacaoEvent,
        (LPVOID)6,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[6]) printInPrincipalScreen(string_format("Thread 6 criada. ID = %d\n", dwThreadID));

    hThreads[7] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoProcessoEvent,
        (LPVOID)7,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[7]) printInPrincipalScreen(string_format("Thread 7 criada. ID = %d\n", dwThreadID));

    hThreads[8] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitExibicaoAlarmesEvent,
        (LPVOID)8,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
    if (hThreads[8]) printInPrincipalScreen(string_format("Thread 8 criada. ID = %d\n", dwThreadID));

    //Leitura dos caractéres do teclado
    do {
        caractereDigitado = _getch(); //Lê um caractere
        switch (caractereDigitado) {
            case ('c'):
                PulseEvent(hEventComunicacao);
                PulseEvent(hEventComunicacao);
                PulseEvent(hEventComunicacao);
                if (onOffComunicacao == DESATIVADO) {
                    onOffComunicacao = ATIVADO;
                }
                else {
                    onOffComunicacao = DESATIVADO;
                }
                break;
            case ('o'):
                PulseEvent(hEventRetiradaOtimizacao);
                if (onOffRetiradaOtimizacao == DESATIVADO) {
                    onOffRetiradaOtimizacao = ATIVADO;
                }
                else {
                    onOffRetiradaOtimizacao = DESATIVADO;
                }
                break;
            case ('w'): {
                struct Node* ponteiroListRetiradaOtimizacao = listRetiradaOtimizacao;
                do {
                    std::cout << ponteiroListRetiradaOtimizacao->info << std::endl;
                    if (ponteiroListRetiradaOtimizacao->next != listRetiradaOtimizacao) {
                        ponteiroListRetiradaOtimizacao = ponteiroListRetiradaOtimizacao->next;
                    }
                    else break;
                    Sleep(100);
                } while (ponteiroListRetiradaOtimizacao != listRetiradaOtimizacao);
                std::cout << listSizeRetiradaOtimizacao << std::endl;
                break;
            }
            case ('q'): {
                struct Node* ponteiroListRetiradaOtimizacao = memoriaRAM;
                do {
                    std::cout << ponteiroListRetiradaOtimizacao->info << std::endl;
                    if (ponteiroListRetiradaOtimizacao->next != memoriaRAM) {
                        ponteiroListRetiradaOtimizacao = ponteiroListRetiradaOtimizacao->next;
                    }
                    else break;
                    Sleep(100);
                } while (ponteiroListRetiradaOtimizacao != memoriaRAM);
                std::cout << listSizeRetiradaOtimizacao << std::endl;
                break;
            }
            case ('t'): {
                PulseEvent(hEventExibicaoOtimizacao);
                if (onOffExibicaoOtimizacao == DESATIVADO) {
                    onOffExibicaoOtimizacao = ATIVADO;
                }
                else {
                    onOffExibicaoOtimizacao = DESATIVADO;
                }
                break;
            }
            case ('a'):
                PulseEvent(hEventRetiradaAlarmes);
                if (onOffRetiradaAlarme == DESATIVADO) {
                    onOffRetiradaAlarme = ATIVADO;
                }
                else {
                    onOffRetiradaAlarme = DESATIVADO;
                }
                break;
            case ('p'):
                PulseEvent(hEventRetiradaProcesso);
                if (onOffRetiradaProcesso == DESATIVADO) {
                    onOffRetiradaProcesso = ATIVADO;
                }
                else {
                    onOffRetiradaProcesso = DESATIVADO;
                }
                break;
            case ('r'):
                PulseEvent(hEventExibicaoProcesso);
                if (onOffExibicaoProcesso == DESATIVADO) {
                    onOffExibicaoProcesso = ATIVADO;
                }
                else {
                    onOffExibicaoProcesso = DESATIVADO;
                }
                break;
            case ('l'):
                PulseEvent(hEventExibicaoAlarmes);
                if (onOffExibicaoAlarmes == DESATIVADO) {
                    onOffExibicaoAlarmes = ATIVADO;
                }
                else {
                    onOffExibicaoAlarmes = DESATIVADO;
                }
                break;
            case ('z'):
                limpaJanelaConsoleExibicaoAlarmes();
                break;
            case (ESC):
                PulseEvent(hEventEsc);
                encerraTarefas();
                break;
            default:
                std::cout << "Caractere invalido" << std::endl;
        }
    } while (caractereDigitado != ESC);
       
    dwRet = WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 9; i++) {
        GetExitCodeThread(hThreads[i], &dwExitCode);
        CloseHandle(hThreads[i]);
    }

    CloseHandle(hEventComunicacao);
    CloseHandle(hEventEsc);
    CloseHandle(hEventRetiradaOtimizacao);
    CloseHandle(hEventRetiradaProcesso);
    CloseHandle(hEventRetiradaAlarmes);
    CloseHandle(hEventExibicaoOtimizacao);
    CloseHandle(hEventExibicaoProcesso);
    CloseHandle(hEventExibicaoAlarmes);
    CloseHandle(hMutexID);
    CloseHandle(hMutexCOUT);

    std::cout << "Clica pra terminar ai" << std::endl;
    _getch();

    std::cout << "Programa encerrado" << std::endl;

    return EXIT_SUCCESS;
}

void printInPrincipalScreen(std::string data) {
    WaitForSingleObject(hMutexCOUT,INFINITE);
    std::cout << data << std::endl;
    ReleaseMutex(hMutexCOUT);
}

DWORD WINAPI WaitComunicacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventComunicacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        if (id == (LPVOID)0) {
            Sleep(10);
            comunicacaoOtimizacao(id);
        }
        else 
        if (id == (LPVOID)1) comunicacaoSCADA(id);
        else 
        if (id == (LPVOID)2) comunicacaoAlarme(id);
        printInPrincipalScreen(string_format("Thread %d de comunicacao foi bloqueada! Aguardando desbloqueamento", id));
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitRetiradaOtimizacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventRetiradaOtimizacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        retiradaDadosOtimizacao();
        printInPrincipalScreen(string_format("Thread %d de retirada de dados de otimização foi bloqueada! Aguardando desbloqueamento", id));
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitRetiradaProcessoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventRetiradaProcesso, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        retiradaDadosProcesso();
        printInPrincipalScreen(string_format("Thread %d de retirada de dados de processo foi bloqueada! Aguardando desbloqueamento", id));
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitRetiradaAlarmeEvent(LPVOID id) {
    HANDLE Events[2] = { hEventRetiradaAlarmes, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        retiradaAlarme();
        printInPrincipalScreen(string_format("Thread %d de retirada de dados de alarmes foi bloqueada! Aguardando desbloqueamento", id));
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitExibicaoOtimizacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventExibicaoOtimizacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        exibicaoDadosOtimizacao();
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitExibicaoProcessoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventExibicaoProcesso, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        exibicaoDadosProcesso();
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitExibicaoAlarmesEvent(LPVOID id) {
    HANDLE Events[2] = { hEventExibicaoAlarmes, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        exibicaoAlarme();
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    printInPrincipalScreen(string_format("Thread %d terminando", id));

    _endthreadex(0);

    return (0);
}

int len(std::string str){
    int length = 0;

    for (int i = 0; str[i] != '\0'; i++){
        length++;
    }

    return length;
}

void split(std::string str, char separator){
    int currIndex = 0, i = 0, startIndex = 0, endIndex = 0;
    
    while (i <= len(str)) {
        if (str[i] == separator || i == len(str)){
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
struct tm* getHorarioLocal() {
    time_t timer;
    struct tm* horarioLocal;

    time(&timer); // Obtem informações de data e hora
    horarioLocal = localtime(&timer); // Converte a hora atual para a hora local

    return horarioLocal;
}

void comunicacaoOtimizacao(LPVOID id) {
    printInPrincipalScreen(string_format("Thread %d de comunicacao está enviando mensagens", id));
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
            WaitForSingleObject(hMutexID, INFINITE);
            int nSeq = nSeqGeral;
            int tipo = 11;
            double spPress = (rand() % 100000);
            double spTemp = (rand() % 100000);
            int vol = rand() % 10000;
            struct tm* horarioLocal = getHorarioLocal();

            std::string stringFormatada = string_format("%06d|%d|%06.1f|%06.1f|%05d|%02d:%02d:%02d", nSeq, tipo, spPress / 10, spTemp / 10, vol, horarioLocal->tm_hour, horarioLocal->tm_min, horarioLocal->tm_sec);
         
            adicionaFinal(stringFormatada);
            nSeqGeral++;
            ReleaseMutex(hMutexID);
        }

        ret = WaitForSingleObject(hEventComunicacao, 1000+(rand()%4000));
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento != 0 && listSize < 100);
}

void comunicacaoSCADA(LPVOID id) {
    printInPrincipalScreen(string_format("Thread %d de comunicacao está enviando mensagens", id));
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
            WaitForSingleObject(hMutexID, INFINITE);
            int nSeq = nSeqGeral;
            int tipo = 22;
            double press_T = (rand() % 100000);
            double temp = (rand() % 100000);
            double press_G = (rand() % 100000);
            double nivel = (rand() % 100000);
            struct tm* horarioLocal = getHorarioLocal();

            std::string stringFormatada = string_format("%06d|%d|%06.1f|%06.1f|%06.1f|%06.1f|%02d:%02d:%02d", nSeq, tipo, press_T / 10, temp / 10, press_G / 10, nivel / 10, horarioLocal->tm_hour, horarioLocal->tm_min, horarioLocal->tm_sec);

            adicionaFinal(stringFormatada);
            nSeqGeral++;
            ReleaseMutex(hMutexID);
        }
        
        ret = WaitForSingleObject(hEventComunicacao, 500);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento != 0 && listSize < 100);
}

void comunicacaoAlarme(LPVOID id) {
    printInPrincipalScreen(string_format("Thread %d de comunicacao está enviando mensagens", id));
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
            WaitForSingleObject(hMutexID, INFINITE);
            int nSeq = nSeqGeral;
            int tipo = 55;
            int id = (rand() % 10000);
            int prioridade = (rand() % 1000);
            struct tm* horarioLocal = getHorarioLocal();

            std::string stringFormatada = string_format("%06d|%d|%04d|%03d|%02d:%02d:%02d", nSeq, tipo, id, prioridade, horarioLocal->tm_hour, horarioLocal->tm_min, horarioLocal->tm_sec);

            adicionaFinal(stringFormatada);
            nSeqGeral++;
            ReleaseMutex(hMutexID);
        }
        
        ret = WaitForSingleObject(hEventComunicacao, 1000 + (rand() % 4000));
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento != 0 && listSize < 100);
}

void adicionaFinal(std::string data) {
    if (listSize == 100) {
        onOffComunicacao = DESATIVADO;
    }
    else{
        // Inicializa o node
        struct Node* temp;
        temp = new Node();

        struct Node* aa = memoriaRAM;

        if (memoriaRAM == NULL) {
            temp = new Node(data, NULL);
            memoriaRAM = temp;
            memoriaRAM->next = memoriaRAM;
        }
        else {
            do {
                aa = aa->next;
            } while (aa->next != memoriaRAM);

            temp = new Node(data, aa->next);
            aa->next = temp;
        }
        listSize++;
    }
}

void adicionaFinalRetirada(std::string data) {
    if (listSizeRetiradaOtimizacao == 200) {
        onOffRetiradaOtimizacao = DESATIVADO;
    }
    else {
        // Inicializa o node
        struct Node* temp;
        temp = new Node();

        struct Node* a = listRetiradaOtimizacao;

        if (listRetiradaOtimizacao == NULL) {
            temp = new Node(data, NULL);
            listRetiradaOtimizacao = temp;
            listRetiradaOtimizacao->next = listRetiradaOtimizacao;
        }
        else {
            do {
                a = a->next;
            } while (a->next != listRetiradaOtimizacao);

            temp = new Node(data, a->next);
            a->next = temp;
        }
        listSizeRetiradaOtimizacao++;
    }
}

void removerDado(std::string data) {
    Node* aux, *remover = NULL;

    if (memoriaRAM != NULL) {
        if ((memoriaRAM)->info == data) {
            aux = memoriaRAM;
            memoriaRAM = memoriaRAM->next;
            aux->info = "";
            aux = NULL;
            listSize--;
        }
        else {
            aux = memoriaRAM;
            while (aux->next && aux->next->info != data)
                aux = aux->next;
            if (aux->next) {
                remover = aux->next;
                aux->next = remover->next;
                listSize--;
            }
        }
    }
}

void* retiradaDadosOtimizacao() {
    DWORD ret;
    Sleep(1000);
    struct Node* ponteiroListRetiradaOtimizacao = memoriaRAM;
    if (onOffRetiradaOtimizacao) {
        if (ponteiroListRetiradaOtimizacao != NULL) {
            printInPrincipalScreen("Retirada de dados desbloqueada");
            do {
                if (ponteiroListRetiradaOtimizacao->info.length() == 38) {
                    adicionaFinalRetirada(ponteiroListRetiradaOtimizacao->info);
                    std::cout << "Otimizacao retirada -> " << ponteiroListRetiradaOtimizacao->info << std::endl;
                    removerDado(ponteiroListRetiradaOtimizacao->info);
                }

                if (ponteiroListRetiradaOtimizacao->next != memoriaRAM) {
                    ponteiroListRetiradaOtimizacao = ponteiroListRetiradaOtimizacao->next;
                }
                
                ret = WaitForSingleObject(hEventRetiradaOtimizacao, 300);
            } while (ret == (DWORD)258);
        }
        onOffRetiradaOtimizacao = DESATIVADO;
    }
    
    return (void*)NULL;
}

void* retiradaDadosProcesso() {
    DWORD ret;
    Sleep(1000);
    struct Node* ponteiroListRetiradaProcesso = memoriaRAM;
    if (onOffRetiradaProcesso) {
        if (ponteiroListRetiradaProcesso != NULL) {
            printInPrincipalScreen("Retirada de dados de processo desbloqueada");
            do {
                if (ponteiroListRetiradaProcesso->info.length() == 46) {
                    std::cout << "Processo retirado   -> " << ponteiroListRetiradaProcesso->info << std::endl;
                    removerDado(ponteiroListRetiradaProcesso->info);
                }

                if (ponteiroListRetiradaProcesso->next != memoriaRAM) {
                    ponteiroListRetiradaProcesso = ponteiroListRetiradaProcesso->next;
                }

                ret = WaitForSingleObject(hEventRetiradaProcesso, 500);
            } while (ret == (DWORD)258);
        }
        onOffRetiradaProcesso = DESATIVADO;
    }

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* retiradaAlarme() {
    DWORD ret;
    Sleep(1000);
    struct Node* ponteiroListRetiradaAlarme = memoriaRAM;
    if (onOffRetiradaAlarme) {
        if (ponteiroListRetiradaAlarme != NULL) {
            printInPrincipalScreen("Retirada de dados de alarme desbloqueada");
            do {
                if (ponteiroListRetiradaAlarme->info.length() == 27) {
                    std::cout << "Alarme retirado     -> " << ponteiroListRetiradaAlarme->info << std::endl;
                    removerDado(ponteiroListRetiradaAlarme->info);
                }

                if (ponteiroListRetiradaAlarme->next != memoriaRAM) {
                    ponteiroListRetiradaAlarme = ponteiroListRetiradaAlarme->next;
                }

                ret = WaitForSingleObject(hEventRetiradaAlarmes, 500);
            } while (ret == (DWORD)258);
        }
        onOffRetiradaAlarme = DESATIVADO;
    }

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* exibicaoDadosOtimizacao() {
    if (onOffExibicaoOtimizacao) {
        HANDLE hProcess = 0;
        HANDLE hThread = 0;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        DWORD dwProcessID = 0;
        BOOL bProcess;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        bProcess = CreateProcess(
            L".\\ProcessExibicaoOtimizacao\\x64\\Debug\\ProcessExibicaoOtimizacao.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi);

        if (bProcess == FALSE) {
            cout << "Create Process Failed & Error NO - " << GetLastError() << endl;
        }
    
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return (void*)NULL;
}

void* exibicaoDadosProcesso() {
    if (onOffExibicaoProcesso) {
        HANDLE hProcess = 0;
        HANDLE hThread = 0;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        DWORD dwProcessID = 0;
        BOOL bProcess;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        bProcess = CreateProcess(
            L".\\ProcessExibicaoSCADA\\x64\\Debug\\ProcessExibicaoSCADA.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi);

        if (bProcess == FALSE) {
            cout << "Create Process Failed & Error NO - " << GetLastError() << endl;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return (void*)NULL;
}

void* exibicaoAlarme() {
    if (onOffExibicaoAlarmes) {
        HANDLE hProcess = 0;
        HANDLE hThread = 0;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        DWORD dwProcessID = 0;
        BOOL bProcess;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        bProcess = CreateProcess(
            L".\\ProcessExibicaoAlarmes\\x64\\Debug\\ProcessExibicaoAlarmes.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi);

        if (bProcess == FALSE) {
            cout << "Create Process Failed & Error NO - " << GetLastError() << endl;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return (void*)NULL;
}

void* limpaJanelaConsoleExibicaoAlarmes() {
    std::cout << "Caractere Z digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* encerraTarefas() {
    std::cout << "Caractere ESC digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}