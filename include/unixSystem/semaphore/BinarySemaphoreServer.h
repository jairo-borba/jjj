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
#include <errno.h>
#include <unistd.h>
#include <sys/sem.h>
#include <fileUtil/File.h>
#include <base/Identificable.h>

namespace unixSystem
{
	namespace semaphore
	{
		class BinarySemaphoreServer :
				public base::Identificable
		{
		public:
			BinarySemaphoreServer(void);
			virtual ~BinarySemaphoreServer(void);

			BinarySemaphoreServer& setFileToKey(
					const fileUtil::File& a_fileToKey );
			void create(
					unsigned short int a_numOfSemaphores );
			void destroy(void);

			int semaphoreID(void) const;
			unsigned short int numOfSemaphores(void) const;

			void lockAll(void);
			void unlockAll(void);

		private:
			int m_semaphoreID;
			key_t m_semaphoreKey;
			unsigned short int m_numOfSemaphores;
			fileUtil::File m_fileToKey;
			unsigned short int* m_semvalArray;
		};
	}//namespace semaphore
}//namespace unixSystem
