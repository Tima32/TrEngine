#pragma once
#ifdef TRENGINE_LIB
#define TRENGINE_EXPORT __declspec(dllexport)
#else
#define TRENGINE_EXPORT __declspec(dllimport)
#pragma comment(lib,"TrEngine.lib")
#endif

// For Visual C++ compilers, we also need to turn off this annoying C4251 warning
#ifdef _MSC_VER
//#pragma warning(disable: 4251)
#endif