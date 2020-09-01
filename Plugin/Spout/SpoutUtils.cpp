/*

			SpoutUtils

			Utility functions

	CREDIT - logging based on Openframeworks ofLog
	https://github.com/openframeworks/openFrameworks/tree/master/libs/openFrameworks/utils

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
	========================

		31.05.15 - started
		01.01.18 - added check for subkey
		17.03.18 - Document SetLogLevel function in header
		16.10.18 - Add SpoutLogNotice, Warning, Error, Fatal
		28.10.18 - Checks for lastlog and fatal messagebox moved from SpoutLog to _doLog
		15.11.18 - Removed delay after SpoutPanel open
		10.12.18 - Add Timeout option to SpoutMessageBox
		12.12.18 - Add SpoutLogFile
		13.12.18 - Add GetSpoutLog and ShowSpoutLogs
		14.12.18 - Clean up
		02.10.19 - Change registry functions including hKey
				   to allow HKLM and changed argument order
				 - Add RemoveSubKey and FindSubKey
		26.01.19 - Corrected Verbose log to show verbose and not notice
		07.04.19 - Add SpoutLog for logging without specifying level
		28.04.19 - Change OpenSpoutConsole() to check for existing console
		19.05.19 - Cleanup
		16.06.19 - Include calling process file name in SpoutMessageBox
		13.10.19 - Corrected EnableSpoutLogFile for a filename without an extension
				   Changed default extension from "txt" to "log"
		27.11.19 - Prevent multiple logs for warnings and errors
		22.12.19 - add pragma in header for registry function lbraries
		22.12.19 - Remove calling process name from SpoutMessageBox
		18.02.20 - Remove messagebox for Fatal errors
		19.05.20 - Add missing LPCSTR cast in SpoutMessageBox ShellExecute
		12.06.20 - Add timing functions for testing

*/
#include "SpoutUtils.h"

namespace spoututils {




	//
	// Registry utilities
	//

	//
	// New registry functions for 2.007 including hKey and changed argument order
	//
	bool ReadDwordFromRegistry(HKEY hKey, const char *subkey, const char *valuename, DWORD *pValue)
	{
		HKEY  hRegKey = NULL;
		LONG  regres = 0;
		DWORD dwKey = 0;
		DWORD dwSize = MAX_PATH;

		// 01.01.18
		if (!subkey[0])	return false;

		// Does the key exist
		regres = RegOpenKeyExA(hKey, subkey, NULL, KEY_READ, &hRegKey);
		if (regres == ERROR_SUCCESS) {
			// Read the key DWORD value
			regres = RegQueryValueExA(hRegKey, valuename, NULL, &dwKey, (BYTE*)pValue, &dwSize);
			RegCloseKey(hRegKey);
			if (regres == ERROR_SUCCESS)
				return true;
		}

		// Just quit if the key does not exist
		return false;

	}

