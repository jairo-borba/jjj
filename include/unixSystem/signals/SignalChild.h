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

#include "base/Command.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include "unixSystem/signals/SignalAction.h"
#include "unixSystem/signals/ChildProcessInformation.h"


namespace unixSystem
{
	namespace signals
	{
		class SignalChild : public SignalAction
		{
		public:

			SignalChild( );
			virtual ~SignalChild( );

			void run(void);
			SignalChild *clone(void) const;

			/**
				Avisa que deve-se tratar o processo filho especificado pelo pid.
				\param pidChildProcess Pid do processo filho que ser� tratado quando finalizado.
				\note Todo processo filho zumbi � limpo, por�m s�ser�o tratados aqueles especificados.
				nesse m�todo.
			*/
			static void addChildProcess( pid_t pidChildProcess );

			/**
				Obt�m o n�mero de processos filhos que finalizaram com sucesso.
				\note A aplica��o do processo filho dever� retornar zero
				para sinalizar que terminou OK;
				\return N�mero de processos filhos que finalizaram com sucesso.
			*/
			static size_t getNumChildrenOK( );

			/**
				Obt�m o n�mero de processos filhos que finalizaram com erro.
				\note A aplica��o do processo filho dever� retornar n�o zero
				para sinalizar que terminou com erro;
				\return N�mero de processos filhos que finalizaram com erro.
			*/
			static size_t getNumChildrenERROR( );

			/**
				Obt�m o n�mero de processos filhos que finalizaram sofrendo um core dump.
				\return N�mero de processos filhos que finalizaram sofrendo um core dump.
			*/
			static size_t getNumChildrenCORE( );

			/**
				Obt�m o n�mero de processos filhos que foram adicionados atrav�s do m�todo
				addChildProcess( pid_t pidChildProcess ) e que ainda n�o foram retornados.
				\return N�mero de processos filhos que ainda n�o retornaram.
			*/
			static size_t getNumChildrenProcessRunning( );
			/**
				Obt�m o n�mero m�ximo de espa�os na fila que armazena os pids dos processos filhos
				ainda n�o retornados.
				\return N�mero m�ximo de processos filhos ainda n�o retornados.
			*/
			static size_t getMaxSizeSigchldQueue( );

			static void waitAllChildProcess( );
			static void releaseChild( );

		private:
			typedef std::map< pid_t, char, std::less< pid_t > > TP_PID_MAP;
			static const size_t MAX_SIZE_SIGCHLD_QUEUE = 1024;
			static const useconds_t WAIT_DELAY = 500000; //microseconds
			static ChildProcessInformation m_arrayChildren[MAX_SIZE_SIGCHLD_QUEUE];
			static size_t m_numZombieChildrenOK;
			static size_t m_numZombieChildrenERROR;
			static size_t m_numZombieChildrenCORE;
			static size_t m_numStartedChildProcess;
			static size_t m_numReturnedChildProcess;
			static TP_PID_MAP m_mapPidStarted;

		};


	}//namespace signals
}//namespace unixSystem
