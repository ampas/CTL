
#pragma once

#ifdef _WIN32

#ifdef DPX_DLL
#  define DPX_EXPORT __declspec(dllimport)
#else
#  define DPX_EXPORT __declspec(dllexport)
#endif

#else
#define DPX_EXPORT
#endif
