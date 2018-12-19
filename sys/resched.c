#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <lab1.h>

unsigned long currSP;   /* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 *  *  * resched  --  reschedule processor to highest priority ready process
 *   *   *
 *    *    * Notes:       Upon entry, currpid gives current process id.
 *     *     *              Proctab[currpid].pstate gives correct NEXT state for
 *      *      *                      current process if other than PRREADY.
 *       *       *------------------------------------------------------------------------
 *        *        */
extern unsigned int class_flag;
int resched()
{
        register struct pentry  *optr;  /* pointer to old process entry */
        register struct pentry  *nptr;  /* pointer to new process entry */
if (class_flag == 1)
	{	
		int prev = 0;
		struct pentry* ptr;
		int sum = 0;
		int a = 0;
		optr = &proctab[currpid];
		if (optr->pstate == PRCURR)
		{
			sum = sum + optr->pprio;
			insert(currpid, rdyhead, optr->pprio);
			optr->pstate = PRREADY;
		}
		prev = q[rdytail].qprev;
		while (prev<NPROC)
		{
			ptr = &proctab[prev];
			sum += ptr->pprio;
			prev = q[prev].qprev;
		}
	
		if (sum != 0)
		 a = rand() % sum;
		prev = q[rdytail].qprev;
		while (prev<NPROC)
		{	ptr = &proctab[prev];
			if (sum ==0 || ((ptr->pprio)>a))
			 {
				nptr = &proctab[(currpid = prev)];
				if(optr == nptr)
				return OK;
				nptr->pstate = PRCURR;
				dequeue(prev);
					
#ifdef  RTCLOCK
				preempt = QUANTUM;
#endif
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}
			 else
                        {
                                a-= ptr->pprio;
                                prev = q[prev].qprev;
                        }
		}

			
	}
         if (class_flag == 2)
        {
                int index = q[rdytail].qprev;
                int  epoch_flag = 2;
                optr = &proctab[currpid];
                while (index < NPROC)
                {
                        if (proctab[index].goodness != 0)
                        {
                                epoch_flag = 1;
                                break;
                        }
                        index = q[index].qprev;
                }
                if (epoch_flag ==2)
                {
                         if (preempt <= 0)
                        {
                                optr->goodness = 0;
                                preempt = 0;
                        }
                        else
                                {
			optr->counter = preempt;
                        optr->goodness = (optr->pprio) + preempt;
                                }
                
                        int i = 0;
                        while (i < NPROC)
                        {
				if(proctab[i].pstate!=PRFREE)
				{
                                proctab[i].quant = proctab[i].pprio + (proctab[i].counter) / 2;
                                proctab[i].goodness = proctab[i].pprio + proctab[i].quant;
                                }
				else
				{
				proctab[i].quant = 0;
				proctab[i].goodness = 0;
				}
                                i++;
 }
                        if (optr->pstate == PRCURR)
                        {
                                insert(currpid, rdyhead, optr->pprio);
                                optr->pstate = PRREADY;
                        }
                        index = q[rdytail].qprev;

                        int max_goodness_index = index;
                        while (index < NPROC) {
                                if (proctab[max_goodness_index].goodness < proctab[index].goodness)
                                        {
                                        max_goodness_index = index;
                                        }

               	//	kprintf("\nProcess: %s Quantum %d Counter %d Goodness %d\n", proctab[index].pname, proctab[index].quant,proctab[index].counter,proctab[index].goodness); 
                                index = q[index].qprev;
                        }
                
                        nptr = &proctab[(currpid = max_goodness_index)];
                        dequeue(max_goodness_index);
                        nptr->pstate = PRCURR;

#ifdef  RTCLOCK
                        preempt = (nptr->quant);
#endif
                   
		//	kprintf("\nOPTR: %s Quantum %d Counter %d Goodness %d\n", optr->pname,optr->quant,optr->counter,optr->goodness);
		//	kprintf("\nNPTR %s Quantum %d Counter %d Goodness %d\n", nptr->pname, nptr->quant, nptr->counter, nptr->goodness);
		//	kprintf("Duration %d\n",preempt);
                        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                        return OK;
                }
                else
                {
                         if (preempt <= 0)
                        {	optr->counter = 0;
				optr->quant = 0;
                                optr->goodness = 0;
                                preempt = 0;
                        }
                        else
                                {
			if(optr->pstate!=PRFREE){
                        optr->goodness = (optr->pprio) + preempt;
			optr->quant = preempt;}
			else {
			optr->goodness = 0;
			optr->quant = 0;
			}
                                }
                        if (optr->pstate == PRCURR)
                        {
                                insert(currpid, rdyhead, optr->pprio);
                                optr->pstate = PRREADY;
                        }
                        index = q[rdytail].qprev;
                        int max_goodness_index = index;

				while (index < NPROC) {
                                if (proctab[max_goodness_index].goodness < proctab[index].goodness)
                                {
                                        max_goodness_index = index;
                                }
		//		kprintf("\nList: %s Quantum %d Counter %d Goodness %d\n", proctab[index].pname, proctab[index].quant,proctab[index].counter,proctab[index].goodness);

                                index = q[index].qprev;
                        }
                        nptr = &proctab[(currpid = max_goodness_index)];
                        dequeue(max_goodness_index);
                        nptr->pstate = PRCURR;

#ifdef  RTCLOCK
                        preempt = (nptr->quant);
#endif
		//	 kprintf("\nOPTR: %s Quantum %d Counter %d Goodness %d\n", optr->pname,optr->quant,optr->counter,optr->goodness);
		//	kprintf("\nNPTR %s Quantum %d Counter %d Goodness %d\n", nptr->pname, nptr->quant, nptr->counter, nptr->goodness);
                //        kprintf("Duration %d\n",preempt);
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                        return OK;
                }
        }
else
{
if (((optr = &proctab[currpid])->pstate == PRCURR) &&
                        (lastkey(rdytail) < optr->pprio)) {
                        return(OK);
                }

                /* force context switch */

                if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid, rdyhead, optr->pprio);
                }

                /* remove highest priority process at end of ready list */

                nptr = &proctab[(currpid = getlast(rdytail))];
                nptr->pstate = PRCURR;          /* mark it currently running    */
#ifdef  RTCLOCK
                preempt = QUANTUM;              /* reset preemption counter     */
#endif
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

                /* The OLD process returns here when resumed. */
                return OK;
}
}


