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

namespace unixSystem
{
	namespace sharedMemory
	{
		static const size_t MAX_NAME_SIZE			= 64;
		static const size_t MAX_DESCRIPTION_SIZE	= 256;
		static const char MAGIC_CODE[] = "AVL_@JJJ_SHM_HEADER";

		typedef struct tagHeader
		{
			char magicCode[sizeof MAGIC_CODE];
			time_t creationDate;
			size_t numSegments;
			size_t segmentSize;
			char name[MAX_NAME_SIZE+1];
			char description[MAX_DESCRIPTION_SIZE+1];
		} Header;

		typedef struct tagSHM_FORMAT
		{
			Header header;
			char segments[1];

		} SharedMemoryFormat;
	}//namespace sharedMemory
}//namespace unixSystem

