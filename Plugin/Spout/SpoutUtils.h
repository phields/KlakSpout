/*

						SpoutUtils.h

					General utility functions

		- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		Copyright (c) 2017-2020, Lynn Jarvis. All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, 
		are permitted provided that the following conditions are met:

		1. Redistributions of source code must retain the above copyright notice, 
		   this list of conditions and the following disclaimer.

		2. Redistributions in binary form must reproduce the above copyright notice, 
		   this list of conditions and the following disclaimer in the documentation 
		   and/or other materials provided with the distribution.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"	AND ANY 
		EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
		OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE	ARE DISCLAIMED. 
		IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
		INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
		PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
		INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
		OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#pragma once
#ifndef __spoutUtils__ // standard way as well
#define __spoutUtils__

#include <windows.h>
#include <stdio.h> // for console
#include <iostream> // std::cout, std::end
#include <fstream> // for log file
#include <io.h> // for _access
#include <vector>
#include <string>

#pragma comment(lib, "Advapi32.lib") // for registry functions

namespace spoututils {

	// Log level definitions


	// Registry utility functions
 	bool ReadDwordFromRegistry(HKEY hKey, const char *subkey, const char *valuename, DWORD *pValue);
	bool WriteDwordToRegistry(HKEY hKey, const char *subkey, const char *valuename, DWORD dwValue);
	bool ReadPathFromRegistry(HKEY hKey, const char *subkey, const char *valuename, char *filepath);
	bool WritePathToRegistry(HKEY hKey, const char *subkey, const char *valuename, const char *filepath);
	bool RemovePathFromRegistry(HKEY hKey, const char *subkey, const char *valuename);
	bool RemoveSubKey(HKEY hKey, const char *subkey);
	bool FindSubKey(HKEY hKey, const char *subkey);



}

#endif
