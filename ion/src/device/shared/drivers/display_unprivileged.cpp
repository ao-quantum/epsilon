#include "display.h"
#include <drivers/svcall_args.h>
#include <drivers/svcall.h>

namespace Ion {
namespace Display {

using namespace Device::Display;

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void pushRectUniform(KDRect r, KDColor c) {
  // Store r and c
  const char * args[2] = {(char *)&r, (char *)&c};
  svcArgs(2, args);
  svc(SVC_PUSH_RECT_UNIFORM);
}

// void pushRect(KDRect r, const KDColor * pixels) {
  // // Store r and c
  // const char * args[2] = {(char *)&r, (char *)&pixels};
  // svcArgs(2, args);
  // svc(SVC_PUSH_RECT);
// }


}
}
