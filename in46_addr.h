/*
 * Copyright (C) 2012 Gernot Tenchio.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
**/

#ifndef _IN_ADDR46_H
#define _IN_ADDR46_H

#include <netinet/in.h>
#include <arpa/inet.h>

struct in46_addr
{
  int family;
  int prefix;
  union
  {
    struct in_addr addr;
    struct in6_addr addr6;
  } in;
};

int inet46_pton(const char *src, struct in46_addr *dst);
int inet46_ntop(struct in46_addr *src, char *dst, socklen_t size);
int inet46_memberof(struct in46_addr *net, struct in46_addr *addr);

void inet6_netaddr(struct in6_addr *dst, struct in6_addr *src, int bits);
void inet6_netmask(struct in6_addr *addr, int bits);

#endif
