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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//http://www.opengroup.org/onlinepubs/009695399/functions/poll.html
namespace unixSystem
{
	namespace messageQueue
	{
		class MessageQueue
		{
		public:
			static const size_t MAX_MESSAGE_SIZE = 1024;

			union Message
			{
				struct tagMessage
				{
					long mtype;
					char buffer[MAX_MESSAGE_SIZE];
					size_t size;
				} customMessage;

				struct msgbuf msg;

			};

			MessageQueue(void);
			virtual ~MessageQueue(void);

			MessageQueue& setKey(
					key_t a_key );

			bool create(void);
			bool open(void);
			void close(void);

			bool send(
					long a_messageType,
					const void* a_msg,
					size_t a_size );
			void receive(
					long a_messageType );
			bool noMsg(void) const;
			const char* recvMessageBuffer(void) const;
			size_t recvMessageSize(void) const;

			MessageQueue& enableBlockMode(
					bool a_blockMode );

		private:
			Message m_sendMessage;
			Message m_recvMessage;
			key_t m_key;
			int m_messageQueueID;
			bool m_opened;
			bool m_created;
			int m_recvMessageFlag;
			bool m_noMsg;
			bool m_blockMode;
		};
	}//namespace messageQueue
}//namespace unixSystem

