/* Common definition for tst-cancel4_* tests.

   Copyright (C) 2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <pthread.h>

/* Pipe descriptors.  */
static int fds[2];

/* Temporary file descriptor, to be closed after each round.  */
static int tempfd = -1;
static int tempfd2 = -1;
/* Name of temporary file to be removed after each round.  */
static char *tempfname;
/* Temporary message queue.  */
static int tempmsg = -1;

/* Often used barrier for two threads.  */
static pthread_barrier_t b2;

/* The WRITE_BUFFER_SIZE value needs to be chosen such that if we set
   the socket send buffer size to '1', a write of this size on that
   socket will block.

   The Linux kernel imposes a minimum send socket buffer size which
   has changed over the years.  As of Linux 3.10 the value is:

     2 * (2048 + SKB_DATA_ALIGN(sizeof(struct sk_buff)))

   which is attempting to make sure that with standard MTUs,
   TCP can always queue up at least 2 full sized packets.

   Furthermore, there is logic in the socket send paths that
   will allow one more packet (of any size) to be queued up as
   long as some socket buffer space remains.   Blocking only
   occurs when we try to queue up a new packet and the send
   buffer space has already been fully consumed.

   Therefore we must set this value to the largest possible value of
   the formula above (and since it depends upon the size of "struct
   sk_buff", it is dependent upon machine word size etc.) plus some
   slack space.  */

#define WRITE_BUFFER_SIZE 16384

/* Cleanup handling test.  */
static int cl_called;

static void
cl (void *arg)
{
  ++cl_called;
}

struct cancel_tests
{
  const char *name;
  void *(*tf) (void *);
  int nb;
  int only_early;
};
#define ADD_TEST(name, nbar, early) { #name, tf_##name, nbar, early }
