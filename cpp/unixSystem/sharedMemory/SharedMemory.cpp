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
#include "unixSystem/sharedMemory/SharedMemory.h"
#include <cstring>
#include <cstdio>
#include "appUtil/JJJException.h"
#include "appUtil/Shortcuts.h"

namespace unixSystem
{
	namespace sharedMemory
	{
		SharedMemory::SharedMemory(void)
		{
			m_semvalArray = NULL;
			m_sharedMemoryPtr = (SharedMemoyFormat**)-1;
			m_sharedMemoryID = -1;
			m_semaphoreID = -1;
			m_sharedMemoryKey = 0;
			m_semaphoreKey = 0;
			m_bCreated = false;
			m_bOpened = false;
			m_countBytes = 0;
			m_numChannels = 1;
			m_thisChannel = 0;
			m_channelSize = DEFAULT_CHANNEL_SIZE;
			m_sharedMemorySize = DEFAULT_CHANNEL_SIZE;
		}
		SharedMemory::~SharedMemory(void)
		{
		}
		SharedMemory& SharedMemory::setFileToKey(
				const fileUtil::File& a_fileToKey )
		{
			if( !m_bCreated ){
				m_fileToKey = a_fileToKey;
			}
			return *this;
		}

		SharedMemory& SharedMemory::setChannel(
				size_t a_channel )
		{
			m_thisChannel = a_channel;
			return *this;
		}

		bool SharedMemory::create(
				size_t a_numChannels,
				size_t a_channelSize )
		{
			if( a_numChannels > MAX_CHANNELS ){
				throw appUtil::JJJException( __FILE__, __LINE__, "Exceeded num max channels" );
			}

			m_sharedMemorySize	=
					( a_numChannels * a_channelSize ) +
					( sizeof( SharedMemoyFormat ) * a_numChannels );
			m_numChannels = a_numChannels;
			m_channelSize = a_channelSize;

			m_sharedMemoryKey	= ftok( m_fileToKey.fullName().c_str(), 0 );
			m_semaphoreKey		= ftok( m_fileToKey.fullName().c_str(), 1 );

			//CRIA SHARED MEMORY
			m_sharedMemoryID = shmget(
					m_sharedMemoryKey ,
					m_sharedMemorySize,
					IPC_CREAT | 0600 );
			if( m_sharedMemoryID == -1 ){
				if( errno == EEXIST ){
					return false;
				}
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"shmget: %s",
						strerror(errno) );
			}

			//ANEXA SHARED MEMORY AO PROCESSO
			char* l_shmPtr = (char *)shmat( m_sharedMemoryID, 0, 0 );
			if( l_shmPtr == (char *)-1 ){
				shmctl( m_sharedMemoryID, IPC_RMID, 0 );
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"shmat: %s",
						strerror(errno) );
			}

			m_sharedMemoryPtr = new SharedMemoyFormatPTR[m_numChannels];
			memset( l_shmPtr, 0, m_sharedMemorySize );
			size_t l_thisChannel;
			for( l_thisChannel = 0; l_thisChannel < m_numChannels; ++l_thisChannel )
			{

				m_sharedMemoryPtr[l_thisChannel] = (SharedMemoyFormat*)&l_shmPtr[l_thisChannel * m_channelSize];

				m_sharedMemoryPtr[l_thisChannel]->numChannels	= m_numChannels;
				m_sharedMemoryPtr[l_thisChannel]->channelSize	= m_channelSize;
				m_sharedMemoryPtr[l_thisChannel]->thisChannel	= l_thisChannel;
				m_sharedMemoryPtr[l_thisChannel]->countBytes	= 0;
				m_sharedMemoryPtr[l_thisChannel]->countMessages	= 0;
			}

			//CRIA SEMAFORO
			m_semaphoreID = semget(
					m_semaphoreKey,
					m_numChannels,
					IPC_CREAT | IPC_EXCL| 0600 );
			if( m_semaphoreID == -1 ){
				if( errno == EEXIST ){
					return false;
				}
				shmdt( m_sharedMemoryPtr );
				shmctl( m_sharedMemoryID, IPC_RMID, 0 );
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semget: %s", strerror(errno) );
			}

			//INICIA SEMAFOROS
			m_semvalArray = new unsigned short[m_numChannels];
			memset(
					m_semvalArray,
					0,
					( sizeof (unsigned short) ) * m_numChannels );
			for( size_t semnum = 0; semnum < m_numChannels; ++semnum ){
				m_semvalArray[semnum] = 1;
			}

