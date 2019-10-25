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
#include "fileUtil/File.h"
#include "appUtil/JJJException.h"
#include "appUtil/SafeStringDef.h"
#include <cstdio>

namespace fileUtil
{
	File::File(
			const std::string& a_fullName )
		:
				m_group( "none" ),
				m_subgroup( "none" ),
				m_fileSize( 0 )
	{
		this->setFullName( a_fullName );
	}
	File::File(
			const std::string& a_path,
			const std::string& a_name )
		:
				m_path( a_path ),
				m_name( a_name ),
				m_group( "none" ),
				m_subgroup( "none" ),
				m_fileSize( 0 )
	{
	}
	File::File(
			const std::string& a_path,
			const std::string& a_name,
			const std::string& a_group,
			const std::string& a_subgroup )
		:
				m_path( a_path ),
				m_name( a_name ),
				m_group( a_group ),
				m_subgroup( a_subgroup ),
				m_fileSize( 0 )
	{
	}

	File* File::clone(void) const
	{
		File* l_pNewFile = new File(*this);

		return l_pNewFile;
	}

	File::File(void) : m_fileSize( 0 )
	{

	}

	File::~File(void)
	{
	}

	File& File::setPath(
			const std::string& a_path )
	{
		this->m_path = a_path;
		m_fileSize = 0;
		return *this;
	}

	File& File::setName(
			const std::string& a_name )
	{
		this->m_name = a_name;
		m_fileSize = 0;
		return *this;
	}

	File& File::setGroup(
			const std::string& a_group )
	{
		this->m_group = a_group;
		return *this;
	}
	File& File::setSubgroup(
			const std::string& a_subgroup )
	{
		this->m_subgroup = a_subgroup;
		return *this;
	}
	File& File::setType(
			const std::string& a_type )
	{
		m_type = a_type;
		return *this;
	}
	File& File::setFullName(
			const std::string& a_fullName )
	{
		std::string l_fullName( a_fullName );
		m_fileSize = 0;

		for( size_t i = 0; i < l_fullName.length(); ++i ){
			if( l_fullName[i] == '\\' ){
				l_fullName[i] = '/';
			}
		}

		size_t l_endPathPos = l_fullName.find_last_of( '/' );

		if( l_endPathPos == std::string::npos ){
			this->setPath( "." );
			this->setName( l_fullName );
		} else {
			this->setPath( l_fullName.substr( 0, l_endPathPos ) );
			this->setName( l_fullName.substr( l_endPathPos + 1 ) );
		}

		return *this;
	}

	const std::string& File::group(void) const
	{
		return this->m_group;
	}

	const std::string& File::path(void) const
	{
		return this->m_path;
	}

	const std::string& File::subgroup(void) const
	{
		return this->m_subgroup;
	}

	const std::string& File::name() const
	{
		return this->m_name;
	}
	const std::string& File::type() const
	{
		return m_type;
	}

	std::string File::fullName( ) const
	{
		std::string l_fullName = this->m_path + "/" + this->m_name;
		return l_fullName;
	}

	bool File::exists(void) const
	{
		FILE* l_fp = fopen( this->fullName().c_str(), "r" );

		if( l_fp != NULL ){
			return true;
			fclose( l_fp );
		}

		return false;
	}

	size_t File::fileSize( ) const
	{
		if( m_fileSize > 0 ){
			return m_fileSize;
		}

		FILE* l_fp = fopen( this->fullName().c_str(), "r" );

		if( l_fp == NULL ){
			return 0;
		}
		
		int seekRet = fseek( l_fp, 0, SEEK_END );
		if( seekRet != 0 ){
			fclose( l_fp );
			return 0;
		}

		size_t l_size = ::ftell( l_fp );

		fclose( l_fp );

		return l_size;
	}

	size_t File::fullFileSize(
			const std::vector<File>& a_vectFiles )
	{
		size_t l_fullSize = 0;
		std::vector<File>::const_iterator it;

		for( it = a_vectFiles.begin(); it != a_vectFiles.end(); ++it )
		{
			l_fullSize += it->fileSize();
		}

		return l_fullSize;
	}

	size_t File::fullFileSize(
				const std::list<File>& a_listFiles )
		{
			size_t l_fullSize = 0;
			std::list<File>::const_iterator it;

			for( it = a_listFiles.begin(); it != a_listFiles.end(); ++it )
			{
				l_fullSize += it->fileSize();
			}

			return l_fullSize;
		}
}//namespace fileUtil

