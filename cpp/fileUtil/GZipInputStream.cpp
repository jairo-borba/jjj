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
#include "fileUtil/GZipInputStream.h"
#include "appUtil/JJJException.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>
#include <cstring>
#include "fileUtil/GZipEnv.h"
#include "fileUtil/FILE_MODE.h"

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

namespace fileUtil
{
	GZipInputStream::GZipInputStream(void)
	{
		this->m_bEOF = true;
		this->m_bFirstHeader = true;
		m_pGZip->m_fileMode = GZSTREAM_MODE_CLOSED;
		m_ifInput = 0;
	}

	GZipInputStream::~GZipInputStream(void)
	{
	}

	bool GZipInputStream::eof(void) const
	{
		return this->m_bEOF;
	}

	bool GZipInputStream::isOpen(void) const
	{
		return m_pGZip->m_fileMode != GZSTREAM_MODE_CLOSED;
	}

	void GZipInputStream::open(
			int bufferSize)
	{
		if( m_pGZip->m_fileMode != GZSTREAM_MODE_CLOSED ){
			throw appUtil::JJJException(
					__FILE__,
					__LINE__,
					"File has been already opened" );
		}

		m_pGZip->m_bufferSize =
				bufferSize < MIN_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : bufferSize;

		m_pGZip->m_zStream.zalloc		= static_cast<alloc_func>( 0 );
		m_pGZip->m_zStream.zfree		= static_cast<free_func	>( 0 );
		m_pGZip->m_zStream.opaque		= static_cast<voidpf	>( 0 );
		m_pGZip->m_zStream.next_in		= Z_NULL;
		m_pGZip->m_zStream.next_out		= Z_NULL;
		m_pGZip->m_zStream.avail_in		= 0;
		m_pGZip->m_zStream.avail_out	= 0;
		m_pGZip->m_zErr					= Z_OK;
		m_pGZip->m_crc32				= ::crc32(0L, Z_NULL, 0);
		m_pGZip->m_buffer				= new unsigned char[m_pGZip->m_bufferSize + 1];
		m_pGZip->m_zStream.next_in		= m_pGZip->m_buffer;
		this->m_bEOF					= false;
		this->m_bFirstHeader			= true;

		int l_err = inflateInit2(&(m_pGZip->m_zStream), -MAX_WBITS);

		if (l_err != Z_OK || m_pGZip->m_buffer == Z_NULL) {
		    close( );
			throw appUtil::JJJException( __FILE__, __LINE__, "inflateInit2 error" );
		}

		m_ifInput = new std::ifstream;
		m_ifInput->open( m_file.fullName( ).c_str( ), std::ios_base::binary );

		if( !m_ifInput->is_open( ) ){
			close( );
			throw appUtil::JJJException(
				__FILE__,
				__LINE__,
				"Open error - Permission denied or file does not exist <%s>",
				m_file.fullName( ).c_str() );
		}

		this->checkHeader( ); // skip the .gz header
	}

	void GZipInputStream::close(void)
	{
		if( m_pGZip->m_fileMode == GZSTREAM_MODE_CLOSED ){
			return;
		}

		m_pGZip->m_fileMode = GZSTREAM_MODE_CLOSED;
		if( m_pGZip->m_buffer != NULL ){
			delete[] m_pGZip->m_buffer;
			m_pGZip->m_buffer = NULL;
		}
		if( m_ifInput != NULL ){
			m_ifInput->close();
			delete m_ifInput;
			m_ifInput = NULL;
		}

		this->m_bEOF = true;
		this->m_bFirstHeader = true;
		m_pGZip->m_fileMode = GZSTREAM_MODE_CLOSED;
	}

	std::streamsize GZipInputStream::getLine(
			char* a_pBuffer,
			std::streamsize a_len )
	{
		std::streamsize i;

		for( i = 0; i < a_len; ++i ){
			if( this->read( a_pBuffer, 1 ) == 1 ){

				if( *a_pBuffer == '\n' ) break;

				++a_pBuffer;
			} else {
				break;
			}
		}

		return i;
	}

	std::streamsize GZipInputStream::read(
			void* a_pBuffer,
			std::streamsize a_len )
	{
		if( a_pBuffer == NULL || a_len == 0 ){
			return 0;
		}

		Bytef* l_pBuffer = 	reinterpret_cast<Bytef*>(a_pBuffer);
		if( m_pGZip->m_fileMode == GZSTREAM_MODE_TRANSPARENT ){
			return readTransparent( l_pBuffer, a_len );
		}

		switch( m_pGZip->m_fileMode )
		{
		case GZSTREAM_MODE_CLOSED:
			return 0;
		case GZSTREAM_MODE_TRANSPARENT:
			return readTransparent( l_pBuffer, a_len );
		case GZSTREAM_MODE_STANDARD_GZ:
			return readStandardGz( l_pBuffer, a_len );
		case GZSTREAM_MODE_CUSTOM_GZ_CRC_ON:
			return readCustomGzCrcOn( l_pBuffer, a_len );
		case GZSTREAM_MODE_CUSTOM_GZ_CRC_OFF:
			return readCustomGzCrcOff( l_pBuffer, a_len );
		}

		return 0;
	}

