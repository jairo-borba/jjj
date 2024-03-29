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

#include "base/Identificable.h"
#include "base/Clonable.h"

namespace logging
{
	class LogHandler : public base::Identificable, public base::Clonable< LogHandler >
	{
	public:

		LogHandler(void);
		virtual ~LogHandler(void);

		virtual bool open(void) = 0;
		virtual void close(void) = 0;
		virtual bool write(
				const char* a_logMessage,
				size_t a_msgSize) = 0;

		void setOutputName(
				const std::string& a_outputName );
		const std::string& getOutputName(void) const;
		void setOutputPath(
				const std::string& a_outputPath );
		const std::string& getOutputPath(void) const;

	private:
		std::string m_outputName;
		std::string m_outputPath;
	};
}//namespace logging
