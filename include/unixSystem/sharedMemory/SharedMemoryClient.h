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

#include "unixSystem/sharedMemory/SharedMemoryFormat.h"
#include "unixSystem/sharedMemory/Segment.h"

namespace unixSystem
{
	namespace sharedMemory
	{
		class SharedMemoryClient :
				public base::Identificable
		{
		public:
			SharedMemoryClient(void);
			virtual ~SharedMemoryClient(void);

			SharedMemoryClient& setFileToKey(
					const fileUtil::File& a_fileToKey );

			bool open(void);
			void close(void);

			//Segment& segment(
			//		size_t a_segmentNum );

		private:
			SharedMemoryFormat* m_pSharedMemory;
			fileUtil::File m_fileToKey;
			key_t m_sharedMemoryKey;
			int m_sharedMemoryID;

		};
	}//namespace sharedMemory
}//namespace unixSystem

