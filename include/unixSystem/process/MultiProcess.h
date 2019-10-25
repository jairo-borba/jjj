/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 jairo-borba jairo.borba.junior@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#pragma once

#include "unixSystem/signals/SignalHandler.h"
#include "unixSystem/signals/SignalChild.h"
#include "base/Command.h"

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>


namespace unixSystem
{
	namespace process
	{
		class MultiProcess
		{
		public:
			MultiProcess(void);
			virtual ~MultiProcess(void);

			/**
				Dispara o processo filho que executar� o m�todo execute( ) da Command.
				Se o n�mero m�ximo de processos filhos configurado atrav�s do m�todo setNumMaxProc
				for igual a 0 (zero), run( ) ser� executado serialmente.
				Se o n�mero m�ximo de processos filhos configurado atrav�s do m�todo setNumMaxProc
				for igual ao n�mero de processos filhos j� disparados, o programa ficar� bloqueado
				aguardando que algum processo filho  retorne para poder disparar o novo processo
				filho.

				\note Este m�todo verifica a fila de SIGCHLD, se o n�mero de processos filhos exceder
				a capadidade da	fila, por seguran�a, o programa ficar� bloqueado aguardando que algum
				processo filho  retorne para poder disparar o novo processo	filho.

				\return Retorna PID do novo processo filho disparado ( > 0 ), 0 (zero) se run( ) foi executado
				serialmente ou -1 se ocorreu erro.
			*/
			pid_t start(void);

			/**
				Configura o n�mero m�ximo de processos filhos do processo atual.
				Altera o comportamento do m�todo start( ).
				\param numMaxProc N�mero m�ximo de processos filhos do processo atual.
			*/
			static void setNumMaxProc(
					long numMaxProc );

			/**
				Aguarda todos os processos filhos retornarem.
			*/
			static void waitAllChildren( );

			void setCommand(
					base::Command* a_pCommand );

		private:

			/**
				Inicializa o tratamento do sinal SIGCHLD, tratamento importante para
				gerenciar os processos filhos (gerenciamento de quantidade, limpeza de zumbis, etc).
			*/
			static void initSigCHLD( );

			///Armazena o n�mero m�ximo de processos filhos que podem ser disparados.
			static size_t m_numMaxProc;

			base::Command* m_pCommand;

		};

	}//namespace process
}//namespace unixSystem
