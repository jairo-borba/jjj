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
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <sys/sem.h>
#include <fileUtil/File.h>
#include <base/Identificable.h>
#include <list>

namespace unixSystem
{
	namespace sharedMemory
	{
		class SharedMemory :
				public base::Identificable
		{
		public:

			static const size_t DEFAULT_CHANNEL_SIZE = 65536; //64Kb
			static const size_t MAX_CHANNELS = 1024;

			typedef struct tagMessage
			{
				size_t messageLength;
				char messageText[1];
			} Message;

			typedef struct tagSharedMemoyFormat
			{
				size_t numChannels;
				size_t channelSize;
				size_t thisChannel;
				size_t countBytes;
				size_t countMessages;

				char messages[1];

			} SharedMemoyFormat;

			typedef SharedMemoyFormat* SharedMemoyFormatPTR;

			SharedMemory(void);
			virtual ~SharedMemory(void);

			SharedMemory& setFileToKey(
					const fileUtil::File& a_fileToKey );

			bool create(
					size_t a_numChannels = 1,
					size_t a_channelSize = DEFAULT_CHANNEL_SIZE );
			bool open(void);
			void close(void);

			SharedMemory& setChannel(
					size_t a_channel );

			bool addMessage(
					const void* a_msg, size_t a_size );
			size_t countMessagesToSend(void) const;
			bool send(void);

			void receive(void);
			//const void* receivedMessage(
			//		size_t a_index);
			//size_t receivedMessageLength(
			//		size_t a_index);
			//size_t countReceivedMessages(void) const;

		private:

			unsigned short* m_semvalArray;
			SharedMemoyFormat** m_sharedMemoryPtr;
			size_t m_numChannels;
			size_t m_channelSize;
			size_t m_thisChannel;

			std::list<std::string> m_messagesToSend;
			std::list<std::string> m_messagesReceived;

			bool m_bCreated;
			bool m_bOpened;

			int m_sharedMemoryID;
			int m_semaphoreID;

			key_t m_sharedMemoryKey;
			key_t m_semaphoreKey;
			fileUtil::File m_fileToKey;

			size_t m_countBytes;
			size_t m_sharedMemorySize;

			void receiveSharedMemoryHeader(void);
		};
	}//namespace sharedMemory
}//namespace unixSystem

