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

#include <zlib.h>
#include "fileUtil/FILE_MODE.h"

namespace fileUtil
{
	class GZipEnv
	{
	public:
		int m_compressionLevel;			//Nivel de compressao.
		int m_compressionStrategy;		//Estrategia de compressao.
		z_stream m_zStream;				//Descritor para controlar a descompress�o.
		Byte* m_buffer;					//Buffer onde os dados lidos do arquivo ser�o armazenados.
		std::streamsize m_bufferSize;	//Tamanho do espaco alocado para o buffer de entrada m_inputBuffer.
		int m_zErr;						//Codigo de erro da ultima operacao.
		enum FILE_MODE m_fileMode;		//File mode obtained in checkHeader method.
		uLong m_crc32;					//crc32 of uncompressed data.

	};
}
