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
#include "unixSystem/messageQueue/MessageQueue.h"
#include <errno.h>
#include <cstring>
#include <cstdio>

namespace unixSystem
{
	namespace messageQueue
	{
		MessageQueue::MessageQueue(void)
		{
			m_key = 0xDEADBEEF;
			m_messageQueueID = 0;
			m_recvMessageFlag = IPC_NOWAIT;
			m_opened = false;
			m_created = false;
			m_noMsg = true;
			m_blockMode = false;
		}

		MessageQueue::~MessageQueue(void)
		{
		}

		MessageQueue& MessageQueue::enableBlockMode(
				bool a_blockMode )
		{
			m_blockMode = a_blockMode;
			return *this;
		}

		MessageQueue& MessageQueue::setKey(
				key_t a_key )
		{
			m_key = a_key;
			return *this;
		}

		bool MessageQueue::create(void)
		{
			int l_flags = IPC_CREAT | IPC_EXCL | 0660;

			m_messageQueueID = msgget( m_key, l_flags );

			m_created = m_messageQueueID != -1;

			return m_created;
		}

		bool MessageQueue::open(void)
		{
			int l_flags = 0;

			m_messageQueueID = msgget( m_key, l_flags );

			m_opened = m_messageQueueID != -1;

			return m_opened;
		}

		void MessageQueue::close(void)
		{
			if( !m_created ){
				return;
			}
			msgctl( m_messageQueueID,IPC_RMID, 0 );
		}

		bool MessageQueue::send(
				long a_messageType,
				const void* a_msg,
				size_t a_size )
		{
			int l_codRet = 0;

			m_sendMessage.customMessage.mtype = a_messageType;
			memcpy( m_sendMessage.customMessage.buffer, a_msg, a_size );

			do{
				l_codRet = msgsnd(
						m_messageQueueID,
						&m_sendMessage.msg,
						a_size,
						0 /*IPC_NOWAIT generates EAGAIN*/ );

			}   while( l_codRet == -1 && (errno == EINTR) );

			bool l_bRet = l_codRet != -1;

			return l_bRet;
		}

		void MessageQueue::receive(
				long a_messageType )
		{
			int l_codRet;

			do{
				l_codRet = msgrcv(
					m_messageQueueID,
					&m_recvMessage.msg,
					MAX_MESSAGE_SIZE,
					a_messageType,
					m_recvMessageFlag
				);

			} while( l_codRet == -1 && errno == EINTR );

			if( l_codRet == -1 ){
				m_noMsg = true;
				m_recvMessage.customMessage.size = 0;
			} else {
				m_noMsg = false;
				m_recvMessage.customMessage.size = l_codRet;
			}
		}

		bool MessageQueue::noMsg(void) const
		{
			return m_noMsg;
		}

		const char* MessageQueue::recvMessageBuffer(void) const
		{
			return m_recvMessage.customMessage.buffer;
		}
		size_t MessageQueue::recvMessageSize(void) const
		{
			return m_recvMessage.customMessage.size;
		}

	}//namespace messageQueue
}//namespace unixSystem
