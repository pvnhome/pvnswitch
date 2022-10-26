/*
 * debglog.h
 *
 *  Created on: 26 окт. 2022 г.
 *      Author: victor
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
#define LOG(str) {printf(str); printf("\n"); fflush(stdout);}
#define LOGF1(tmpl, arg1) {printf(tmpl, arg1); printf("\n"); fflush(stdout);}
#define LOGF2(tmpl, arg1, arg2) {printf(tmpl, arg1, arg2); printf("\n"); fflush(stdout);}
#define LOGF3(tmpl, arg1, arg2, arg3) {printf(tmpl, arg1, arg2, arg3); printf("\n"); fflush(stdout);}
#define LOGF4(tmpl, arg1, arg2, arg3, arg4) {printf(tmpl, arg1, arg2, arg3, arg4); printf("\n"); fflush(stdout);}
#define LOGF5(tmpl, arg1, arg2, arg3, arg4, arg5) {printf(tmpl, arg1, arg2, arg3, arg4, arg5); printf("\n"); fflush(stdout);}
#endif

#endif /* DEBGLOG_H_ */
