#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spicerack/andgate.h"
#include "spicerack/clock.h"
#include "spicerack/counter.h"
#include "spicerack/output.h"
#include "spicerack/signal.h"

struct Debugger {
   struct Signal* I;
   char* name;
};

void debugHandler(void* block) {
   struct Debugger* d = block;
   printf("%s: %s\n", d->name, signalValueStr(signalRead(d->I)));
}

int main(void) {

   {
      struct Clock* clock;
      struct Debugger d;
      printf("clock test\n");
      clock = clockNew();
      d.I = signalNew(&d, debugHandler);
      d.name = "clock";
      clockConnect(clock, d.I);
      clockRun(clock, 2);
      clockFree(clock);
   }

   {
      struct Debugger d;
      int writerA = 46546;
      int writerB = 94945;
      struct AndGate* gate = andGateNew();
      printf("and test\n");
      d.I = signalNew(&d, debugHandler);
      d.name = "and";
      andGateConnect(gate, d.I);
      signalWrite(andGateInputA(gate), HIGH, writerA);
      signalWrite(andGateInputB(gate), HIGH, writerB);
      signalPropagate();
      andGateFree(gate);
   }

   {
      int i;
      struct Clock* clock = clockNew();
      struct Counter* counter = counterNew();
      struct Debugger d[4];
      int rstOID;
      printf("counter test\n");
      clockConnect(clock, counterClock(counter));
      for (i = 0; i < 4; i++) {
         char buffer[10];
         sprintf(buffer, "signal%d", i);
         d[i].I = signalNew(&d[i], debugHandler);
         d[i].name = strdup(buffer);
         counterConnect(counter, i, d[i].I);
      }

      /* TODO this should be a temporary pulldown not a magic pulse */
      rstOID = allocateOutput();
      signalWrite(counterReset(counter), LOW, rstOID);
      signalPropagate();
      signalWrite(counterReset(counter), HIGH, rstOID);
      signalPropagate();

      for (i = 0; i < 20; i++) {
         printf("clock cycle %d\n", i);
         clockRun(clock, 1);
      }
      clockFree(clock);
      counterFree(counter);
      for (i = 0; i < 4; i++) {
         signalFree(d[i].I);
         free(d[i].name);
      }
   }

   return 0;
}
