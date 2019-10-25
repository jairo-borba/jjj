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
#include "unixSystem/signals/ChildProcessInformation.h"

namespace unixSystem
{
	namespace signals
	{
		ChildProcessInformation::ChildProcessInformation(void)
		{
			m_processID = 0;
			m_exitStatus = 0;
			m_computed = true;
		}
		ChildProcessInformation::~ChildProcessInformation(void)
		{
		}
		void ChildProcessInformation::setProcessID(
				pid_t a_pid )
		{
			m_processID = a_pid;
		}
		void ChildProcessInformation::setExitStatus(
				int a_exitStatus )
		{
			m_exitStatus = a_exitStatus;
		}
		void ChildProcessInformation::setComputed(
				bool a_computed)
		{
			m_computed = a_computed;
		}
		pid_t ChildProcessInformation::processID(void) const
		{
			return m_processID;
		}
		int ChildProcessInformation::exitStatus(void) const
		{
			return m_exitStatus;
		}
		bool ChildProcessInformation::computed(void) const
		{
			return m_computed;
		}
	}//namespace signals
}//namespace unixSystem

