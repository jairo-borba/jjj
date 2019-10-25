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
#include "fileUtil/GZipOutputStream.h"
#include "appUtil/JJJException.h"
#include <zlib.h>
#include <zutil.h>
#include <fstream>
#include "appUtil/SafeStringDef.h"
#include "fileUtil/GZipEnv.h"
#include "fileUtil/FILE_MODE.h"

namespace fileUtil
{
	const size_t GZipOutputStream::MAX_TRAILER_SIZE = 1024;
	const size_t GZipOutputStream::PRE_BUFFER_SIZE = 32768;
	
	GZipOutputStream::GZipOutputStream(
			enum FILE_OPEN_MODE a_openMode ) :
					m_openMode(a_openMode){
		m_usedBufferLength = 0;
		m_uncompressedDataSize = 0;
		m_headerLength = 0;
		m_bGzOpened = false;
		m_pPreBuffer = NULL;
		m_preBufferPosition = 0;
		m_bPreBufferUsing = true;
		m_numOfSaves = 0;
	}
	
	GZipOutputStream::~GZipOutputStream(void){
	}

	bool GZipOutputStream::isOpen(void) const
	{
		return m_bGzOpened;
	}
	
	void GZipOutputStream::addLine(
			const char* a_szLine,
			size_t a_length )
	{
		if( !m_bGzOpened ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZipOutputStream not open" );
		}
		
		if( m_bPreBufferUsing ){
			if( m_preBufferPosition + a_length + 1 > PRE_BUFFER_SIZE ){
				this->write( m_pPreBuffer, m_preBufferPosition );
				this->write( a_szLine, a_length );
				this->write( "\n", 1 );
				m_preBufferPosition = 0;
			} else {
				memcpy( &m_pPreBuffer[m_preBufferPosition], a_szLine, a_length );
				m_preBufferPosition += a_length;
				m_pPreBuffer[m_preBufferPosition++] = '\n';
			}
		} else {
			this->write( a_szLine, a_length );
			this->write( "\n", 1 );
		}
	}

	void GZipOutputStream::addBuffer(
			const char* a_szLine,
			size_t a_length )
	{
		if( !m_bGzOpened ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZipOutputStream not open" );
		}
		
		if( m_bPreBufferUsing ){
			if( m_preBufferPosition + a_length > PRE_BUFFER_SIZE ){
				this->write( m_pPreBuffer, m_preBufferPosition );
				this->write( a_szLine, a_length );		
				m_preBufferPosition = 0;
			} else {
				memcpy( &m_pPreBuffer[m_preBufferPosition], a_szLine, a_length );
				m_preBufferPosition += a_length;
			}
		} else {
			this->write( a_szLine, a_length );
		}
	}
	
	std::streamsize GZipOutputStream::write(
			const void* a_buffer,
			size_t a_length )
	{	
	    m_pGZip->m_zStream.next_in = (Bytef*)a_buffer;
	    m_pGZip->m_zStream.avail_in = (uInt)a_length;
	    
	    m_pGZip->m_crc32 =
	    		::crc32( m_pGZip->m_crc32,
	    				(Bytef*)a_buffer,
						(uInt)a_length );
	
	    while( m_pGZip->m_zStream.avail_in != 0 ){
	
	        if( m_pGZip->m_zStream.avail_out == 0 ){
				
				this->save();
	            this->gzipClose();
	            this->gzipOpen();
	        }
	        
	        this->m_usedBufferLength += m_pGZip->m_zStream.avail_out;
	        this->m_uncompressedDataSize += m_pGZip->m_zStream.avail_in;
	        m_pGZip->m_zErr = deflate(&(m_pGZip->m_zStream), Z_NO_FLUSH);
	        this->m_usedBufferLength -= m_pGZip->m_zStream.avail_out;
	        this->m_uncompressedDataSize -= m_pGZip->m_zStream.avail_in;
	        
	        if( m_pGZip->m_zErr != Z_OK ) break;
	    }
		
	    return static_cast<std::streamsize>(a_length - m_pGZip->m_zStream.avail_in);
	}
	void GZipOutputStream::save(void){
		
		if( m_usedBufferLength == 0 ) return;
		
		std::ofstream l_ofOutput;
		l_ofOutput.open(
			m_file.fullName().c_str( ),
			(this->m_openMode == TRUNC &&
					this->m_numOfSaves == 0 ? std::ios::trunc : std::ios::app) | std::ios::binary );
		
		if( !l_ofOutput.is_open( ) )
		{
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"Open file error <%s>",
					m_file.fullName().c_str()  );
		}
		
