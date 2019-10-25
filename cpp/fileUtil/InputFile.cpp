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
#include "fileUtil/InputFile.h"
#include <iostream>
#include <sstream>
#include "appUtil/JJJException.h"
#include <cstring>

namespace fileUtil
{

	InputFile::InputFile(void) :
			m_bufferSize( DEFAULT_BUFFER_SIZE )
	{
		this->m_buffer = NULL;
		this->m_maxLineSize = MAX_LINE_SIZE;

		this->m_startByte = 0;
		this->m_maxSize = 0;
		this->m_totalBytesReady = 0;
		this->m_gcount = 0;

		this->m_ptrAtualPosition = 0;
		this->m_bufferRemainLen = 0;
		this->m_lineLength = 0;
		this->m_ptrLine = NULL;
	}

	InputFile::~InputFile(void)
	{
		this->close( );

		if( this->m_buffer != NULL )
		{
			delete[] this->m_buffer;
		}
	}

	const File& InputFile::file( ) const
	{
		return this->m_file;
	}

	void InputFile::setFile( const File& a_file )
	{
		this->m_file = a_file;
	}

	void InputFile::open( )
	{

		m_gzInputStream.setFile( this->m_file );
		m_gzInputStream.open();
		
		if( !m_gzInputStream.isOpen() ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
				"File not found or permission denied <%s>",
				this->m_file.fullName( ).c_str() );
		}

		this->m_totalBytesReady = 0;

		if( this->m_buffer == NULL ){
			this->m_buffer =
					new char[this->m_bufferSize + this->m_maxLineSize + 1];
		}
	}

	bool InputFile::isOpen( ) const
	{
		return m_gzInputStream.isOpen();
	}

	void InputFile::close( )
	{
		this->m_startByte = 0;
		this->m_maxSize = 0;
		this->m_totalBytesReady = 0;
		this->m_gcount = 0;

		this->m_ptrAtualPosition = NULL;
		this->m_bufferRemainLen = 0;

		m_gzInputStream.close();
	}

	const char* InputFile::read( )
	{
		this->m_gcount = m_gzInputStream.read(
				this->m_buffer,
				this->m_bufferSize );
		if( this->m_gcount <= 0 ){
			return NULL;
		}

		if( !m_gzInputStream.eof() ){
			
			this->m_gcount += m_gzInputStream.getLine(
					&this->m_buffer[this->m_gcount],
					this->m_maxLineSize );
		}

		this->m_totalBytesReady += this->m_gcount;

		return this->m_buffer;
	}

	int InputFile::gcount( ) const
	{
		return this->m_gcount;
	}

	bool InputFile::nextLine( )
	{
		if( this->m_ptrAtualPosition == NULL ||
				this->m_bufferRemainLen <= 0 ){

			if( m_gzInputStream.eof() ){
				return false;
			}
			this->m_lineLength = 0;

			this->m_ptrAtualPosition = this->read( );
			this->m_bufferRemainLen = this->gcount( );

			if( this->m_ptrAtualPosition == NULL ){
				return false;
			}
		}

		const char* l_foundPtr =
			reinterpret_cast<const char*>(
					memchr(
							this->m_ptrAtualPosition,
							'\n',
							this->m_bufferRemainLen ));

		int l_lineLength;
		if( l_foundPtr != NULL ){
			++l_foundPtr; //skip \n
			l_lineLength = int(l_foundPtr - this->m_ptrAtualPosition);
		} else {
			l_lineLength = this->m_bufferRemainLen;
		}

		this->m_ptrLine = this->m_ptrAtualPosition;

		if( l_foundPtr == NULL ){
			this->m_ptrAtualPosition = NULL;
			this->m_bufferRemainLen = 0;
		} else {
			this->m_ptrAtualPosition = l_foundPtr;
			this->m_bufferRemainLen -= l_lineLength;
		}

		this->m_lineLength = l_lineLength - 1;//-1 para nao considerar o \n
		if( this->m_lineLength > MAX_LINE_SIZE ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"Line too big <%d>",
					this->m_lineLength );
		}
		return true;
	}

	const char* InputFile::linePtr( ) const
	{
		return this->m_ptrLine;
	}

	size_t InputFile::lineLength( ) const
	{
		return this->m_lineLength;
	}

}//namespace fileUtil
