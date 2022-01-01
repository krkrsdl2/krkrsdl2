
// Precompiled header entry point
#include "tjsCommHead.h"

// SIMDe functions included here to save some time
#if defined(__vita__) || defined(__SWITCH__)
#include <simde/simde/simde-common.h>
#undef SIMDE_HAVE_FENV_H
#endif
#include <simde/x86/sse.h>
#include <simde/x86/sse2.h>
#include <simde/x86/sse3.h>
#include <simde/x86/ssse3.h>
#include <simde/x86/sse4.1.h>
#include <simde/x86/avx.h>
#include <simde/x86/avx2.h>
#include <simde/x86/fma.h>
