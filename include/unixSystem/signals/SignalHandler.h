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

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "base/Command.h"
#include "appUtil/JJJException.h"
#include "unixSystem/signals/SignalAction.h"

namespace unixSystem
{
	namespace signals
	{
		class SignalHandler
		{
		public:

			static SignalHandler *getInstance( );

			///Destrutor.
			virtual ~SignalHandler( );

			/**
				Especifica o objeto que ir� tratar o sinal SIGCHLD.
				\param Objeto de tratamento espec�fico do sinal SIGCHLD.
			*/
			void acceptSignalCHLD(
					const SignalAction& a_action );

			/**
				Especifica o objeto que ir� tratar o sinal SIGURG.
				\param Objeto de tratamento espec�fico do sinal SIGURG.
			*/
			void acceptSignalURG(
					const SignalAction& a_action );

			/**
				Especifica o objeto que ir� tratar o sinal SIGHUP.
				\param Objeto de tratamento espec�fico do sinal SIGHUP.
			*/
			void acceptSignalHUP(
					const SignalAction& a_action );

			/**
				Especifica o objeto que ir� tratar o sinal SIGIO.
				\param Objeto de tratamento espec�fico do sinal SIGIO.
			*/
			void acceptSignalIO(
					const SignalAction& a_action );

			/**
				Especifica o objeto que ir� tratar o sinal SIGPIPE.
				\param Objeto de tratamento espec�fico do sinal SIGPIPE.
			*/
			void acceptSignalPIPE(
					const SignalAction& a_action );

			/**
				Especifica o objeto que ir� tratar o sinal SIGALRM.
				\param Objeto de tratamento espec�fico do sinal SIGALRM.
			*/
			void acceptSignalALRM(
					const SignalAction& a_action );

		private:
			/**
				Construtor da classe.
				\note Construtor privado, o objeto dessa classe � �nico (classe singleton)
				e dever� ser obtido por UnixSignals::getInstance( ).
			*/
			SignalHandler(void);

			static SignalHandler *m_hInstance;

			SignalAction *m_pSignalActionCHLD;
			SignalAction *m_pSignalActionURG;
			SignalAction *m_pSignalActionHUP;
			SignalAction *m_pSignalActionIO;
			SignalAction *m_pSignalActionPIPE;
			SignalAction *m_pSignalActionALRM;

			struct sigaction m_signalAction;

			static void signalHandler(
					int signalNumber );
			///Inicializa o sistema de captura de sinais do sistema operacional na aplica��o.
			void initSignalAction( );
			///Chama a fun��o sigaction da API do UNIX.
			void signalAction(
					int signo );
			///Configura a estrutura sigaction.
			void setSignalFlags(
					int flags );
		};

	}//namespace signals
}//namespace unix
