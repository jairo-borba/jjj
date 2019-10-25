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
#include "unixSystem/semaphore/BinarySemaphoreClient.h"
#include "appUtil/JJJException.h"
#include "appUtil/Shortcuts.h"
#include <cstring>
#include <cstdio>

namespace unixSystem
{
	namespace semaphore
	{
		BinarySemaphoreClient::BinarySemaphoreClient(void)
		{
			m_semaphoreID		= -1;
			m_semaphoreKey		= 0;
			m_semaphoreNumber	= 0;
		}
		BinarySemaphoreClient::~BinarySemaphoreClient(void)
		{
		}

		BinarySemaphoreClient& BinarySemaphoreClient::setFileToKey(
				const fileUtil::File& a_fileToKey )
		{
			m_fileToKey = a_fileToKey;
			return *this;
		}

		bool BinarySemaphoreClient::open(void)
		{
			RETURN_IF( m_semaphoreID != -1, true );
			m_semaphoreKey = ftok( m_fileToKey.fullName().c_str(), 1 );
			m_semaphoreID = semget( m_semaphoreKey, 0, 0 );
			RETURN_IF( m_semaphoreID == -1, false );

			return true;
		}

		bool BinarySemaphoreClient::setVal(
				unsigned short int a_semaphoreNumber,
				unsigned short int a_val )
		{
			RETURN_IF( m_semaphoreID == -1, false );
			int l_semctlRet = semctl(
					m_semaphoreID,
					a_semaphoreNumber,
					SETVAL,
					a_val );
			appUtil::assert(
					l_semctlRet > -1,
					__FILE__,
					__LINE__,
					"BinarySemaphoreClient::setVal(semctl) - %s",
					strerror(errno) );

			return true;
		}

		bool BinarySemaphoreClient::lock(
				unsigned short int a_semaphoreNumber)
		{
			RETURN_IF( m_semaphoreID == -1, false );
			struct sembuf l_sembuf;
			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = a_semaphoreNumber;
			l_sembuf.sem_op = -1;
			l_sembuf.sem_flg = 0;//SEM_UNDO;

			int l_semopRet = semop( m_semaphoreID, &l_sembuf, 1 );
			while( l_semopRet == -1 && errno == EINTR ){
				l_semopRet = semop( m_semaphoreID, &l_sembuf, 1 );
			}

			bool l_ret = l_semopRet == 0;

			return l_ret;
		}
		void BinarySemaphoreClient::unlock(
				unsigned short int a_semaphoreNumber)
		{
			struct sembuf l_sembuf;
			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = a_semaphoreNumber;
			l_sembuf.sem_op = 1;
			l_sembuf.sem_flg = 0;//SEM_UNDO;

			int l_semopRet = semop( m_semaphoreID, &l_sembuf, 1 );
			while( l_semopRet == -1 && errno == EINTR ){
				l_semopRet = semop( m_semaphoreID, &l_sembuf, 1 );
			}

			appUtil::assert(
					l_semopRet == 0,
					__FILE__,
					__LINE__,
					"BinarySemaphoreClient::unlock::semop - %s",
					strerror(errno) );
		}

		void BinarySemaphoreClient::close(void)
		{
		}

		int BinarySemaphoreClient::semaphoreID(void) const
		{
			return m_semaphoreID;
		}
	}//namespace semaphore
}//namespace unixSystem