		//=========== FLUSH ======================
		uInt l_len;
		bool l_done = false;
		for (;;)
		{
	        l_len = (uInt)(m_pGZip->m_bufferSize - m_pGZip->m_zStream.avail_out);
	
	        if (l_len != 0) {
	        	
	        	if( l_done ){
					this->putLong( m_pGZip->m_crc32 );
					this->putLong( this->m_uncompressedDataSize & 0xffffffff );
				}
				
				l_ofOutput.write(
						reinterpret_cast<const char*>(m_pGZip->m_buffer),
						this->m_usedBufferLength );
	            
	            m_pGZip->m_zStream.avail_out = (uInt)m_pGZip->m_bufferSize;
				m_pGZip->m_zStream.next_out = m_pGZip->m_buffer;
				this->m_usedBufferLength = 0;
	        }
	        if( l_done ) break;
	        
	        this->m_usedBufferLength += m_pGZip->m_zStream.avail_out;
	        this->m_uncompressedDataSize += m_pGZip->m_zStream.avail_in;
	        m_pGZip->m_zErr = deflate( &m_pGZip->m_zStream, Z_FINISH );
	        this->m_usedBufferLength -= m_pGZip->m_zStream.avail_out;
	        this->m_uncompressedDataSize -= m_pGZip->m_zStream.avail_in;
	
	        // Ignore the second of two consecutive flushes:
	        if( l_len == 0 && m_pGZip->m_zErr == Z_BUF_ERROR ) m_pGZip->m_zErr = Z_OK;
	
	        /* deflate has finished flushing only when it hasn't used up
	         * all the available space in the output buffer:
	         */
	        l_done = m_pGZip->m_zStream.avail_out != 0 || m_pGZip->m_zErr == Z_STREAM_END;
	
	        if( m_pGZip->m_zErr != Z_OK && m_pGZip->m_zErr != Z_STREAM_END ) break;
	    }
	    //=========== FLUSH ======================
	    		
		l_ofOutput.close();
		
