/**********************************************************************************
// Engine (Arquivo de Cabe�alho)
//
// Cria��o:		15 Mai 2014
// Atualiza��o:	09 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descri��o:	A Engine roda aplica��es criadas a partir da classe App.		
//              Para usar a Engine crie uma inst�ncia e chame o m�todo
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
	int Loop();							// inicia la�o principal do motor

public:
	static Window* window;				// janela da aplica��o
	static Input* input;				// entrada da aplica��o
	static App* app;					// aplica��o a ser executada
	static double frameTime;				// tempo do quadro atual

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App * application);		// inicia o execu��o da aplica��o

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

#endif