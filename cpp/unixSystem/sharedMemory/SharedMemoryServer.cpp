
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
 */#include "unixSystem/sharedMemory/SharedMemoryServer.h"
#include <cstring>
#include <cstdio>
#include "appUtil/JJJException.h"
#include "appUtil/Shortcuts.h"
#include "appUtil/SafeStringDef.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

namespace unixSystem
{
	namespace sharedMemory
	{
		SharedMemoryServer::SharedMemoryServer(void)
		{
			m_numSegments		= DEFAULT_NUM_OF_SEGMENTS;
			m_segmentSize		= DEFAULT_SEGMENT_SIZE;
			m_sharedMemoryID	= 0;
			m_sharedMemoryKey	= 0;
			m_sharedMemorySize	= 0;
			m_pSharedMemory 	= NULL;
		}
		SharedMemoryServer::~SharedMemoryServer(void)
		{
		}
		SharedMemoryServer& SharedMemoryServer::setFileToKey(
				const fileUtil::File& a_fileToKey )
		{
			m_fileToKey = a_fileToKey;
			return *this;
		}

		void SharedMemoryServer::setNumSegments(
				size_t a_numSegments )
		{
			m_numSegments = a_numSegments;
		}
		void SharedMemoryServer::setSegmentSize(
				size_t a_segmentSize )
		{
			m_segmentSize = a_segmentSize;
		}

		bool SharedMemoryServer::create(void)
		{
			if( m_numSegments * m_segmentSize > MAX_SHARED_MEMORY_SIZE ){
				throw appUtil::JJJException( __FILE__, __LINE__,
					"Exceeded MAX_SHARED_MEMORY_SIZE <%d>, required <%d>, NumOfSegmenst<%d>, SegmentSize<%d>",
					MAX_SHARED_MEMORY_SIZE, m_numSegments * m_segmentSize, m_numSegments, m_segmentSize );
			}

			if( m_numSegments == 0 ){
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"Number of segments not defined" );
			}

			if( m_segmentSize == 0 ){
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"Segment size not defined" );
			}

			m_sharedMemorySize	= sizeof( Header ) + ( m_numSegments * m_segmentSize );
			m_sharedMemoryKey	= ftok( m_fileToKey.fullName().c_str(), 0 );

			//CRIA SHARED MEMORY
			m_sharedMemoryID	= shmget( m_sharedMemoryKey , m_sharedMemorySize, IPC_CREAT | 0600 );
			if( m_sharedMemoryID == -1 ){

				m_sharedMemoryID	= 0;
				m_sharedMemoryKey	= 0;
				m_sharedMemorySize	= 0;
				m_pSharedMemory		= NULL;

				if( errno == EEXIST ){
					return false;
				}
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"SharedMemoryServer::create::shmget - %s",
						strerror(errno) );
			}

			//ANEXA SHARED MEMORY AO PROCESSO
			m_pSharedMemory = (SharedMemoryFormat *)shmat( m_sharedMemoryID, 0, 0 );
			if( m_pSharedMemory == (SharedMemoryFormat *)-1 ){
				shmctl( m_sharedMemoryID, IPC_RMID, 0 );
				m_sharedMemoryID	= 0;
				m_sharedMemoryKey	= 0;
				m_sharedMemorySize	= 0;
				m_pSharedMemory		= NULL;
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"SharedMemoryServer::create::shmat - %s",
						strerror(errno) );
			}

			memset( m_pSharedMemory, 0, m_sharedMemorySize );
			memcpy(
					m_pSharedMemory->header.magicCode,
					MAGIC_CODE,
					sizeof m_pSharedMemory->header.magicCode );
			m_pSharedMemory->header.numSegments = m_numSegments;
			m_pSharedMemory->header.segmentSize = m_segmentSize;
			safeSPrintf(
					m_pSharedMemory->header.name,
					sizeof m_pSharedMemory->header.name,
					"%s" ,
					this->label().c_str() );
			safeSPrintf(
					m_pSharedMemory->header.description,
					sizeof m_pSharedMemory->header.description,
					"%s" ,
					this->description().c_str() );

			time( &m_pSharedMemory->header.creationDate );

			return true;
		}

		void* SharedMemoryServer::segmentAddr( size_t a_segmentNumber )
		{
			if( m_pSharedMemory == NULL ){
				return NULL;
			}
			void* l_addr =
					(m_pSharedMemory->segments) +
					(a_segmentNumber * m_pSharedMemory->header.segmentSize);

			return l_addr;
		}

		void SharedMemoryServer::destroy(void)
		{
			if( m_sharedMemoryID == 0 ){
				m_sharedMemoryKey	= ftok( m_fileToKey.fullName().c_str(), 0 );
				m_sharedMemoryID	= shmget( m_sharedMemoryKey , 0, 0 );
				if( m_sharedMemoryID == -1 ){
					return;
				}
			}

			if( m_pSharedMemory != 0 ){
				shmdt( m_pSharedMemory );
			}
			shmctl( m_sharedMemoryID, IPC_RMID, 0 );

			m_sharedMemoryID	= 0;
			m_sharedMemoryKey	= 0;
			m_sharedMemorySize	= 0;
			m_pSharedMemory		= NULL;
		}
	}//namespace sharedMemory
}//namespace unixSystem
