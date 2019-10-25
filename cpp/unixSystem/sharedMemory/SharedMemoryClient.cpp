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
#include "unixSystem/sharedMemory/SharedMemoryClient.h"

namespace unixSystem
{
	namespace sharedMemory
	{
		SharedMemoryClient::SharedMemoryClient(void)
		{
			m_pSharedMemory = NULL;
			m_sharedMemoryID = -1;
			m_sharedMemoryKey = -1;
		}
		SharedMemoryClient::~SharedMemoryClient(void)
		{
		}
		SharedMemoryClient& SharedMemoryClient::setFileToKey(
				const fileUtil::File& a_fileToKey )
		{
			m_fileToKey = a_fileToKey;
			return *this;
		}
		bool SharedMemoryClient::open(void)
		{
			m_sharedMemoryKey	= ftok( m_fileToKey.fullName().c_str(), 0 );
			m_sharedMemoryID = shmget( m_sharedMemoryKey , 0, 0 );
			if( m_sharedMemoryID == -1 ){
				return false;
			}

			//ANEXA SHARED MEMORY AO PROCESSO
			m_pSharedMemory = (SharedMemoryFormat*)shmat( m_sharedMemoryID, 0, 0 );
			if( m_pSharedMemory == (SharedMemoryFormat*)-1 ){
				return false;
			}
			if( memcmp( m_pSharedMemory->header.magicCode, MAGIC_CODE, sizeof MAGIC_CODE ) != 0 ){
				this->close();
				return false;
			}

			return true;
		}
		void SharedMemoryClient::close(void)
		{
			if( m_pSharedMemory == 0 ){
				return;
			}
			shmdt( m_pSharedMemory );
			m_pSharedMemory = NULL;
		}
	}//namespace sharedMemory
}//namespace unixSystem