	std::streamsize GZipInputStream::readTransparent(
			unsigned char* a_pBuffer,
			std::streamsize a_len ){

		if( this->m_bEOF ){
			return 0;
		}

		std::streamsize l_bytesRead = 0;

		if( m_pGZip->m_zStream.avail_in == 0 ){

			m_ifInput->read( reinterpret_cast<char*>(a_pBuffer), a_len );
			this->m_bEOF = m_ifInput->gcount() == 0;
			l_bytesRead = m_ifInput->gcount();

		} else {

			if( a_len <= (std::streamsize)m_pGZip->m_zStream.avail_in ){
				memcpy( a_pBuffer, m_pGZip->m_zStream.next_in, a_len );
				m_pGZip->m_zStream.next_in += a_len;
				m_pGZip->m_zStream.avail_in -= a_len;
				l_bytesRead = a_len;
			} else {
				l_bytesRead = (std::streamsize)m_pGZip->m_zStream.avail_in;
				memcpy( a_pBuffer, m_pGZip->m_zStream.next_in, m_pGZip->m_zStream.avail_in );
				a_pBuffer += m_pGZip->m_zStream.avail_in;
				m_ifInput->read( reinterpret_cast<char*>(a_pBuffer), a_len - m_pGZip->m_zStream.avail_in );
				m_pGZip->m_zStream.avail_in = 0;
				l_bytesRead += m_ifInput->gcount();
			}
		}
		return l_bytesRead;
	}
	std::streamsize GZipInputStream::readStandardGz(
			unsigned char* a_pBuffer,
			std::streamsize a_len ){

		m_pGZip->m_zStream.next_out = a_pBuffer;
		m_pGZip->m_zStream.avail_out = a_len;
		unsigned char* l_pStartForCrc32 = a_pBuffer;

		if( m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) return -1;
    	if( m_pGZip->m_zErr == Z_STREAM_END || this->m_bEOF ) return 0;

		while( m_pGZip->m_zStream.avail_out != 0 ){

			if( m_pGZip->m_zStream.avail_in == 0 && !this->m_bEOF ){

				m_ifInput->read( reinterpret_cast<char*>(m_pGZip->m_buffer), m_pGZip->m_bufferSize );
				m_pGZip->m_zStream.avail_in = m_ifInput->gcount();

				if( m_pGZip->m_zStream.avail_in == 0 ){
					this->m_bEOF = true;
					break;
				}

				m_pGZip->m_zStream.next_in = m_pGZip->m_buffer;
			}

			m_pGZip->m_zErr = ::inflate( &m_pGZip->m_zStream, Z_NO_FLUSH );

			if( m_pGZip->m_zErr == Z_STREAM_END ){

				//Check CRC and original size
				m_pGZip->m_crc32 =
					::crc32( 
					m_pGZip->m_crc32, 
					l_pStartForCrc32,
					static_cast<unsigned int>(m_pGZip->m_zStream.next_out - l_pStartForCrc32) );
				l_pStartForCrc32 = m_pGZip->m_zStream.next_out;

				uLong crc32FromArq = static_cast<uLong>(this->getLong());
				if( crc32FromArq == m_pGZip->m_crc32 )
				{

					this->getLong();//descarta o tamanho do conteúdo descompactado.
					this->checkHeader();

					if( m_pGZip->m_zErr == Z_OK ){
						::inflateReset( &m_pGZip->m_zStream );
						m_pGZip->m_crc32 = ::crc32( 0L, Z_NULL, 0 );
					}

				} else {
					m_pGZip->m_zErr = Z_DATA_ERROR;
				}
			}
			if( m_pGZip->m_zErr != Z_OK || this->m_bEOF ) break;
		}

		m_pGZip->m_crc32 =
					::crc32(
							m_pGZip->m_crc32,
							l_pStartForCrc32,
							static_cast<unsigned int>(m_pGZip->m_zStream.next_out - l_pStartForCrc32) );

		if( a_len == (std::streamsize)m_pGZip->m_zStream.avail_out &&
				(m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) ){
			return -1;
		}

		return a_len - m_pGZip->m_zStream.avail_out;
	}
	std::streamsize GZipInputStream::readCustomGzCrcOn(
			unsigned char* a_pBuffer,
			std::streamsize a_len ){
		m_pGZip->m_zStream.next_out = a_pBuffer;
		m_pGZip->m_zStream.avail_out = a_len;
		Bytef* l_pStartForCrc32 = a_pBuffer;

		if( m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) return -1;
    	if( m_pGZip->m_zErr == Z_STREAM_END || this->m_bEOF ) return 0;

		while( m_pGZip->m_zStream.avail_out != 0 ){

			if( m_pGZip->m_zStream.avail_in == 0 && !this->m_bEOF ){

				m_ifInput->read( reinterpret_cast<char*>(m_pGZip->m_buffer), m_pGZip->m_bufferSize );
				m_pGZip->m_zStream.avail_in = m_ifInput->gcount();

				if( m_pGZip->m_zStream.avail_in == 0 ){
					this->m_bEOF = true;
					break;
				}

				m_pGZip->m_zStream.next_in = m_pGZip->m_buffer;
			}

			m_pGZip->m_zErr = ::inflate( &m_pGZip->m_zStream, Z_NO_FLUSH );

			if( m_pGZip->m_zErr == Z_STREAM_END ){

				//Check CRC and original size
				m_pGZip->m_crc32 =
					::crc32(
							m_pGZip->m_crc32,
							l_pStartForCrc32,
							static_cast<uInt>(m_pGZip->m_zStream.next_out - l_pStartForCrc32) );
				l_pStartForCrc32 = m_pGZip->m_zStream.next_out;

				uLong crc32FromArq = static_cast<uLong>(this->getLong());
				if( crc32FromArq == m_pGZip->m_crc32 ){

					this->getLong();//descarta o tamanho do conteúdo descompactado.
					this->checkHeader();

					if( m_pGZip->m_zErr == Z_OK ){
						::inflateReset( &m_pGZip->m_zStream );
						m_pGZip->m_crc32 = ::crc32( 0L, Z_NULL, 0 );
					}

				} else {
					m_pGZip->m_zErr = Z_DATA_ERROR;
				}
			}
			if( m_pGZip->m_zErr != Z_OK || this->m_bEOF ) break;
		}

		m_pGZip->m_crc32 =
					::crc32(
							m_pGZip->m_crc32,
							l_pStartForCrc32,
							static_cast<uInt>(m_pGZip->m_zStream.next_out - l_pStartForCrc32) );

		if( a_len == (std::streamsize)m_pGZip->m_zStream.avail_out &&
				(m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) ){
			return -1;
		}

		return a_len - m_pGZip->m_zStream.avail_out;
	}
	std::streamsize GZipInputStream::readCustomGzCrcOff(
			unsigned char* a_pBuffer,
			std::streamsize a_len ){
		m_pGZip->m_zStream.next_out = a_pBuffer;
		m_pGZip->m_zStream.avail_out = a_len;

		if( m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) return -1;
    	if( m_pGZip->m_zErr == Z_STREAM_END || this->m_bEOF ) return 0;

		while( m_pGZip->m_zStream.avail_out != 0 ){

			if( m_pGZip->m_zStream.avail_in == 0 && !this->m_bEOF ){

				m_ifInput->read( reinterpret_cast<char*>(m_pGZip->m_buffer), m_pGZip->m_bufferSize );
				m_pGZip->m_zStream.avail_in = m_ifInput->gcount();

				if( m_pGZip->m_zStream.avail_in == 0 ){
					this->m_bEOF = true;
					break;
				}

				m_pGZip->m_zStream.next_in = m_pGZip->m_buffer;
			}

			m_pGZip->m_zErr = ::inflate( &m_pGZip->m_zStream, Z_NO_FLUSH );

			if( m_pGZip->m_zErr == Z_STREAM_END ){
				this->getLong();//descarta o crc32.

				this->getLong();//descarta o tamanho do conteúdo descompactado.
				this->checkHeader();

				if( m_pGZip->m_zErr == Z_OK ){
					::inflateReset( &m_pGZip->m_zStream );
				}
			}
			if( m_pGZip->m_zErr != Z_OK || this->m_bEOF ) break;
		}

		if( a_len == (std::streamsize)m_pGZip->m_zStream.avail_out &&
				(m_pGZip->m_zErr == Z_DATA_ERROR || m_pGZip->m_zErr == Z_ERRNO ) ){
			return -1;
		}

		return a_len - m_pGZip->m_zStream.avail_out;
	}

