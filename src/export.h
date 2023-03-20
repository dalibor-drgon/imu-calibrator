#ifndef _CALIBRATOR_EXPORT_H_
#define _CALIBRATOR_EXPORT_H_

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define CALIBRATOR_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#ifndef CALIBRATOR_EXPORT
#define CALIBRATOR_EXPORT
#endif
#endif

#endif
