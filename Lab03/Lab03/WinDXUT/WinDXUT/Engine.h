/**********************************************************************************
// Engine (Arquivo de Cabeçalho)
//
// Criação:		15 Mai 2014
// Atualização:	09 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descrição:	A Engine roda aplicações criadas a partir da classe App.		
//              Para usar a Engine crie uma instância e chame o método
//				Start() com um objeto derivado da classe App.
//
**********************************************************************************/

#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

// ---------------------------------------------------------------------------------

#include "Window.h"
#include "Timer.h"
#include "App.h"

// ---------------------------------------------------------------------------------

class Engine
{
private:
	static Timer timer;					// medidor de tempo
	double FrameTime();					// calcula o tempo do quadro
	int Loop();							// inicia laço principal do motor

public:
	static Window* window;				// janela da aplicação
	static Input* input;				// entrada da aplicação
	static App* app;					// aplicação a ser executada
	static double frameTime;				// tempo do quadro atual

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App * application);		// inicia o execução da aplicação

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

#endif