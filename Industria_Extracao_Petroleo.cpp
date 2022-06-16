#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>				//_getch
#include <process.h>			//_beginthreadex() e _endthreadex()
#include <locale>
#include <string.h>  

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
void adicionaFinal();
DWORD WINAPI WaitComunicacaoEvent(LPVOID);
void comunicacaoOtimizacao();
void comunicacaoSCADA();
void comunicacaoAlarme();

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
int onOffComunicacao = ATIVADO;
HANDLE hEventComunicacao;
HANDLE hEventTest;
HANDLE hEventEsc;
std::string strings[8];

int main()
{
    HANDLE hThreads[3];
    DWORD dwThreadID;
    DWORD dwExitCode = 0;
    DWORD dwRet;

    char caractereDigitado;
    
    hEventComunicacao = CreateEvent(NULL, FALSE, FALSE, L"Evento");
    hEventTest = CreateEvent(NULL, FALSE, FALSE, L"Teste");
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
                comunicacaoDados();
                break;
            case ('o'):
                PulseEvent(hEventTest);
                retiradaDadosOtimizacao();
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
    CloseHandle(hEventTest);
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

    Sleep(100);

    do {
        if (id == 0) {
            comunicacaoOtimizacao();
            std::cout << "Otimização" << std::endl;
        }
        else if (id == (LPVOID)1) {
            comunicacaoSCADA();
            std::cout << "SCADA" << std::endl;
        }
        else if (id == (LPVOID)2) {
            comunicacaoAlarme();
            std::cout << "Alarme" << std::endl;
        }
        std::cout << "Thread " << id << " esperando evento" << std::endl;
        ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
        nTipoEvento = ret - WAIT_OBJECT_0;
    } while (nTipoEvento == 0);

    std::cout << "Thread " << id << " terminando" << std::endl;

    _endthreadex(0);

    return (0);
}

int len(std::string str)
{
    int length = 0;

    for (int i = 0; str[i] != '\0'; i++){
        length++;
    }

    return length;
}

void split(std::string str, char separator)
{
    int currIndex = 0, i = 0, startIndex = 0, endIndex = 0;
    
    while (i <= len(str)) {
        if (str[i] == separator || i == len(str))
        {
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

void comunicacaoOtimizacao() {
    if (onOffComunicacao) {
        //MONTAR MENSAGEM
        char str[100] = "008755|11|0343.5|0330.0|12500|14:45:00";
        adicionaFinal(str);
    }
}

void comunicacaoSCADA() {
    if (onOffComunicacao) {
        //MONTAR MENSAGEM
        char str[100] = "345666|22|0453.8|0076.3|0034.5|0453.2|12:37:24";
        adicionaFinal(str);
    }
}

void comunicacaoAlarme() {
    if (onOffComunicacao) {
        //MONTAR MENSAGEM
        char str[100] = "007459|55|0005|025|22:12:53";
        adicionaFinal(str);
    }
}

void adicionaFinal(std::string data) {
    if (listSize == 100) {
        return;
    }
    else{
        // Inicializa o node
        struct Node* temp;
        temp = new Node();

        // Entrada de dados
        std::cout << "Insira o dado a ser colocado na lista: ";
        std::cin >> data;

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
    }
}

// THREADS SECUNDARIAS
void* comunicacaoDados() {
    std::cout << "Caractere C digitado" << std::endl;
    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)NULL;
}

void* retiradaDadosOtimizacao() {
    std::cout << "Caractere O digitado" << std::endl;

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
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