	bool WriteDwordToRegistry(HKEY hKey, const char *subkey, const char *valuename, DWORD dwValue)
	{
		HKEY  hRegKey = NULL;
		LONG  regres = 0;
		char  mySubKey[512];

		if (!subkey[0])
			return false;

		// The required key
		strcpy_s(mySubKey, 512, subkey);

		// Does the key already exist ?
		regres = RegOpenKeyExA(hKey, mySubKey, NULL, KEY_ALL_ACCESS, &hRegKey);
		if (regres != ERROR_SUCCESS) {
			// Create a new key
			regres = RegCreateKeyExA(hKey, mySubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
		}

		if (regres == ERROR_SUCCESS && hRegKey != NULL) {
			// Write the DWORD value
			regres = RegSetValueExA(hRegKey, valuename, 0, REG_DWORD, (BYTE*)&dwValue, 4);
			// For immediate read after write - necessary here because the app might set the values 
			// and read the registry straight away and it might not be available yet
			// The key must have been opened with the KEY_QUERY_VALUE access right 
			// (included in KEY_ALL_ACCESS)
			RegFlushKey(hRegKey); // needs an open key
			RegCloseKey(hRegKey); // Done with the key
		}

		if (regres != ERROR_SUCCESS) {
			printf("WriteDwordToRegistry - could not write to registry");
			return false;
		}

		return true;

	}

	bool ReadPathFromRegistry(HKEY hKey, const char *subkey, const char *valuename, char *filepath)
	{
		HKEY  hRegKey = NULL;
		LONG  regres = 0;
		DWORD dwKey = 0;
		DWORD dwSize = MAX_PATH;

		if (!subkey[0])
			return false;

		// Does the key exist
		regres = RegOpenKeyExA(hKey, subkey, NULL, KEY_READ, &hRegKey);
		if (regres == ERROR_SUCCESS) {
			// Read the key Filepath value
			regres = RegQueryValueExA(hRegKey, valuename, NULL, &dwKey, (BYTE*)filepath, &dwSize);
			RegCloseKey(hRegKey);
			if (regres == ERROR_SUCCESS)
				return true;
		}
		// Quit if the key does not exist
		printf("ReadPathFromRegistry - could not open subkey [%s] Error (%ld)", subkey, regres);

		return false;
	}
	
	bool WritePathToRegistry(HKEY hKey, const char *subkey, const char *valuename, const char *filepath)
	{
		HKEY  hRegKey = NULL;
		LONG  regres = 0;
		char  mySubKey[512];

		if (!subkey[0]) {
			printf("WritePathToRegistry - no subkey specified");
			return false;
		}

		// The required key
		strcpy_s(mySubKey, 512, subkey);

		// Does the key already exist ?
		regres = RegOpenKeyExA(hKey, mySubKey, NULL, KEY_ALL_ACCESS, &hRegKey);
		if (regres != ERROR_SUCCESS) {
			// Create a new key
			regres = RegCreateKeyExA(hKey, mySubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
		}

		if (regres == ERROR_SUCCESS && hRegKey != NULL) {
			// Write the path
			regres = RegSetValueExA(hRegKey, valuename, 0, REG_SZ, (BYTE*)filepath, ((DWORD)strlen(filepath) + 1) * sizeof(unsigned char));
			RegCloseKey(hRegKey);
		}

		if (regres != ERROR_SUCCESS) {
			printf("WritePathToRegistry - could not write to registry");
			return false;
		}

		return true;

	}

	bool RemovePathFromRegistry(HKEY hKey, const char *subkey, const char *valuename)
	{
		HKEY  hRegKey = NULL;
		LONG  regres = 0;

		// 01.01.18
		if (!subkey[0]) {
			printf("RemovePathFromRegistry - no subkey specified");
			return false;
		}

		regres = RegOpenKeyExA(hKey, subkey, NULL, KEY_ALL_ACCESS, &hRegKey);
		if (regres == ERROR_SUCCESS) {
			regres = RegDeleteValueA(hRegKey, valuename);
			RegCloseKey(hRegKey);
			return true;
		}

		// Quit if the key does not exist
		printf("RemovePathFromRegistry - could not open key [%s]", subkey);
		return false;
	}

	// Delete a subkey and its values.
	// Note that key names are not case sensitive.
	// It must be a subkey of the key that hKey identifies, but it cannot have subkeys.
	bool RemoveSubKey(HKEY hKey, const char *subkey)
	{
		LONG lStatus;

		lStatus = RegDeleteKeyA(hKey, subkey);
		if (lStatus == ERROR_SUCCESS)
			return true;

		printf("RemoveSubkey - error #%ld", lStatus);
		return false;
	}

	bool FindSubKey(HKEY hKey, const char *subkey)
	{
		HKEY hRegKey;
		LONG lStatus = RegOpenKeyExA(hKey, subkey, NULL, KEY_READ, &hRegKey);
		if(lStatus == ERROR_SUCCESS) {
			RegCloseKey(hRegKey);
			return true;
		}

		printf("FindSubkey - error #%ld", lStatus);
		return false;

	}

	//
	// Local functions
	//


	// Get the name for the current log level




} // end namespace spoututils
