/****************************************************************************
 * libc/wqueue/work_cancel.c
 *
 *   Copyright (C) 2009-2010, 2012-2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "work_queue.hpp"
#include "dq/dq.hpp"
#include "work_lock.h"

#include <px4_config.h>
#include <px4_defines.h>

namespace px4
{

/****************************************************************************
 * Name: work_cancel
 *
 * Description:
 *   Cancel previously queued work.  This removes work from the work queue.
 *   After work has been canceled, it may be re-queue by calling work_queue()
 *   again.
 *
 * Input parameters:
 *   qid    - The work queue ID
 *   work   - The previously queue work structure to cancel
 *
 * Returned Value:
 *   Zero on success, a negated errno on failure
 *
 ****************************************************************************/

int work_cancel(int qid, struct work_s *work)
{
	struct wqueue_s *wqueue = &h_work[qid];

	//DEBUGASSERT(work != NULL && (unsigned)qid < NWORKERS);

	/* Cancelling the work is simply a matter of removing the work structure
	 * from the work queue.  This must be done with interrupts disabled because
	 * new work is typically added to the work queue from interrupt handlers.
	 */

	work_lock(qid);

	if (work->worker != NULL) {
		/* A little test of the integrity of the work queue */

		//DEBUGASSERT(work->dq.flink ||(deque_entry_t *)work == wqueue->q.tail);
		//DEBUGASSERT(work->dq.blink ||(deque_entry_t *)work == wqueue->q.head);

		/* Remove the entry from the work queue and make sure that it is
		 * mark as availalbe (i.e., the worker field is nullified).
		 */

		deque_rem((deque_entry_t *)work, &wqueue->q);
		work->worker = NULL;
	}

	work_unlock(qid);
	return PX4_OK;
}

} // namespace px4
