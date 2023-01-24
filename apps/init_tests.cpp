#include "init.h"
#include "global_preferences.h"
#include "shared/continuous_function_store.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  ::GlobalPreferences::sharedGlobalPreferences.init();
  ::Shared::GlobalContext::sequenceStore.init();
  ::Shared::GlobalContext::continuousFunctionStore.init();
}

}
