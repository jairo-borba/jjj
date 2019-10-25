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
#include "unixSystem/process/MultiProcess.h"

namespace unixSystem
{
	namespace process
	{
		size_t MultiProcess::m_numMaxProc = 1;

		MultiProcess::MultiProcess(void)
		{
			m_pCommand = NULL;
			initSigCHLD( );
		}

		MultiProcess::~MultiProcess(void)
		{
		}

		void MultiProcess::setCommand(
				base::Command* a_pCommand )
		{
			m_pCommand = a_pCommand;
		}

		pid_t MultiProcess::start(void)
		{
			pid_t l_pid;
			long l_retProcFilho;

			if( MultiProcess::m_numMaxProc == 0 )
			{
				m_pCommand->execute( );
				return 0;
			}

			while(
				signals::SignalChild::getMaxSizeSigchldQueue( )
				<=
				signals::SignalChild::getNumChildrenProcessRunning( )
			)
			{
				usleep( 250000 );
			}

			while(
					MultiProcess::m_numMaxProc <=
					signals::SignalChild::getNumChildrenProcessRunning( ) )
			{
				usleep( 250000 );
			}

			l_pid = fork( );

			switch( l_pid )
			{
			case -1:	// Erro no fork.
				break;

			case 0:		// at the child process.
			 	l_retProcFilho = m_pCommand->execute( );
			 	exit( l_retProcFilho );
				break;

			default:	//at the parent process
				signals::SignalChild::addChildProcess( l_pid );
				usleep( 500000 );
				break;
			}

			return l_pid;
		}

		void MultiProcess::initSigCHLD( )
		{
			static bool l_bInitialized = false;

			if( !l_bInitialized )
			{
				signals::SignalHandler::getInstance( )->acceptSignalCHLD(
						signals::SignalChild( ) );
				l_bInitialized = true;
			}
		}

		void MultiProcess::setNumMaxProc(
				long a_numMaxProc )
		{
			MultiProcess::m_numMaxProc = a_numMaxProc;
		}

		void MultiProcess::waitAllChildren( )
		{
			signals::SignalChild::waitAllChildProcess( );
		}

	}//namespace process
}//namespace unixSystem