		++this->m_numOfSaves;
	}
	
	void GZipOutputStream::open(
			enum FILE_MODE a_fileMode,
			int a_bufferSize )
	{
		
		if( m_bGzOpened )
		{
			return;
		}

		m_pGZip->m_bufferSize = a_bufferSize < MIN_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : a_bufferSize;
		m_pGZip->m_buffer = new Byte[m_pGZip->m_bufferSize + MAX_TRAILER_SIZE];
		
		if( m_pGZip->m_buffer == NULL ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZip output - Buffer allocation error" );
		}
		
		this->m_pPreBuffer = new char[PRE_BUFFER_SIZE];
		
		if( this->m_pPreBuffer == NULL ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZip output - PreBuffer allocation error" );
		}
		
		m_preBufferPosition = 0;
		
		m_pGZip->m_fileMode = a_fileMode;
		this->m_uncompressedDataSize = 0;
		this->m_numOfSaves = 0;
		this->gzipOpen();
	}
	
	void GZipOutputStream::gzipOpen(void){
		
		int l_level		= Z_DEFAULT_COMPRESSION; // compression level
		int l_strategy	= Z_DEFAULT_STRATEGY;	 // compression strategy

		m_pGZip->m_zStream.zalloc		= static_cast<alloc_func>( 0 );
		m_pGZip->m_zStream.zfree		= static_cast<free_func	>( 0 );
		m_pGZip->m_zStream.opaque		= static_cast<voidpf	>( 0 );
		m_pGZip->m_zStream.next_in		= Z_NULL;
		m_pGZip->m_zStream.next_out	= Z_NULL;
		m_pGZip->m_zStream.avail_in	= 0;
		
		m_pGZip->m_zErr				= Z_OK;
		m_pGZip->m_crc32				= ::crc32(0L, Z_NULL, 0);
		
		m_usedBufferLength = 0;
		m_pGZip->m_zStream.avail_out = (uInt)m_pGZip->m_bufferSize;
		m_pGZip->m_zStream.next_out	= m_pGZip->m_buffer;
		
		m_pGZip->m_zErr = deflateInit2(
				&m_pGZip->m_zStream,
				l_level,
				Z_DEFLATED,
				-MAX_WBITS,
				MAX_MEM_LEVEL,
				l_strategy );
		if( m_pGZip->m_zErr != Z_OK ) {
			delete[] m_pGZip->m_buffer;
			m_pGZip->m_buffer = NULL;
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZip output - Start error" );
			m_pGZip->m_fileMode = GZSTREAM_MODE_CLOSED;
		}

		this->createHeader();
		m_bGzOpened = true;
	}
	
	void GZipOutputStream::gzipClose(void){
		
		m_pGZip->m_crc32 = ::crc32(0L, Z_NULL, 0);
		m_pGZip->m_zErr = deflateEnd( &m_pGZip->m_zStream );
		this->m_uncompressedDataSize = 0;
		m_bGzOpened = false;
	}
	
	void GZipOutputStream::close(void){

		if( !m_bGzOpened ){
			return;
		}
		
		if( m_bPreBufferUsing ){
			this->write( m_pPreBuffer, m_preBufferPosition );
			m_preBufferPosition = 0;
		}
		
		this->save();
		this->gzipClose();
		
		if( m_pGZip->m_buffer != NULL ){
			delete[] m_pGZip->m_buffer;
			m_pGZip->m_buffer = NULL;
		}
		
		if( this->m_pPreBuffer != NULL ){
			delete [] this->m_pPreBuffer;
			this->m_pPreBuffer = NULL;
		}
	}
		
	void GZipOutputStream::enablePreBufferUsing(
			bool a_use )
	{
		m_bPreBufferUsing = a_use;
	}
	
	void GZipOutputStream::putLong(
			uLong a_value )
	{
		
		int l_n;
		
	    for( l_n = 0; l_n < 4; l_n++) {
	    	
	    	*m_pGZip->m_zStream.next_out++ = static_cast<char>(a_value & 0xFF);
	    	
	    	--m_pGZip->m_zStream.avail_out;
	    	++m_usedBufferLength;
	        
	        a_value >>= 8;
	    }
	}
		
	void GZipOutputStream::createHeader(void)
	{
		
		int l_magic1;
		int l_magic2;
		
		// Peek ahead to check the gzip magic header
	    if( m_pGZip->m_fileMode == GZSTREAM_MODE_STANDARD_GZ ){
	    	l_magic1 = m_gzMagicNumber[0];
	    	l_magic2 = m_gzMagicNumber[1];
		} else if( m_pGZip->m_fileMode == GZSTREAM_MODE_CUSTOM_GZ_CRC_ON ){
			l_magic1 = m_gzCustomMagicNumberCRC_ON[0];
	    	l_magic2 = m_gzCustomMagicNumberCRC_ON[1];
		} else if( m_pGZip->m_fileMode == GZSTREAM_MODE_CUSTOM_GZ_CRC_OFF ){
			l_magic1 = m_gzCustomMagicNumberCRC_OFF[0];
	    	l_magic2 = m_gzCustomMagicNumberCRC_OFF[1];
		} else {
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"GZipOutputStream - Create header: File mode error" );
		}
		
		char l_header[100];
		m_headerLength =
			safeSPrintf( l_header, sizeof l_header,
				"%c%c%c%c%c%c%c%c%c%c",
				l_magic1,
				l_magic2,
				Z_DEFLATED, 0,0,0,0,0,0, OS_CODE );
		
		memcpy( m_pGZip->m_zStream.next_out, l_header, m_headerLength );
		m_usedBufferLength	+= std::streamsize(m_headerLength);
		m_pGZip->m_zStream.avail_out	-= (uInt)m_headerLength;
		m_pGZip->m_zStream.next_out	= &m_pGZip->m_zStream.next_out[m_headerLength];
	}
}//namespace fileUti
