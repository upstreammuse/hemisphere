#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spicerack/andgate.h"
#include "spicerack/clock.h"
#include "spicerack/counter.h"
#include "spicerack/output.h"
#include "spicerack/signal.h"
#include "spicerack/signalset.h"
#include "spicerack/watcher.h"

int main(void) {

   printf("*** clock test\n");
   {
      struct Clock* clock = clockNew();
      struct Watcher* w = watcherNew("clock");
      clockConnect(clock, watcherClock(w));
      clockConnect(clock, watcherInput(w));
      clockRun(clock, 2);
      clockFree(clock);
      watcherFree(w);
   }

   printf("*** and test\n");
   {
      struct AndGate* gate = andGateNew();
      struct Watcher* w = watcherNew("and");
      int writerA = allocateOutput();
      int writerB = allocateOutput();
      andGateConnect(gate, watcherInput(w));
      signalWrite(andGateInputA(gate), HIGH, writerA);
      signalWrite(andGateInputB(gate), HIGH, writerB);
      signalPropagate();
      andGateFree(gate);
      watcherFree(w);
   }

   printf("counter test\n");
   {
      int i;
      struct Clock* clock = clockNew();
      struct Counter* counter = counterNew();
      struct Watcher* w[4];
      int rstOID;
      clockConnect(clock, counterClock(counter));
      for (i = 0; i < 4; i++) {
         char buffer[10];
         sprintf(buffer, "signal%d", i);
         w[i] = watcherNew(buffer);
         clockConnect(clock, watcherClock(w[i]));
         counterConnect(counter, i, watcherInput(w[i]));
      }

      /* TODO this should be a temporary pulldown not a magic pulse */
      rstOID = allocateOutput();
      signalWrite(counterReset(counter), LOW, rstOID);
      signalPropagate();
      signalWrite(counterReset(counter), HIGH, rstOID);
      signalPropagate();

      clockRun(clock, 20);
      clockFree(clock);
      counterFree(counter);
      for (i = 0; i < 4; i++) {
         watcherFree(w[i]);
      }
   }

   printf("*** signal set test\n");
   {
      struct SignalSet* set = signalSetNew(50);
      struct AndGate* gate = andGateNew();
      signalSetAt(set, 13);
      signalSetMap(set, 15, andGateInputA(gate));
      signalSetFree(set);
   }

   return 0;
}
