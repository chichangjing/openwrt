/*
 * "$Id: mxml-string.c 387 2009-04-18 17:05:52Z mike $"
 *
 * String functions for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2003-2009 by Michael Sweet.
 *
 * 2011 - Adopted to Marvell CPSS LUA CLI - added _mxml_ prefix for all ANSI functions
 *        Added implementation for part of _mxml_ functions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Contents:
 *
 *   _mxml_snprintf()  - Format a string.
 *   _mxml_strdup()    - Duplicate a string.
 *   _mxml_strdupf()   - Format and duplicate a string.
 *   _mxml_vsnprintf() - Format a string into a fixed size buffer.
 *   _mxml_vstrdupf()  - Format and duplicate a string.
 */

/*
 * Include necessary headers...
 */

#include "mxml_config.h"


#ifndef HAVE_SNPRINTF
/*
 * '_mxml_snprintf()' - Format a string.
 */

int					/* O - Number of bytes formatted */
_mxml_snprintf(char       *buffer,	/* I - Output buffer */
               size_t     bufsize,	/* I - Size of output buffer */
	       const char *format,	/* I - Printf-style format string */
	       ...)			/* I - Additional arguments as needed */
{
  va_list	ap;			/* Argument list */
  int		bytes;			/* Number of bytes formatted */


  va_start(ap, format);
  bytes = _mxml_vsnprintf(buffer, bufsize, format, ap);
  va_end(ap);

  return (bytes);
}
#endif /* !HAVE_SNPRINTF */


/*
 * '_mxml_strdup()' - Duplicate a string.
 */

#ifndef HAVE_STRDUP
char 	*				/* O - New string pointer */
_mxml_strdup(const char *s)		/* I - String to duplicate */
{
  char	*t;				/* New string pointer */


  if (s == NULL)
    return (NULL);

  if ((t = _mxml_malloc(_mxml_strlen(s) + 1)) == NULL)
    return (NULL);

  return (_mxml_strcpy(t, s));
}
#endif /* !HAVE_STRDUP */


/*
 * '_mxml_strdupf()' - Format and duplicate a string.
 */

char *					/* O - New string pointer */
_mxml_strdupf(const char *format,	/* I - Printf-style format string */
              ...)			/* I - Additional arguments as needed */
{
  va_list	ap;			/* Pointer to additional arguments */
  char		*s;			/* Pointer to formatted string */


 /*
  * Get a pointer to the additional arguments, format the string,
  * and return it...
  */

  va_start(ap, format);
  s = _mxml_vstrdupf(format, ap);
  va_end(ap);

  return (s);
}


#ifndef HAVE_VSNPRINTF
/*
 * '_mxml_vsnprintf()' - Format a string into a fixed size buffer.
 */

