/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- generate and apply network deltas
 * $Id: trace.h,v 1.2 2003-11-03 16:52:30 eilers Exp $
 * 
 * Copyright (C) 2000, 2001 by Martin Pool <mbp@samba.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * TODO: A function like perror that includes strerror output.  Apache
 * does this by adding flags as well as the severity level which say
 * whether such information should be included.
 */


/*
 * trace may be turned off.
 *
 * error is always on, but you can return and continue in some way
 *
 * fatal terminates the whole process
 */

#if defined(HAVE_VARARG_MACROS) && defined(__GNUC__)
/*
 * TODO: Don't assume this is a gcc thing; rather test in autoconf for
 * support for __FUNCTION__.  One simple way might just be to try compiling 
 * the definition of one of these functions!
 *
 * TODO: Also look for the C9X predefined identifier `_function', or
 * whatever it's called.
 */

void rs_log0(int level, char const *fn, char const *fmt, ...)
    __attribute__ ((format(printf, 3, 4)));

#ifdef DO_RS_TRACE
#  define rs_trace(fmt, arg...)                            \
    do { rs_log0(RS_LOG_DEBUG, __FUNCTION__, fmt , ##arg);  \
    } while (0)
#else
#  define rs_trace(fmt, arg...)
#endif	/* !DO_RS_TRACE */

#define rs_log(l, s, str...) do {              \
     rs_log0((l), __FUNCTION__, (s) , ##str);  \
     } while (0)


#define rs_error(s, str...) do {                       \
     rs_log0(RS_LOG_ERR,  __FUNCTION__, (s) , ##str);  \
     } while (0)


#define rs_fatal(s, str...) do {               \
     rs_log0(RS_LOG_CRIT,  __FUNCTION__,       \
	      (s) , ##str);                    \
     abort();                                  \
     } while (0)


#else /************************* ! __GNUC__ */
#  define rs_trace rs_trace0
#  define rs_fatal rs_fatal0
#  define rs_error rs_error0
#  define rs_log   rs_log0_nofn
#endif				/* ! __GNUC__ */

void rs_trace0(char const *s, ...);
void rs_fatal0(char const *s, ...);
void rs_error0(char const *s, ...);
void rs_log0(int level, char const *fn, char const *fmt, ...);
void rs_log0_nofn(int level, char const *fmt, ...);

enum {
    RS_LOG_PRIMASK       = 7,   /**< Mask to extract priority
                                   part. \internal */

    RS_LOG_NONAME        = 8    /**< \b Don't show function name in
                                   message. */
};


/**
 * \macro rs_trace_enabled()
 *
 * Call this before putting too much effort into generating trace
 * messages.
 */

extern int rs_trace_level;

#ifdef DO_RS_TRACE
#  define rs_trace_enabled() ((rs_trace_level & RS_LOG_PRIMASK) >= RS_LOG_DEBUG)
#else
#  define rs_trace_enabled() 0
#endif
