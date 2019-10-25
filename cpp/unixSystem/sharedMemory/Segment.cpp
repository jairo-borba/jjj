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
#include "unixSystem/sharedMemory/Segment.h"

namespace unixSystem
{
	namespace sharedMemory
	{
		Segment::Segment(void)
		{
			m_segmentSize = 0;
			m_address = NULL;
		}
		Segment::~Segment(void)
		{
		}
		void Segment::setSegmentSize(
				size_t a_segmentSize )
		{
			m_segmentSize = a_segmentSize;
		}
		void Segment::setAddress(
				void* a_address )
		{
			m_address = (char*)a_address;
		}
		void* Segment::address(void)
		{
			return m_address;
		}
		void Segment::clear(void)
		{
			memset( m_address, 0, m_segmentSize );
		}
		bool Segment::write(
				size_t a_position,
				const void* a_data,
				size_t a_dataSize )
		{
			if( a_position + a_dataSize > m_segmentSize ){
				return false;
			}
			memcpy( &m_address[a_position], a_data, a_dataSize );
			return true;
		}
		bool Segment::read(
				size_t a_position,
				void* a_buffer,
				size_t a_sizeToCopy )
		{
			if( a_position + a_sizeToCopy > m_segmentSize ){
				return false;
			}
			memcpy( a_buffer, &m_address[a_position], a_sizeToCopy );

			return true;
		}
	}//namespace sharedMemory
}//namespace unixSystem
