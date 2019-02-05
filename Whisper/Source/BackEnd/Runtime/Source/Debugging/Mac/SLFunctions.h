#pragma once

#ifdef __cplusplus
extern "C" {
#endif

RUNTIME_EXPORT void SLDisable(void);
RUNTIME_EXPORT void SLEnable(void);

RUNTIME_EXPORT void SLEnterInterrupt(void);
RUNTIME_EXPORT void SLLeaveInterrupt(void);

RUNTIME_EXPORT void SLResetLeaks(void);

#ifdef __cplusplus
}
#endif
