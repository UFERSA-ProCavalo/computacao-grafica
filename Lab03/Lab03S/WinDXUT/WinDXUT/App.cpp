/**********************************************************************************
// App (Código Fonte)
//
// Criação:		11 Jan 2020
// Atualização:	09 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descrição:	Uma classe abstrata para representar uma aplicação
//              Todas as aplicações derivadas da classe App devem
//              sobrescrever os métodos Init, Update, Draw e Finalize.
//
**********************************************************************************/

#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe

// As referências fazem com que não seja necessário verificar
// se os ponteiros de Engine foram inicializados. Sendo uma
// referência, os atributos de App serão atualizados tão logo
// os atributos de Engine recebam valores

Window* & App::window = Engine::window;				// ponteiro para a janela
Input* & App::input = Engine::input;				// ponteiro para a entrada
double & App::frameTime = Engine::frameTime;	    // tempo do último quadro

// -------------------------------------------------------------------------------

App::App()
{
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------