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
#include "unixSystem/signals/SignalHandler.h"
#include <cstring>

namespace unixSystem
{
	namespace signals
	{
		SignalHandler *SignalHandler::m_hInstance = NULL;

		SignalHandler::SignalHandler(void)
		{
			m_pSignalActionCHLD		= NULL;
			m_pSignalActionURG		= NULL;
			m_pSignalActionHUP		= NULL;
			m_pSignalActionIO		= NULL;
			m_pSignalActionPIPE		= NULL;
			m_pSignalActionALRM		= NULL;

			initSignalAction( );
		}

		SignalHandler *SignalHandler::getInstance(void)
		{
			if( m_hInstance == NULL )
			{
				SignalHandler::m_hInstance = new SignalHandler;
			}

			return SignalHandler::m_hInstance;
		}

		SignalHandler::~SignalHandler(void)
		{
		}

		void SignalHandler::acceptSignalCHLD(
				const SignalAction& a_action )
		{
			if( m_pSignalActionCHLD != NULL )
			{
				delete m_pSignalActionCHLD;
			}

			m_pSignalActionCHLD = a_action.clone( );

			this->signalAction( SIGCHLD );
		}

		void SignalHandler::acceptSignalURG(
				const SignalAction& a_action )
		{
			if( m_pSignalActionURG != NULL )
			{
				delete m_pSignalActionURG;
			}

			m_pSignalActionURG = a_action.clone( );

			this->signalAction( SIGURG );
		}

		void SignalHandler::acceptSignalHUP(
				const SignalAction& a_action )
		{
			if( m_pSignalActionHUP != NULL )
			{
				delete m_pSignalActionHUP;
			}

			m_pSignalActionHUP = a_action.clone( );

			this->signalAction( SIGHUP );
		}

		void SignalHandler::acceptSignalIO(
				const SignalAction& a_action )
		{
			if( m_pSignalActionIO != NULL )
			{
				delete m_pSignalActionIO;
			}

			m_pSignalActionIO = a_action.clone( );

			this->signalAction( SIGIO );
		}

		void SignalHandler::acceptSignalPIPE(
				const SignalAction& a_action )
		{
			if( m_pSignalActionPIPE != NULL )
			{
				delete m_pSignalActionPIPE;
			}

			m_pSignalActionPIPE = a_action.clone( );

			this->signalAction( SIGPIPE );
		}

		void SignalHandler::acceptSignalALRM(
				const SignalAction& a_action )
		{
			if( m_pSignalActionALRM != NULL )
			{
				delete m_pSignalActionALRM;
			}

			m_pSignalActionALRM = a_action.clone( );

			this->signalAction( SIGALRM );
		}

		void SignalHandler::signalHandler(
				int signalNumber )
		{
			switch( signalNumber )
			{
			case SIGCHLD:
				if( SignalHandler::m_hInstance->m_pSignalActionCHLD != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionCHLD->run( );
				}
				break;

			case SIGURG:
				if( SignalHandler::m_hInstance->m_pSignalActionURG != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionURG->run( );
				}
				break;

			case SIGHUP:
				if( SignalHandler::m_hInstance->m_pSignalActionHUP != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionHUP->run( );
				}
				break;

			case SIGIO:
				if( SignalHandler::m_hInstance->m_pSignalActionIO != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionIO->run( );
				}
				break;

			case SIGPIPE:
				if( SignalHandler::m_hInstance->m_pSignalActionPIPE != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionPIPE->run( );
				}
				break;

			case SIGALRM:
				if( SignalHandler::m_hInstance->m_pSignalActionALRM != NULL )
				{
					SignalHandler::m_hInstance->m_pSignalActionALRM->run( );
				}
				break;

			default:
				break;
			}
		}

		void SignalHandler::initSignalAction( )
		{
			// Zera a estrutura m_signalAction
			memset( &m_signalAction, 0, sizeof m_signalAction );
			// Aponta a funcao que sera chamada nas sinalizacoes
			m_signalAction.sa_handler = signalHandler;
			// Inicializa as flags
			sigemptyset( &m_signalAction.sa_mask );
			setSignalFlags( SA_NOCLDSTOP | SA_RESTART );
		}

		void SignalHandler::setSignalFlags( int flags )
		{
			m_signalAction.sa_flags = flags;
		}

		void SignalHandler::signalAction(
				int signo )
		{
			sigaction( signo, &m_signalAction, 0 );
		}

	}//namespace signals
}//namespace unixSystem

