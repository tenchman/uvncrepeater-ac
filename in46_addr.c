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

#define _SVID_SOURCE 1 /* s6_addr32 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include "in46_addr.h"
#ifdef FreeBSD
  #define s6_addr32 __u6_addr.__u6_addr32
#endif

const unsigned char V4mappedprefix[12] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };

/* return 0 for a mapped ipv4 address */
int inet6_v4mappedprefix(struct in6_addr *addr)
{
  return memcmp(addr, V4mappedprefix, 12);
}

/*! \brief convert a network prefix into a in6_addr netmask
 *
 * Convert the network prefix \a bits into a in6_addr structure
 * stored in \a addr.
 *
 * \param addr - an in6_addr structure to store the result in
 * \param bits - the prefix to convert
 *
 * \return Nothing
**/
void inet6_netmask(struct in6_addr *addr, int bits)
{
  int x, y = 0, z = 0;

  if (!bits || bits > 128)
    bits = 128;

  for (x = 0; x < 4; x++)
    addr->s6_addr32[x] = 0;

  y = 0;
  z = 0;
  for (x = 0; x < bits; x++) {
    addr->s6_addr[y] = addr->s6_addr[y] | (1 << (7 - z));
    z++;
    if (z == 8) {
      z = 0;
      y++;
    }
  }
}

void inet6_netaddr(struct in6_addr *dst, struct in6_addr *src, int bits)
/*! \brief return the network address of addr/bits
 *
 * Return the network address according to the \a src/\a bits
 * combination.
 *
 * \param dst[out] - an in6_addr structure to store the result in
 * \param src[in] - an in6_addr structure to read the ipaddr from
 * \param bits[in] - the network prefix to use
 *
 * \return Nothing
**/
{
  struct in6_addr mask;
  int x;

  inet6_netmask(&mask, bits);
  for (x = 0; x < 4; x++) {
    dst->s6_addr32[x] = src->s6_addr32[x] & mask.s6_addr32[x];
  }
}

/*! \brief return the broadcast address of addr/bits
 *
 * Return the broadcast address according to the \a src/\a bits
 * combination.
 *
 * \param dst[out] - an in6_addr structure to store the result in
 * \param in6_addr[in] - an in6_addr structure to read the ipaddr from
 * \param bits[in] - the network prefix to use
 *
 * \return Nothing
**/

void inet6_brdaddr(struct in6_addr *dst, struct in6_addr *src, int bits)
{
  struct in6_addr mask;
  int x;

  inet6_netmask(&mask, bits);
  for (x = 0; x < 4; x++) {
    dst->s6_addr32[x] = src->s6_addr32[x] | ~mask.s6_addr32[x];
  }
}
/*! \brief convert IPv4 and IPv6 addresses from text to binary form
 *
 * This  function  converts the character string src into a network
 * address structure in either the AF_INET or AF_INET6 family, then
 * copies the network address structure to dst.
 *
 * \param[in] src - points to a character string containing an IPv4
 *    or IPv6 network address with optional network prefix
 * \param[out] dst - an in46_addr structure to hold the result
 *
 * \return 0 on success, -1 otherwise.
**/
int inet46_pton(const char *src, struct in46_addr *dst)
{
  char copy[INET6_ADDRSTRLEN + 4] = { };
  char *bits;
  unsigned long prefix = 0;
  int ret = -1;

  strncpy(copy, src, sizeof(copy) - 1);

  if ((bits = strchr(copy, '/'))) {
    *bits++ = '\0';
    prefix = strtoul(bits, NULL, 10);
  }

  if (1 == inet_pton(AF_INET, copy, &dst->in.addr)) {
    dst->family = AF_INET;
  } else if (1 == inet_pton(AF_INET6, copy, &dst->in.addr6)) {
    dst->family = AF_INET6;
  } else {
    goto SPOR;
  }

  if ((dst->family == AF_INET && prefix <= 32) ||
      (dst->family == AF_INET6 && prefix <= 120)) {
    dst->prefix = prefix;
    ret = 0;
  }

SPOR:
  return ret;
}

int inet46_ntop(struct in46_addr *src, char *dst, socklen_t size)
{
  int n, ret = -1;

  if (!(size && dst)) {
    /* error */
  } else if (!inet_ntop(src->family, &src->in.addr6, dst, size)) {
    /* error */
  } else if (!src->prefix) {
    ret = 0;
  } else if (src->prefix < 0) {
    /* error */
  } else if (src->family == AF_INET && src->prefix > 32) {
    /* error */
  } else if (src->family == AF_INET6 && src->prefix > 120) {
    /* error */
  } else if ((n = size - strlen(dst)) <= 0) {
    /* error */
  } else if (snprintf(dst + strlen(dst), n, "/%d", src->prefix) >= n) {
    /* error */
  } else {
    ret = 0;
  }

  return ret;
}

#define IN46_ADDRSIZE (net->family == AF_INET ? sizeof(struct in_addr) : sizeof(struct in6_addr))
/*! \brief check if a host is member of a network address
 *
 * \param[in] net - address of the network to check against
 * \param[in] addr - address of the host to check
 *
 * \return 0 if the host \p addr is a member of \p net, -1 if not.
**/
int inet46_memberof(struct in46_addr *net, struct in46_addr *addr)
{
  int ret = -1;

  if (!(net && addr)) {
    /* error */
  } else if (net->family != addr->family) {
    /* error */
  } else {
    struct in6_addr n, a;
    inet6_netaddr(&n, &net->in.addr6, net->prefix);
    inet6_netaddr(&a, &addr->in.addr6, net->prefix);
    if (0 == memcmp(&n, &a, IN46_ADDRSIZE))
      ret = 0;
  }
  return ret;
}
