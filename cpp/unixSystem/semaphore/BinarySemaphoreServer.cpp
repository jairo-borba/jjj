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
#include "unixSystem/semaphore/BinarySemaphoreServer.h"
#include "appUtil/JJJException.h"
#include "appUtil/Shortcuts.h"
#include <cstring>
#include <cstdio>

namespace unixSystem
{
	namespace semaphore
	{
		BinarySemaphoreServer::BinarySemaphoreServer(void)
		{
			m_semaphoreID		= -1;
			m_semaphoreKey		= 0;
			m_numOfSemaphores	= 0;
			m_semvalArray		= NULL;
		}
		BinarySemaphoreServer::~BinarySemaphoreServer(void)
		{
		}
		BinarySemaphoreServer& BinarySemaphoreServer::setFileToKey(
				const fileUtil::File& a_fileToKey )
		{
			m_fileToKey = a_fileToKey;
			return *this;
		}

		void BinarySemaphoreServer::create(
				unsigned short int a_numOfSemaphores )
		{
			appUtil::assert(
					m_semaphoreID == -1,
					__FILE__,
					__LINE__,
					"Semaphore already created" );

			m_semaphoreID		= -1;
			m_semaphoreKey		= 0;
			m_numOfSemaphores	= 0;
			m_semvalArray		= 0;

			m_numOfSemaphores	= a_numOfSemaphores;
			m_semaphoreKey		= ftok( m_fileToKey.fullName().c_str(), 1 );

			appUtil::assert(
					m_semaphoreKey > -1,
					__FILE__,
					__LINE__,
					"BinarySemaphoreServer::create(ftok) - %s",
					strerror(errno) );

			m_semaphoreID = semget(
					m_semaphoreKey,
					m_numOfSemaphores,
					IPC_CREAT | IPC_EXCL| 0600 );
			if(errno == EEXIST)
			{
				m_semaphoreID = semget(
						m_semaphoreKey,
						0,
						0 );
			}
			appUtil::assert(
					m_semaphoreID > -1,
					__FILE__,
					__LINE__,
					"BinarySemaphoreServer::create(semget) - %s",
					strerror(errno) );

			//INICIA SEMAFOROS
			m_semvalArray =
					new unsigned short[m_numOfSemaphores];
			memset(
					m_semvalArray,
					0,
					( sizeof (unsigned short) ) * m_numOfSemaphores );

			for(
					unsigned short int semnum = 0;
					semnum < m_numOfSemaphores;
					++semnum )
			{
				m_semvalArray[semnum] = 1;
			}

			int l_semctlRet = semctl(
					m_semaphoreID,
					0,
					SETALL,
					m_semvalArray );

			if( l_semctlRet == -1 ){
				const char* sl_strError = strerror(errno);
				semctl( m_semaphoreID, 0, IPC_RMID, m_semvalArray );
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"BinarySemaphoreServer::create(semctl) - %s",
						sl_strError );
			}
		}
		void BinarySemaphoreServer::lockAll(void)
		{
			appUtil::assert(
					m_semvalArray != NULL && m_semaphoreID >= 0,
					__FILE__,
					__LINE__,
					"lockAll error, semaphores not created" );

			memset(
					m_semvalArray,
					0,
					( sizeof (unsigned short) ) * m_numOfSemaphores );

			for( unsigned short int semnum = 0; semnum < m_numOfSemaphores; ++semnum ){
				m_semvalArray[semnum] = 0;
			}

			int l_semctlRet = semctl( m_semaphoreID, 0, SETALL, m_semvalArray );

			appUtil::assert(
					l_semctlRet > -1,
					__FILE__,
					__LINE__,
					"BinarySemaphoreServer::unlockAll(semctl) - %s",
					strerror(errno) );
		}
		void BinarySemaphoreServer::unlockAll(void)
		{
			appUtil::assert(
					m_semvalArray != NULL && m_semaphoreID >= 0,
					__FILE__,
					__LINE__,
					"unlockAll error, semaphores not created" );
			memset( m_semvalArray, 0, ( sizeof (unsigned short) ) * m_numOfSemaphores );
			for( unsigned short int semnum = 0; semnum < m_numOfSemaphores; ++semnum ){
				m_semvalArray[semnum] = 1;
			}
			int l_semctlRet = semctl( m_semaphoreID, 0, SETALL, m_semvalArray );
			appUtil::assert(
					l_semctlRet != -1,
					__FILE__,
					__LINE__,
					"BinarySemaphoreServer::unlockAll(semctl) - %s",
					strerror(errno) );
		}
		void BinarySemaphoreServer::destroy(void)
		{
			if( m_semaphoreID == -1 ){
				return;
			} else if( m_semaphoreID == 0 ){
				m_semaphoreKey	= ftok( m_fileToKey.fullName().c_str(), 1 );
				m_semaphoreID	= semget( m_semaphoreKey, 0, 0 );
				appUtil::assert(
						m_semaphoreID != -1,
						__FILE__,
						__LINE__,
						"BinarySemaphoreServer::destroy - %s",
						strerror(errno) );
			}
			semctl( m_semaphoreID, 0, IPC_RMID, m_semvalArray );

			delete m_semvalArray;

			m_semaphoreID		= 0;
			m_semaphoreKey		= 0;
			m_numOfSemaphores	= 0;
			m_semvalArray		= NULL;
		}
		unsigned short int BinarySemaphoreServer::numOfSemaphores(void) const
		{
			return m_numOfSemaphores;
		}
		int BinarySemaphoreServer::semaphoreID(void) const
		{
			return m_semaphoreID;
		}
	}//namespace semaphore
}//namespace unixSystem
