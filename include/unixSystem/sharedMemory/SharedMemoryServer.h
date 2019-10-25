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
#include <fileUtil/File.h>
#include <base/Identificable.h>
#include "unixSystem/sharedMemory/SharedMemoryFormat.h"
#include "unixSystem/sharedMemory/Segment.h"

namespace unixSystem
{
	namespace sharedMemory
	{
		class SharedMemoryServer :
				public base::Identificable
		{
		public:
			static const size_t DEFAULT_SEGMENT_SIZE	= 65536;	//64Kb
			static const size_t DEFAULT_NUM_OF_SEGMENTS	= 1;
			static const size_t MAX_SHARED_MEMORY_SIZE	= 536870912;	//500M

			SharedMemoryServer(void);
			virtual ~SharedMemoryServer(void);

			SharedMemoryServer& setFileToKey(
					const fileUtil::File& a_fileToKey );

			bool create(void);
			void destroy(void);

			void setNumSegments(
					size_t a_numSegments );
			void setSegmentSize(
					size_t a_segmentSize );

			void* segmentAddr(
					size_t a_segmentNumber );

		private:
			size_t m_numSegments;
			size_t m_segmentSize;
			size_t m_sharedMemorySize;
			int m_sharedMemoryID;
			key_t m_sharedMemoryKey;
			fileUtil::File m_fileToKey;
			SharedMemoryFormat* m_pSharedMemory;
		};
	}//namespace sharedMemory
}//namespace unixSystem

