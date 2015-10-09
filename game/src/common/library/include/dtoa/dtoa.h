#ifndef _DTOA_H_
#define _DTOA_H_

#ifdef __cplusplus
extern "C" {
#endif

extern double _strtod(const char *s00, char **se);
extern char* _dtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve);
extern char* _dtoa_radixed(double dval, int radix);

#ifdef __cplusplus
}
#endif

#endif	// _DTOA_H_