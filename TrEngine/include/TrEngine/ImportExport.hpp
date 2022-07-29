#pragma once

#ifdef TRENGINE_LIB
	#define TRENGINE_EXPORT __declspec(dllexport)
#else
	#define TRENGINE_EXPORT __declspec(dllimport)

	#ifdef _WIN64
		#pragma comment(lib,"TrEnginex64.lib")
	#else
		#pragma comment(lib,"TrEnginex32.lib")
	#endif
#endif

// For Visual C++ compilers, we also need to turn off this annoying C4251 warning
#ifdef _MSC_VER
//#pragma warning(disable: 4251)
#endif