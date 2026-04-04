#pragma once

#define UNUSED(p) (void)p

#define HEAPALLOC(s) calloc(1, s);

#if defined(_MSC_VER)

#define STACKALLOC(s) _alloca(s)

#define UNREACHABLE() __assume(0)

#else

#define STACKALLOC(s) __builtin_alloca(s)

#define UNREACHABLE() __builtin_unreachable()

#endif