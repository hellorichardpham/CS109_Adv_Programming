// Mustapha Hamade - mhamade
// asg1

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

#include "trace.h"

typedef vector<char> boolvec;
boolvec traceflags::flags (UCHAR_MAX + 1, false);
const boolvec trueflags (UCHAR_MAX + 1, true);

void traceflags::setflags (const string &flags) {
   for (auto itor = flags.begin(); itor != flags.end(); ++itor) {
      if (*itor == '@') {
         traceflags::flags = trueflags;
      }else {
         traceflags::flags[*itor] = true;
      }
   }
   // Note that TRACE can trace setflags.
   TRACE ('t', "flags = " << flags);
}

//
// getflag -
//    Check to see if a certain flag is on.
//

bool traceflags::getflag (char flag) {
   // WARNING: Don't TRACE this function or the stack will blow up.
   bool result = flags[flag];
   return result;
}