			int l_semctlRet = semctl( m_semaphoreID, 0, SETALL, m_semvalArray );
			if( l_semctlRet == -1 ){
				shmdt( m_sharedMemoryPtr );
				shmctl( m_sharedMemoryID, IPC_RMID, 0 );
				semctl( m_semaphoreID, 0, IPC_RMID, m_semvalArray );
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semctl: %s",
						strerror(errno) );
			}

			m_bCreated = true;

			return true;
		}
		bool SharedMemory::open(void)
		{
			if( m_bCreated ){
				return true;
			}

			m_sharedMemoryKey	= ftok( m_fileToKey.fullName().c_str(), 0 );
			m_semaphoreKey		= ftok( m_fileToKey.fullName().c_str(), 1 );

			m_sharedMemoryID = shmget( m_sharedMemoryKey , 0, 0 );
			if( m_sharedMemoryID == -1 ){
				return false;
			}

			//ANEXA SHARED MEMORY AO PROCESSO
			SharedMemoyFormat* l_shmPtr;
			l_shmPtr = (SharedMemoyFormat*)shmat( m_sharedMemoryID, 0, 0 );
			if( l_shmPtr == (SharedMemoyFormat*)-1 ){
				return false;
			}

			m_numChannels = l_shmPtr->numChannels;
			m_channelSize = l_shmPtr->channelSize;
			m_sharedMemorySize	= m_numChannels * m_channelSize;

			m_sharedMemoryPtr = new SharedMemoyFormat*[m_numChannels];

			size_t l_thisChannel;
			for( l_thisChannel = 0; l_thisChannel < m_numChannels; ++l_thisChannel ){
				m_sharedMemoryPtr[l_thisChannel] = l_shmPtr + ( l_thisChannel * m_channelSize );
			}

			//OBTEM SEMAFORO
			m_semaphoreID = semget( m_semaphoreKey, 0, 0 );
			if( m_semaphoreID == -1 ){
				return false;
			}

			return true;
		}
		void SharedMemory::close(void)
		{
			if( !m_bCreated && !m_bOpened ){
				return;
			}

			shmdt( m_sharedMemoryPtr );

			m_bOpened = false;

			if( !m_bCreated ){
				return;
			}

			shmctl( m_sharedMemoryID, IPC_RMID, 0 );

			semctl( m_semaphoreID, 0, IPC_RMID, m_semvalArray );

			delete m_semvalArray;
			m_semvalArray = NULL;

			m_bCreated = false;

		}
		bool SharedMemory::addMessage(
				const void* a_msg,
				size_t a_size )
		{
			if( m_countBytes + a_size > m_sharedMemorySize ){
				return false;
			}

			m_messagesToSend.push_back(
					std::string( (const char*)a_msg, a_size ) );

			m_countBytes += a_size;

			return true;
		}

		size_t SharedMemory::countMessagesToSend(void) const
		{
			return m_messagesToSend.size();
		}

		bool SharedMemory::send(void)
		{
			if( m_messagesToSend.size() == 0 ){
				return true;
			}

			bool l_bRet = true;
			struct sembuf l_sembuf;
			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = m_thisChannel;
			l_sembuf.sem_op = -1;
			l_sembuf.sem_flg = 0;

			if( semop( m_semaphoreID, &l_sembuf, 1 ) == -1 ){
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semop: %s",
						strerror(errno) );
			}

			//Acessa Shared Memory
			std::list<std::string>::const_iterator l_it;
			size_t l_pos = m_sharedMemoryPtr[m_thisChannel]->countBytes;
			Message* l_msgPtr = NULL;

			for( l_it = m_messagesToSend.begin(); l_it != m_messagesToSend.end(); ++l_it )
			{
				if( l_pos + sizeof( Message ) > m_sharedMemoryPtr[m_thisChannel]->channelSize )
				{
					l_bRet = false;
					break;
				}
				l_msgPtr = (Message*)&m_sharedMemoryPtr[m_thisChannel]->messages[l_pos];
				l_msgPtr->messageLength = l_it->length();
				memcpy( l_msgPtr->messageText, l_it->data(), l_it->length() );
				l_pos += sizeof( size_t ) + l_it->length();
			}

			if( l_bRet )
			{
				m_sharedMemoryPtr[m_thisChannel]->countBytes = l_pos;
				m_sharedMemoryPtr[m_thisChannel]->countMessages += m_messagesToSend.size();
				m_messagesToSend.clear();
				m_countBytes = 0;
			}

			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = m_thisChannel;
			l_sembuf.sem_op = 1;
			l_sembuf.sem_flg = 0;

			if( semop( m_semaphoreID, &l_sembuf, 1 ) == -1 )
			{
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semop: %s",
						strerror(errno) );
			}

			return l_bRet;
		}

		void SharedMemory::receive(void)
		{
			struct sembuf l_sembuf;
			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = m_thisChannel;
			l_sembuf.sem_op = -1;
			l_sembuf.sem_flg = 0;

			if( semop( m_semaphoreID, &l_sembuf, 1 ) == -1 ){
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semop: %s",
						strerror(errno) );
			}

			//Acessa Shared Memory
			m_messagesReceived.clear();
			size_t l_pos = 0;
			Message* l_msgPtr = NULL;

			while( l_pos < m_sharedMemoryPtr[m_thisChannel]->countBytes ){
				l_msgPtr = (Message*)&m_sharedMemoryPtr[m_thisChannel]->messages[l_pos];
				m_messagesReceived.push_back(
						std::string( l_msgPtr->messageText,
								l_msgPtr->messageLength ) );

				l_pos += sizeof( size_t ) + l_msgPtr->messageLength;
			}

			m_sharedMemoryPtr[m_thisChannel]->countBytes = 0;
			m_sharedMemoryPtr[m_thisChannel]->countMessages = 0;
			//Libera Sem

			memset( &l_sembuf, 0, sizeof l_sembuf );

			l_sembuf.sem_num = m_thisChannel;
			l_sembuf.sem_op = 1;
			l_sembuf.sem_flg = 0;

			if( semop( m_semaphoreID, &l_sembuf, 1 ) == -1 ){
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"semop: %s",
						strerror(errno) );
			}
		}
		//const void* SharedMemory::receivedMessage(
		//		size_t a_index)
		//{
		//	return m_messagesReceived[a_index].data();
		//}
		//size_t SharedMemory::receivedMessageLength(
		//		size_t a_index)
		//{
		//	return m_messagesReceived[a_index].length();
		//}
		//size_t SharedMemory::countReceivedMessages(void) const
		//{
		//	return m_messagesReceived.size();
		//}
	}//namespace sharedMemory
}//namespace unixSystem
