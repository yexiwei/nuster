/*
 * include/nuster/common.h
 * This file defines everything related to nuster common.
 *
 * Copyright (C) Jiang Wenyuan, < koubunen AT gmail DOT com >
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * exclusively.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _NUSTER_COMMON_H
#define _NUSTER_COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

#if defined NUSTER_USE_PTHREAD || defined USE_PTHREAD_PSHARED
#include <pthread.h>
#else
#ifdef USE_SYSCALL_FUTEX
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#endif
#endif

#include <common/mini-clist.h>
#include <types/acl.h>

#define NUSTER_OK               0
#define NUSTER_ERR              1

#define NST_DEFAULT_TTL         0

enum {
    NUSTER_STATUS_UNDEFINED = -1,
    NUSTER_STATUS_OFF       =  0,
    NUSTER_STATUS_ON        =  1,
};

enum {
    NUSTER_MODE_CACHE = 1,
    NUSTER_MODE_NOSQL,
};

#define nuster_str_set(str)     { (char *) str, sizeof(str) - 1 }

struct nuster_str {
    char *data;
    int   len;
};

enum nuster_rule_key_type {
    /* method: GET, POST... */
    NUSTER_RULE_KEY_METHOD = 1,

    /* scheme: http, https */
    NUSTER_RULE_KEY_SCHEME,

    /* host: Host header */
    NUSTER_RULE_KEY_HOST,

    /* uri: first slash to end of the url */
    NUSTER_RULE_KEY_URI,

    /* path: first slach to question mark */
    NUSTER_RULE_KEY_PATH,

    /* delimiter: '?' or '' */
    NUSTER_RULE_KEY_DELIMITER,

    /*query: question mark to end of the url, or empty */
    NUSTER_RULE_KEY_QUERY,

    /* param: query key/value pair */
    NUSTER_RULE_KEY_PARAM,

    /* header */
    NUSTER_RULE_KEY_HEADER,

    /* cookie */
    NUSTER_RULE_KEY_COOKIE,

    /* body */
    NUSTER_RULE_KEY_BODY,
};

struct nuster_rule_key {
    enum nuster_rule_key_type  type;
    char                      *data;
};

struct nuster_rule_code {
    struct nuster_rule_code *next;
    int                      code;
};

enum {
    NUSTER_RULE_DISABLED = 0,
    NUSTER_RULE_ENABLED  = 1,
};

enum {
    /* no disk persistence */
    NUSTER_DISK_OFF    = 0,

    /* disk persistence only, do not cache in memory */
    NUSTER_DISK_ONLY   = 1,

    /* persist the response on disk before return to client */
    NUSTER_DISK_SYNC   = 2,

    /* cache in memory first and persist on disk later */
    NUSTER_DISK_ASYNC  = 3,
};

struct nuster_rule {
    struct list              list;       /* list linked to from the proxy */
    struct acl_cond         *cond;       /* acl condition to meet */
    char                    *name;       /* cache name for logging */
    struct nuster_rule_key **key;        /* key */
    struct nuster_rule_code *code;       /* code */
    uint32_t                *ttl;        /* ttl: seconds, 0: not expire */
    int                     *state;      /* enabled or disabled */
    int                      id;         /* same for identical names */
    int                      uuid;       /* unique cache-rule ID */
    int                      disk;       /* NUSTER_DISK_* */
};

struct nuster_rule_stash {
    struct nuster_rule_stash *next;
    struct nuster_rule       *rule;
    struct buffer            *key;
    uint64_t                  hash;
};

struct nuster_flt_conf {
    int status;
};


/* get current timestamp in milliseconds */
static inline uint64_t get_current_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void nuster_debug(const char *fmt, ...);

#endif /* _NUSTER_COMMON_H */