int					/* O - Number of bytes formatted */
_mxml_vsnprintf(char       *buffer,	/* O - Output buffer */
                size_t     bufsize,	/* O - Size of output buffer */
		const char *format,	/* I - Printf-style format string */
 		va_list    ap)		/* I - Pointer to additional arguments */
{
  char		*bufptr,		/* Pointer to position in buffer */
		*bufend,		/* Pointer to end of buffer */
		sign,			/* Sign of format width */
		size,			/* Size character (h, l, L) */
		type;			/* Format type character */
  int		width,			/* Width of field */
		prec;			/* Number of characters of precision */
  char		tformat[100],		/* Temporary format string for sprintf() */
		*tptr,			/* Pointer into temporary format */
		temp[1024];		/* Buffer for formatted numbers */
  char		*s;			/* Pointer to string */
  int		slen;			/* Length of string */
  int		bytes;			/* Total number of bytes needed */


 /*
  * Loop through the format string, formatting as needed...
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;
  bytes  = 0;
  size = 'l';

  while (*format)
  {
    if (*format == '%')
    {
      tptr = tformat;
      *tptr++ = *format++;

      if (*format == '%')
      {
        if (bufptr && bufptr < bufend) *bufptr++ = *format;
        bytes ++;
        format ++;
	continue;
      }
      else if (_mxml_strchr(" -+#\'", *format))
      {
        *tptr++ = *format;
        sign = *format++;
      }
      else
        sign = 0;

      if (*format == '*')
      {
       /*
        * Get width from argument...
	*/

	format ++;
	width = va_arg(ap, int);

	_mxml_snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", width);
	tptr += _mxml_strlen(tptr);
      }
      else
      {
	width = 0;

	while (*format >= '0' && *format <= '9')
	{
	  if (tptr < (tformat + sizeof(tformat) - 1))
	    *tptr++ = *format;

	  width = width * 10 + *format++ - '0';
	}
      }

      if (*format == '.')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        format ++;

        if (*format == '*')
	{
         /*
	  * Get precision from argument...
	  */

	  format ++;
	  prec = va_arg(ap, int);

	  _mxml_snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", prec);
	  tptr += _mxml_strlen(tptr);
	}
	else
	{
	  prec = 0;

	  while (*format >= '0' && *format <= '9')
	  {
	    if (tptr < (tformat + sizeof(tformat) - 1))
	      *tptr++ = *format;

	    prec = prec * 10 + *format++ - '0';
	  }
	}
      }
      else
        prec = -1;

      if (*format == 'l' && format[1] == 'l')
      {
        size = 'L';

	if (tptr < (tformat + sizeof(tformat) - 2))
	{
	  *tptr++ = 'l';
	  *tptr++ = 'l';
	}

	format += 2;
      }
      else if (*format == 'h' || *format == 'l' || *format == 'L')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        size = *format++;
      }

      if (!*format)
        break;

      if (tptr < (tformat + sizeof(tformat) - 1))
        *tptr++ = *format;

      type  = *format++;
      *tptr = '\0';

      switch (type)
      {
#if 0
	case 'E' : /* Floating point formats */
	case 'G' :
	case 'e' :
	case 'f' :
	case 'g' :
	    if ((width + 2) > sizeof(temp))
	      break;

	    sprintf(temp, tformat, va_arg(ap, double));

            bytes += _mxml_strlen(temp);

            if (bufptr)
	    {
	      if ((bufptr + _mxml_strlen(temp)) > bufend)
	      {
		_mxml_strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		_mxml_strcpy(bufptr, temp);
		bufptr += _mxml_strlen(temp);
	      }
	    }
	    break;
#endif

        case 'B' : /* Integer formats */
	case 'X' :
	case 'b' :
        case 'd' :
	case 'i' :
	case 'o' :
	case 'u' :
	case 'x' :
	    if ((width + 2) > sizeof(temp))
	      break;

	    if (size == 'L')
        {
    #ifdef HAVE_LONG_LONG
              _mxml_sprintf(temp, tformat, va_arg(ap, long long));
    #endif /* HAVE_LONG_LONG */
        }
	    else
          _mxml_sprintf(temp, tformat, va_arg(ap, int));

            bytes += _mxml_strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + _mxml_strlen(temp)) > bufend)
	      {
		_mxml_strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		_mxml_strcpy(bufptr, temp);
		bufptr += _mxml_strlen(temp);
	      }
	    }
	    break;

	case 'p' : /* Pointer value */
	    if ((width + 2) > sizeof(temp))
	      break;

	    _mxml_sprintf(temp, tformat, va_arg(ap, void *));

            bytes += _mxml_strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + _mxml_strlen(temp)) > bufend)
	      {
		_mxml_strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		_mxml_strcpy(bufptr, temp);
		bufptr += _mxml_strlen(temp);
	      }
	    }
	    break;

        case 'c' : /* Character or character array */
	    bytes += width;

	    if (bufptr)
	    {
	      if (width <= 1)
	        *bufptr++ = va_arg(ap, int);
	      else
	      {
		if ((bufptr + width) > bufend)
		  width = bufend - bufptr;

		_mxml_memcpy(bufptr, va_arg(ap, char *), (size_t)width);
		bufptr += width;
	      }
	    }
	    break;

	case 's' : /* String */
	    if ((s = va_arg(ap, char *)) == NULL)
	      s = "(null)";

	    slen = _mxml_strlen(s);
	    if (slen > width && prec != width)
	      width = slen;

            bytes += width;

	    if (bufptr)
	    {
	      if ((bufptr + width) > bufend)
	        width = bufend - bufptr;

              if (slen > width)
	        slen = width;

	      if (sign == '-')
	      {
		_mxml_strncpy(bufptr, s, (size_t)slen);
		_mxml_memset(bufptr + slen, ' ', (size_t)(width - slen));
	      }
	      else
	      {
		_mxml_memset(bufptr, ' ', (size_t)(width - slen));
		_mxml_strncpy(bufptr + width - slen, s, (size_t)slen);
	      }

	      bufptr += width;
	    }
	    break;

	case 'n' : /* Output number of chars so far */
	    *(va_arg(ap, int *)) = bytes;
	    break;
      }
    }
    else
    {
      bytes ++;

      if (bufptr && bufptr < bufend)
        *bufptr++ = *format;

      format ++;
    }
  }

 /*
  * Nul-terminate the string and return the number of characters needed.
  */

  *bufptr = '\0';

  return (bytes);
}
#endif /* !HAVE_VSNPRINTF */


