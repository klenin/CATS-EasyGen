#ifndef __LANGUAGE_STANDARD_H__
#define __LANGUAGE_STANDARD_H__

#if defined(__STDC__)
    #define C89
    #if defined(__STDC_VERSION__)
        #define C90
        #if (__STDC_VERSION__ >= 199409L)
            #define C94
        #endif
        #if (__STDC_VERSION__ >= 199901L)
            #define C99
        #endif
    #endif
#endif

#if defined(_MSC_VER)
#define MSVS_2003 1310
#define MSVS_2005 1400
#define MSVS_2008 1500
#define MSVS_2010 1600
#define MSVS_2012 1700
#endif

#endif // __LANGUAGE_STANDARD_H__
