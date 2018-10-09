#ifndef C2ENGINE_PREDEFINED_H_
#define C2ENGINE_PREDEFINED_H_
////////////////////////////////////////////////////////////////////////////////

#define C2Interface

#ifdef _MSC_VER
typedef __int8 Sint8;
typedef unsigned __int8 Uint8;
typedef __int16 Sint16;
typedef unsigned __int16 Uint16;
typedef __int32 Sint32;
typedef unsigned __int32 Uint32;
typedef __int64 Sint64;
typedef unsigned __int64 Uint64;
#else
typedef signed char Sint8;
typedef unsigned char Uint8;
typedef signed short Sint16;
typedef unsigned short Uint16;
typedef signed int Sint32;
typedef unsigned int Uint32;
typedef signed long long Sint64;
typedef unsigned long long Uint64;
#endif

////////////////////////////////////////////////////////////////////////////////
#endif// C2ENGINE_PREDEFINED_H_
