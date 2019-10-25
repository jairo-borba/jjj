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
#include "fileUtil/GZipStream.h"
#include <cstring>
#include "fileUtil/GZipEnv.h"
#include "fileUtil/FILE_MODE.h"

namespace fileUtil
{
	const int GZipStream::m_gzMagicNumber[]					= { 0x1f, 0x8b };
	const int GZipStream::m_gzCustomMagicNumberCRC_ON[]		= { 0x1e, 0x76 };
	const int GZipStream::m_gzCustomMagicNumberCRC_OFF[]	= { 0x1e, 0x6c };
	const int GZipStream::DEFAULT_BUFFER_SIZE				= 1048576;// 1MB - 4194304;	//4 Mb
	const int GZipStream::MIN_BUFFER_SIZE					= 65536;	//64 Kb
	
	GZipStream::GZipStream(void)
	{
		m_pGZip = new fileUtil::GZipEnv;
		m_pGZip->m_compressionLevel		= Z_DEFAULT_COMPRESSION;
		m_pGZip->m_compressionStrategy	= Z_DEFAULT_STRATEGY;
		m_pGZip->m_buffer				= NULL;
		m_pGZip->m_bufferSize			= DEFAULT_BUFFER_SIZE;
		m_pGZip->m_zErr					= Z_OK;
		m_pGZip->m_fileMode				= GZSTREAM_MODE_CLOSED;
		m_pGZip->m_crc32				= 0;
		memset( &m_pGZip->m_zStream, 0, sizeof m_pGZip->m_zStream );
	}
	
	GZipStream::~GZipStream(void)
	{
		delete m_pGZip;
	}
	
	void GZipStream::setFile(
			fileUtil::File a_file )
	{
		m_file = a_file;
	}
	
}//namespace fileUtil