/*
 * '_mxml_vstrdupf()' - Format and duplicate a string.
 */

char *					/* O - New string pointer */
_mxml_vstrdupf(const char *format,	/* I - Printf-style format string */
               va_list    ap)		/* I - Pointer to additional arguments */
{
  int	bytes;				/* Number of bytes required */
  char	*buffer,			/* String buffer */
	temp[256];			/* Small buffer for first vsnprintf */


 /*
  * First format with a tiny buffer; this will tell us how many bytes are
  * needed...
  */

  bytes = _mxml_vsnprintf(temp, sizeof(temp), format, ap);

  if (bytes < sizeof(temp))
  {
   /*
    * Hey, the formatted string fits in the tiny buffer, so just dup that...
    */

    return (strdup(temp));
  }

 /*
  * Allocate memory for the whole thing and reformat to the new, larger
  * buffer...
  */

  if ((buffer = _mxml_calloc(1, bytes + 1)) != NULL)
    _mxml_vsnprintf(buffer, bytes + 1, format, ap);

 /*
  * Return the new string...
  */

  return (buffer);
}

void* _mxml_memmove(void* dest, void *src, mxml_size_t n)
{
    if (dest == NULL || src == NULL)
        return dest;
    if (dest == src)
        return dest;
    if ((GT_UINTPTR)dest < (GT_UINTPTR)src)
        return _mxml_memcpy(dest,src,n);
    if ((GT_UINTPTR)dest > (GT_UINTPTR)src + n)
        return _mxml_memcpy(dest,src,n);
    for (;n;n--)
        ((char*)dest)[n-1] = ((char*)src)[n-1];
    return dest;
}
char* _mxml_strncpy(char* dest, const char *src, mxml_size_t n)
{
    char *ret = dest;
    if (dest == NULL || src == NULL)
        return ret;
    for (;*src && n; n--)
        *dest++ = *src++;
    if (n)
        *dest = 0;
    return ret;
}
int _mxml_strncmp(const char* s1, const char *s2, mxml_size_t n)
{
    if (s1 == NULL || s2 == NULL)
        return 0;
    for (;*s1 && *s2 && n; s1++, s2++, n--)
    {
        if (*s1 != *s2)
            break;
    }
    if (!n)
        return 0;
    if (*s1 > *s2)
        return 1;
    if (*s1 < *s2)
        return -1;
    return 0;
}

void* _mxml_calloc(mxml_size_t nmemb, mxml_size_t size)
{
    mxml_size_t s = nmemb*size;
    void *r;
    r = _mxml_malloc(s);
    if (r != NULL)
        _mxml_memset(r, 0, s);
    return r;
}

int _mxml_isalnum(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return 1;
    if (ch >= 'A' && ch <= 'Z')
        return 1;
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}

const char* _mxml_strrchr(const char* s, char ch)
{
    const char *r = NULL;
    if (!s)
        return NULL;
    for(;*s;s++)
        if (*s == ch)
            r = s;
    return r;
}

long _mxml_strtol(char *nptr, char **endptr, int base)
{
    long r = 0;

    for (;*nptr;nptr++)
    {
        int digit = -1;
        if (*nptr >= '0' && *nptr <= '9')
            digit = *nptr - '0';
        if (*nptr >= 'A' && *nptr <= 'Z')
            digit = *nptr - 'A' + 10;
        if (*nptr >= 'a' && *nptr <= 'z')
            digit = *nptr - 'a' + 10;
        if (digit < 0 || digit >= base)
            break;
        r = r * base + digit;
    }
    if (endptr)
        *endptr = nptr;
    return r;
}

/*
 * End of "$Id: mxml-string.c 387 2009-04-18 17:05:52Z mike $".
 */
