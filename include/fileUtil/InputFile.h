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

#include <string>
#include <fstream>
#include <exception>

#include "fileUtil/GZipInputStream.h"

#include "fileUtil/File.h"

namespace fileUtil
{
	class InputFile
	{
		static const std::streamsize DEFAULT_BUFFER_SIZE = 1048576;
		static const int MAX_LINE_SIZE = 16384;

	public:
		InputFile(void);
		virtual ~InputFile(void);

		const File& file( ) const;

		void open( );
		bool isOpen( ) const;
		void open( std::streamsize a_startByte, std::streamsize a_maxSize = 0 );
		void close( );
		void setFile( const File& a_file );
		const char* read( );
		int gcount( ) const;

		bool nextLine( );
		const char* linePtr( ) const;
		size_t lineLength( ) const;

	private:
		File m_file;
		GZipInputStream m_gzInputStream;
		std::streamsize m_bufferSize;
		char* m_buffer;

		std::streamsize m_startByte;
		std::streamsize m_maxSize;
		std::streamsize m_totalBytesReady;
		std::streamsize m_maxLineSize;
		std::streamsize m_gcount;

		//line loader
		std::streamsize m_bufferRemainLen;
		const char* m_ptrAtualPosition;
		std::streamsize m_lineLength;
		const char* m_ptrLine;
	};

}//namespace fileUtil
