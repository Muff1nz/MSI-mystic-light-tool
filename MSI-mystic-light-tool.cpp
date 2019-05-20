/*
MSI-Mystic-light-tool
Copyright(C) 2017 Simone Dassi

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

//reference for the Mystic Light lib: https://www.msi.com/files/pdf/Mystic_Light_Software_Development_Kit.pdf


#include "stdafx.h"
#include <atlsafe.h>
#include "MysticLight_SDK.h"
#include <string>
#include <Windows.h>

HINSTANCE MLinstance;

struct MysticLight {		//struct grouping all the function pointers under one pseudo-object, ml, for better clarity
	LPMLAPI_Initialize init;
	LPMLAPI_GetLedColor getColor;		//coloUr is better tbh..
	LPMLAPI_SetLedColor setColor;
	LPMLAPI_SetLedBright setBright;
} ml;

int loadLib() {

	MLinstance = LoadLibrary(TEXT("MysticLight_SDK.dll"));
	if (!MLinstance)
		return 0;

	//initialise the struct fields
	ml.init = (LPMLAPI_Initialize)GetProcAddress(MLinstance, "MLAPI_Initialize");
	ml.getColor = (LPMLAPI_GetLedColor)GetProcAddress(MLinstance, "MLAPI_GetLedColor");
	ml.setColor = (LPMLAPI_SetLedColor)GetProcAddress(MLinstance, "MLAPI_SetLedColor");
	ml.setBright = (LPMLAPI_SetLedBright)GetProcAddress(MLinstance, "MLAPI_SetLedBright");

	return 1;
}

//Just made this specific to my build
BSTR mobo = L"MSI_MB";

struct Color {
	DWORD r;
	DWORD b;
	DWORD g;

	bool operator==(Color other) const {
		return r == other.r && b == other.b && g == other.g;
	}

	Color operator*(float f) {
		return {static_cast<DWORD>(r*f), static_cast<DWORD>(g*f), static_cast<DWORD>(b*f) };
	}
};

Color black = { 0, 0, 0 };
Color white = { 100, 100, 100 };


void SetLed(BSTR device, DWORD index, Color clr, DWORD bright) {
	ml.setColor(device, index, clr.r, clr.g, clr.b);
	ml.setBright(device, index, bright);
}

void ToggleLed(BSTR device, DWORD index) {
	Color clr;
	ml.getColor(device, index, &clr.r, &clr.g, &clr.b);
	if (clr == black) {
		SetLed(device, index, white, 1);
	} else {
		SetLed(device, index, black, 1);
	}
}

int main(int argc, char** argv) {
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	if (loadLib()) {
		if (ml.init() == 0) {		//initialisation successful
			ToggleLed(mobo, 1);
			ToggleLed(mobo, 2);
		}

		FreeLibrary(MLinstance);
	} 
	return 0;
}

