#include <iostream>
#define WIN32_LEAN_AND_MEAN 
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
#define LIVRE 0
#define OCUPADO 1
#define FALHA -1
#define SUCESSO 1
#define ATIVADO 1
#define DESATIVADO 0

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

void* comunicacaoDados();
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

struct Node* first = NULL;
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

int main()
{
    HANDLE hThreads[4];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;

    char caractereDigitado;
    
    hEventComunicacao = CreateEvent(NULL, FALSE, FALSE, L"Evento Comunicacao");
    hEventRetiradaOtimizacao = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de otimização");
    hEventRetiradaProcesso = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de processo");
    hEventRetiradaAlarmes = CreateEvent(NULL, FALSE, FALSE, L"Evento retirada de dados de alarmes");
    hEventExibicaoOtimizacao = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de otimização");
    hEventExibicaoProcesso = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de dados de processo");
    hEventExibicaoAlarmes = CreateEvent(NULL, FALSE, FALSE, L"Evento exibição de alarmes");
    hEventLimpaConsole = CreateEvent(NULL, FALSE, FALSE, L"Evento de limpar o console");
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
        if (hThreads[i])
            std::cout << "Thread " << i << " criada id " << dwThreadID << std::endl;
    }

    hThreads[3] = (HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)WaitRetiradaOtimizacaoEvent,
        (LPVOID)3,
        0,
        (CAST_LPDWORD)&dwThreadID
    );
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
            case ('p'):
                retiradaDadosProcesso();
                break;
            case ('a'):
                retiradaAlarme();
                break;
            case ('t'):
                exibicaoDadosOtimizacao();
                break;
            case ('r'):
                exibicaoDadosProcesso();
                break;
            case ('l'):
                exibicaoAlarme();
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
    for (int i = 0; i < 3; i++) {
        GetExitCodeThread(hThreads[i], &dwExitCode);
        CloseHandle(hThreads[i]);
    }

    CloseHandle(hEventComunicacao);
    CloseHandle(hEventEsc);
    // TODO: Tarefas de retirada
    // TODO: Tarefa de comunicação de Dados
    // TODO: Tarefas de exibição

    std::cout << "Clica pra terminar ai" << std::endl;
    _getch();

    std::cout << "Programa encerrado" << std::endl;

    return EXIT_SUCCESS;
}

DWORD WINAPI WaitComunicacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventComunicacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        if (id == (LPVOID)0) comunicacaoOtimizacao(id);
        else 
        if (id == (LPVOID)1) comunicacaoSCADA(id);
        else 
        if (id == (LPVOID)2) comunicacaoAlarme(id);
        std::cout << "Thread " << id << " de comunicacao foi bloqueada! Aguardando desbloqueamento" << std::endl;
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    std::cout << "Thread " << id << " terminando" << std::endl;

    _endthreadex(0);

    return (0);
}

DWORD WINAPI WaitRetiradaOtimizacaoEvent(LPVOID id) {
    HANDLE Events[2] = { hEventRetiradaOtimizacao, hEventEsc };
    DWORD ret;
    int nTipoEvento;

    do {
        retiradaDadosOtimizacao();
        std::cout << "Thread " << id << " de retirada de dados de otimização foi bloqueada! Aguardando desbloqueamento" << std::endl;
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    std::cout << "Thread " << id << " terminando" << std::endl;

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
    std::cout << "Thread " << id << " de comunicacao está enviando mensagens" << std::endl;
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
            int nSeq = nSeqGeral;
            int tipo = 11;
            double spPress = (rand() % 100000);
            double spTemp = (rand() % 100000);
            int vol = rand() % 10000;
            struct tm* horarioLocal = getHorarioLocal();

            std::string stringFormatada = string_format("%06d|%d|%06.1f|%06.1f|%05d|%02d:%02d:%02d", nSeq, tipo, spPress / 10, spTemp / 10, vol, horarioLocal->tm_hour, horarioLocal->tm_min, horarioLocal->tm_sec);

            adicionaFinal(stringFormatada);
            nSeqGeral++;
        }

        ret = WaitForSingleObject(hEventComunicacao, 1000+(rand()%4000));
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento != 0 && listSize < 100);
}

void comunicacaoSCADA(LPVOID id) {
    std::cout << "Thread " << id << " de comunicacao está enviando mensagens" << std::endl;
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
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
        }
        
        ret = WaitForSingleObject(hEventComunicacao, 500);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento != 0 && listSize < 100);
}

void comunicacaoAlarme(LPVOID id) {
    std::cout << "Thread " << id << " de comunicacao está enviando mensagens" << std::endl;
    DWORD ret;
    int nTipoEvento = 0;
    do {
        if (onOffComunicacao) {
            int nSeq = nSeqGeral;
            int tipo = 55;
            int id = (rand() % 10000);
            int prioridade = (rand() % 1000);
            struct tm* horarioLocal = getHorarioLocal();

            std::string stringFormatada = string_format("%06d|%d|%04d|%03d|%02d:%02d:%02d", nSeq, tipo, id, prioridade, horarioLocal->tm_hour, horarioLocal->tm_min, horarioLocal->tm_sec);
        
            adicionaFinal(stringFormatada);
            nSeqGeral++;
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

        struct Node* a = first;

        if (first == NULL) {
            temp = new Node(data, NULL);
            first = temp;
            first->next = first;
        }
        else {
            do {
                a = a->next;
            } while (a->next != first);

            temp = new Node(data, a->next);
            a->next = temp;
        }
        listSize++;
        std::cout << listSize << std::endl;
    }
}

void adicionaFinalRetirada(std::string data) {
    if (listSize == 100) {
        onOffComunicacao = DESATIVADO;
    }
    else {
        // Inicializa o node
        struct Node* temp;
        temp = new Node();

        struct Node* a = first;

        if (first == NULL) {
            temp = new Node(data, NULL);
            first = temp;
            first->next = first;
        }
        else {
            do {
                a = a->next;
            } while (a->next != first);

            temp = new Node(data, a->next);
            a->next = temp;
        }
        listSize++;
        std::cout << listSize << std::endl;
    }
}


// THREADS SECUNDARIAS

void* retiradaDadosOtimizacao() {
    return (void*)NULL;
}

void* retiradaDadosProcesso() {
    std::cout << "Caractere P digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* retiradaAlarme() {
    std::cout << "Caractere A digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* exibicaoDadosOtimizacao() {
    std::cout << "Caractere T digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* exibicaoDadosProcesso() {
    std::cout << "Caractere R digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* exibicaoAlarme() {
    std::cout << "Caractere L digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
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