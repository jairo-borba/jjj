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
#include <fstream>

namespace fileUtil
{
	class GZipInputStream : public GZipStream
	{
	public:
		GZipInputStream(void);
		virtual ~GZipInputStream(void);
		
		void open(
				int bufferSize = -1 );
		void close(void);
		std::streamsize read(
				void* a_pBuffer,
				std::streamsize a_len );
		bool eof(void) const;
		std::streamsize getLine(
				char* a_pBuffer,
				std::streamsize a_len );
		bool isOpen(void) const;
		
	private:
		bool m_bEOF;					//Flag End-Of-File.
		bool m_bFirstHeader;			//Flag - First header in the gz file.
		std::ifstream* m_ifInput;
		
		std::streamsize readTransparent(
				unsigned char* a_pBuffer,
				std::streamsize a_len );
		std::streamsize readStandardGz(
				unsigned char* a_pBuffer,
				std::streamsize a_len );
		std::streamsize readCustomGzCrcOff(
				unsigned char* a_pBuffer,
				std::streamsize a_len );
		std::streamsize readCustomGzCrcOn(
				unsigned char* a_pBuffer,
				std::streamsize a_len );
		void checkHeader(void);
		int getByte(void);
		unsigned long getLong(void);
	};
	
}//namespace fileUtil