	void GZipInputStream::checkHeader(void)
	{
		int l_magic1 = this->getByte( );

		if( this->m_bEOF ){
			if( this->m_bFirstHeader ){
				m_pGZip->m_fileMode = GZSTREAM_MODE_TRANSPARENT;
			}
			return;
		}
		int l_magic2 = this->getByte( );

		if( this->m_bEOF ){

			if( this->m_bFirstHeader ){
				m_pGZip->m_zStream.avail_in++;
				m_pGZip->m_zStream.next_in--;

				m_pGZip->m_fileMode = GZSTREAM_MODE_TRANSPARENT;
			} else {
				m_pGZip->m_zErr = Z_DATA_ERROR;
			}
			return;
		}

	    // Peek ahead to check the gzip magic header
	    if( l_magic1 == m_gzMagicNumber[0] &&
	    		l_magic2 == m_gzMagicNumber[1] ){
	    	m_pGZip->m_fileMode = GZSTREAM_MODE_STANDARD_GZ;
		} else if( l_magic1 == m_gzCustomMagicNumberCRC_ON[0] &&
				l_magic2 == m_gzCustomMagicNumberCRC_ON[1] ){
			m_pGZip->m_fileMode = GZSTREAM_MODE_CUSTOM_GZ_CRC_ON;
		} else if( l_magic1 == m_gzCustomMagicNumberCRC_OFF[0] &&
				l_magic2 == m_gzCustomMagicNumberCRC_OFF[1] ){
			m_pGZip->m_fileMode = GZSTREAM_MODE_CUSTOM_GZ_CRC_OFF;
		} else {
			if( this->m_bFirstHeader ){
				m_pGZip->m_zStream.avail_in += 2;
				m_pGZip->m_zStream.next_in -= 2;
				m_pGZip->m_fileMode = GZSTREAM_MODE_TRANSPARENT;
				this->m_bFirstHeader = false;
			} else {
				m_pGZip->m_zErr = Z_DATA_ERROR;
				throw appUtil::JJJException(
						__FILE__,
						__LINE__,
						"Z_DATA_ERROR <%s>", m_file.fullName().c_str() );
			}
			return;
		}

		this->m_bFirstHeader = false;

		int l_method; // l_method byte
		int l_flags;  // l_flags byte
		uInt l_len;

		// Check the rest of the gzip header
		l_method = this->getByte( );
		l_flags = this->getByte( );
		if( l_method != Z_DEFLATED || (l_flags & RESERVED) != 0 ){
			m_pGZip->m_zErr = Z_DATA_ERROR;
			return;
		}

		// Discard time, xl_flags and OS code:
		for(int l_ixl = 0; l_ixl < 6; ++l_ixl ){
			this->getByte();
		}

		if( (l_flags & EXTRA_FIELD) != 0 ){		//skip the extra field
			l_len  = static_cast<uInt>( this->getByte( ) );
			l_len += static_cast<uInt>( this->getByte( ) ) << 8;

			for(uInt l_iFlg = 0; l_iFlg < l_len; ++l_iFlg ){
				this->getByte();
			}
		}
		if( (l_flags & ORIG_NAME) != 0 ){		//skip the original file name
			int c;
			while( ( c = getByte() ) != 0 && c != EOF );
		}
		if( (l_flags & COMMENT) != 0 ){			//skip the .gz file comment
			int c;
			while( ( c = getByte() ) != 0 && c != EOF );
		}
		if( (l_flags & HEAD_CRC) != 0 ){		//skip the header crc
			this->getByte();
			this->getByte();
		}

		m_pGZip->m_zErr = this->m_bEOF ? Z_DATA_ERROR : Z_OK;
	}

	int GZipInputStream::getByte(void)
	{
		if( this->m_bEOF ) return EOF;

		if( m_pGZip->m_zStream.avail_in == 0 ){

			m_ifInput->read(
					reinterpret_cast<char*>(m_pGZip->m_buffer),
					m_pGZip->m_bufferSize );
			m_pGZip->m_zStream.avail_in = m_ifInput->gcount();

			if( m_pGZip->m_zStream.avail_in == 0 ){
				this->m_bEOF = true;
				return EOF;
			}
			m_pGZip->m_zStream.next_in = m_pGZip->m_buffer;
		}

		m_pGZip->m_zStream.avail_in--;

		return *(m_pGZip->m_zStream.next_in)++;
	}

	unsigned long GZipInputStream::getLong(void)
	{
		uLong longValue = static_cast<uLong>(this->getByte());

		longValue += static_cast<uLong>(this->getByte()) << 8;
		longValue += static_cast<uLong>(this->getByte()) << 16;

		int msb = this->getByte();

		if( msb == EOF ){
			m_pGZip->m_zErr = Z_DATA_ERROR;
		}

		longValue += static_cast<unsigned long>(msb) << 24;

		return longValue;
	}

}//namespace fileUtil
