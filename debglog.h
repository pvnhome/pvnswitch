/*
 * This file is part of the PvnSwitch (https://github.com/pvnhome/pvnswitch).
 *
 * Copyright (C) 2013-2022 Victor Pyankov <pvnhome@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DEBGLOG_H_
#define DEBGLOG_H_


#if defined(NDEBUG)

#define LOG(str)
#define LOGF1(tmpl, arg1)
#define LOGF2(tmpl, arg1, arg2)
#define LOGF3(tmpl, arg1, arg2, arg3)
#define LOGF4(tmpl, arg1, arg2, arg3, arg4)
#define LOGF5(tmpl, arg1, arg2, arg3, arg4, arg5)

#else

// #define DEBUG_LLKHF_FLAGS

#define LOG(str) {printf(str); printf("\n"); fflush(stdout);}
#define LOGF1(tmpl, arg1) {printf(tmpl, arg1); printf("\n"); fflush(stdout);}
#define LOGF2(tmpl, arg1, arg2) {printf(tmpl, arg1, arg2); printf("\n"); fflush(stdout);}
#define LOGF3(tmpl, arg1, arg2, arg3) {printf(tmpl, arg1, arg2, arg3); printf("\n"); fflush(stdout);}
#define LOGF4(tmpl, arg1, arg2, arg3, arg4) {printf(tmpl, arg1, arg2, arg3, arg4); printf("\n"); fflush(stdout);}
#define LOGF5(tmpl, arg1, arg2, arg3, arg4, arg5) {printf(tmpl, arg1, arg2, arg3, arg4, arg5); printf("\n"); fflush(stdout);}
#endif

#endif /* DEBGLOG_H_ */
