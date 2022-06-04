#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>			//_beginthreadex() e _endthreadex()
#include <conio.h>				//_getch
#include <locale>

#define	ESC				0x1B			//Tecla para encerrar o programa

void* comunicacaoDados();
void* retiradaDadosOtimizacao();
void* retiradaDadosProcesso();
void* retiradaAlarme();
void* exibicaoDadosOtimizacao();
void* exibicaoDadosProcesso();
void* exibicaoAlarme();
void* limpaJanelaConsoleExibicaoAlarmes();
void* encerraTarefas();

int main()
{
    char caractereDigitado;
    //Leitura dos caractéres do teclado
    do {
        caractereDigitado = _getch(); //Lê um caractere
        switch (caractereDigitado) {
            case ('c'):
                comunicacaoDados();
                break;
            case ('o'):
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
                encerraTarefas();
                break;
            default:
                std::cout << "Caractere invalido" << std::endl;
        }
    } while (caractereDigitado != ESC);

    // TODO: Lista Circular em Memória
    // TODO: Tarefas de retirada
    // TODO: Tarefa de comunicação de Dados
    // TODO: Tarefas de exibição

    std::cout << "Programa encerrado" << std::endl;
}

// THREADS SECUNDARIAS
// Atencao para o formato conforme o exemplo abaixo!
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