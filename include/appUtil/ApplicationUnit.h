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

#include <exception>
#include <string>
#include <list>
#include <set>
#include <map>
#include <sstream>
#include "appUtil/JJJException.h"

namespace appUtil
{
	class ApplicationUnit
	{
	public:
		
		static void assert(
				const ApplicationUnit* a_pAppUnit );
		static ApplicationUnit* findUnit(
				const std::string& a_appName );
	
		typedef std::list<std::string> TP_ARGS;
	
		ApplicationUnit(
				const std::string& a_name,
				const std::string& a_version );
		virtual ~ApplicationUnit(void);

		virtual int start(void);
		virtual int run(void) = 0;
		virtual int end(
				int a_runReturnCode );
		virtual int end(
				const appUtil::JJJException& a_e );
		
		const TP_ARGS& args(void) const;
		
		static ApplicationUnit* theApp(void);
		
		void addArg(
				const std::string& a_arg );
		
		const std::string& name(void) const;
		const std::string& version(void) const;
		
	private:
		TP_ARGS m_args;
		std::string m_name;
		std::string m_version;
		
		typedef std::set<const ApplicationUnit*> APPLICATION_SET_TYPE;
		typedef std::map<std::string, ApplicationUnit*> APPLICATION_MAP_TYPE;
		typedef std::pair<std::string, ApplicationUnit*> APPLICATION_MAP_PAIR;
		
		static APPLICATION_SET_TYPE m_setAppUnits;
		static APPLICATION_MAP_TYPE m_mapApps;
	};
}
