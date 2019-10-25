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

#include "fileUtil/GZipStream.h"
#include "fileUtil/FILE_MODE.h"

namespace fileUtil
{
	class GZipOutputStream : public GZipStream
	{
	public:
		enum FILE_OPEN_MODE
		{
			APPEND,
			TRUNC
		};
		GZipOutputStream(
				enum FILE_OPEN_MODE a_openMode );
		virtual ~GZipOutputStream(void);
		
		void open(
				enum FILE_MODE a_fileMode,
				int a_bufferSize = -1);
		void close(void);
		
		bool isOpen(void) const;

		void enablePreBufferUsing(
				bool use );
		void addLine(
				const char* a_szLine,
				size_t a_length );
		void addBuffer(
				const char* a_szLine,
				size_t a_length );
	private:
		char* m_pPreBuffer;
		size_t m_preBufferPosition;
		bool m_bPreBufferUsing;
		std::streamsize m_usedBufferLength;
		unsigned long m_uncompressedDataSize;
		size_t m_headerLength;
		size_t m_numOfSaves;
		enum FILE_OPEN_MODE m_openMode;
		bool m_bGzOpened;
	
		void gzipOpen(void);
		void gzipClose(void);
		
		void save(void);
		void putLong( unsigned long a_value );
		void createHeader(void);

		std::streamsize write(
				const void* a_buffer,
				size_t a_length );
		
		static const size_t MAX_TRAILER_SIZE;
		static const size_t PRE_BUFFER_SIZE;
	};

}//namespace fileUtil
