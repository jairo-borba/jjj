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
#include <vector>
#include <list>

namespace fileUtil
{
	class File
	{
	public:
		File(
				const std::string& a_fullName );
		File(
				const std::string& a_path,
				const std::string& a_name );
		File(
				const std::string& a_path,
				const std::string& a_name,
				const std::string& a_group,
				const std::string& a_subgroup = "none" );

		File(void);
		virtual ~File(void);
		File* clone(void) const;

		const std::string& path(void) const;
		const std::string& name(void) const;
		std::string fullName(void) const;
		const std::string& group(void) const;
		const std::string& subgroup(void) const;
		const std::string& type(void) const;
		size_t fileSize(void) const;

		bool exists(void) const;

		File& setFullName(
				const std::string& a_fullName );
		File& setPath(
				const std::string& a_path );
		File& setName(
				const std::string& a_name );
		File& setGroup(
				const std::string& a_group );
		File& setSubgroup(
				const std::string& a_subgroup );
		File& setType(
				const std::string& a_type );

		static size_t fullFileSize(
				const std::vector<File>& a_vectFiles );

		static size_t fullFileSize(
						const std::list<File>& a_listFiles );

	private:

		std::string m_path;
		std::string m_name;
		std::string m_group;
		std::string m_subgroup;
		std::string m_type;
		size_t m_fileSize;
	};


}//namespace fileUtil

