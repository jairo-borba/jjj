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
#include "unixSystem/signals/SignalChild.h"

namespace unixSystem
{
	namespace signals
	{

		ChildProcessInformation SignalChild::m_arrayChildren[ MAX_SIZE_SIGCHLD_QUEUE ];
		size_t SignalChild::m_numZombieChildrenOK = 0;
		size_t SignalChild::m_numZombieChildrenERROR = 0;
		size_t SignalChild::m_numZombieChildrenCORE = 0;
		size_t SignalChild::m_numStartedChildProcess = 0;
		size_t SignalChild::m_numReturnedChildProcess = 0;
		SignalChild::TP_PID_MAP SignalChild::m_mapPidStarted;

		SignalChild::SignalChild(void)
		{

		}

		SignalChild::~SignalChild(void)
		{
		}

		void SignalChild::run(void)
		{
			pid_t l_pid = 0;
			int l_childExitStatus = 0;

			l_pid = waitpid( -1, &l_childExitStatus, WNOHANG );
			while( l_pid > 0 )
			{
				for( size_t l_indZombie = 0; l_indZombie < MAX_SIZE_SIGCHLD_QUEUE; ++l_indZombie )
				{
					if( !m_arrayChildren[l_indZombie].computed() ) continue;

					m_arrayChildren[l_indZombie].setProcessID( l_pid );
					m_arrayChildren[l_indZombie].setExitStatus( l_childExitStatus );
					m_arrayChildren[l_indZombie].setComputed( false );

					break;
				}

				l_pid = waitpid( -1, &l_childExitStatus, WNOHANG );
			}
		}

		SignalChild *SignalChild::clone( ) const
		{
			return new SignalChild( *this );
		}

		void SignalChild::addChildProcess(
				pid_t pidChildProcess )
		{
			++SignalChild::m_numStartedChildProcess;
			m_mapPidStarted.insert(
					TP_PID_MAP::value_type( pidChildProcess, ' ' ) );
		}

		size_t SignalChild::getNumChildrenOK( )
		{
			releaseChild( ); //Atualiza as vari�veis
			return m_numZombieChildrenOK;
		}

		size_t SignalChild::getNumChildrenERROR( )
		{
			releaseChild( ); //Atualiza as vari�veis
			return m_numZombieChildrenERROR;
		}

		size_t SignalChild::getNumChildrenCORE( )
		{
			releaseChild( ); //Atualiza as vari�veis
			return m_numZombieChildrenCORE;
		}

		void SignalChild::waitAllChildProcess(void)
		{
			while( SignalChild::getNumChildrenProcessRunning( ) > 0 )
			{
				usleep( WAIT_DELAY );
			}
		}

		size_t SignalChild::getNumChildrenProcessRunning( )
		{
			releaseChild( );
			return
					SignalChild::m_numStartedChildProcess -
					SignalChild::m_numReturnedChildProcess;
		}

		size_t SignalChild::getMaxSizeSigchldQueue( )
		{
			return MAX_SIZE_SIGCHLD_QUEUE;
		}
		void SignalChild::releaseChild( )
		{
			TP_PID_MAP::iterator l_iteratorPid;
			for( size_t sl_indZombie = 0; sl_indZombie < MAX_SIZE_SIGCHLD_QUEUE; ++sl_indZombie )
			{
				if( m_arrayChildren[sl_indZombie].computed() ) continue;

				l_iteratorPid = m_mapPidStarted.find( m_arrayChildren[sl_indZombie].processID() );

				if( l_iteratorPid != m_mapPidStarted.end( ) )
				{//Contabiliza ..
					++SignalChild::m_numReturnedChildProcess;
					int sl_exitStatus = m_arrayChildren[sl_indZombie].exitStatus();
					if( WIFEXITED( sl_exitStatus ) )	// Se o processo filho saiu normalmente.
					{
						if( WEXITSTATUS( sl_exitStatus ) == 0 )	// O processo retornou Zero(0) - Processo retornou OK.
						{
							++m_numZombieChildrenOK;
						}
						else	// O processo retornou nao Zero(0) - Processo retornou ERRO.
						{
							++m_numZombieChildrenERROR;
						}
						m_mapPidStarted.erase( l_iteratorPid );
					}
					else// Se o processo filho n�o saiu normalmente.
					{
						if( WIFSIGNALED( sl_exitStatus ) )
						{
							if( WCOREDUMP( sl_exitStatus ) )//Verifica se foi core dump.
							{
								++m_numZombieChildrenCORE;
							}
						}

						++m_numZombieChildrenERROR;
					}
					m_arrayChildren[sl_indZombie].setComputed( true );
				}
			}
		}


	}//namespace signals
}//namespace unixSystem
