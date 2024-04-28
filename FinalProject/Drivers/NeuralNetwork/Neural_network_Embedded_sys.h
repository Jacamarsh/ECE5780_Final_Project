/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Neural_network_Embedded_sys.h
 *
 * Code generated for Simulink model 'Neural_network_Embedded_sys'.
 *
 * Model version                  : 1.2
 * Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
 * C/C++ source code generated on : Thu Apr 25 01:32:51 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Neural_network_Embedded_sys_h_
#define RTW_HEADER_Neural_network_Embedded_sys_h_
#ifndef Neural_network_Embedded_sys_COMMON_INCLUDES_
#define Neural_network_Embedded_sys_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                        /* Neural_network_Embedded_sys_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T tpX[52];                      /* '<S1>/ Compute TANH' */
  real_T tpY[52];                      /* '<S1>/ Compute TANH' */
  real_T xTemp;                        /* '<S1>/ Compute TANH' */
  real_T yTemp;                        /* '<S1>/ Compute TANH' */
  real_T cTemp;                        /* '<S1>/ Compute TANH' */
  real_T oneTemp;                      /* '<S1>/ Compute TANH' */
  real_T yOut;                         /* '<S1>/ Compute TANH' */
  real_T q;                            /* '<S1>/ Compute TANH' */
  real_T z;                            /* '<S1>/ Compute TANH' */
  real_T zFinal;                       /* '<S1>/ Compute TANH' */
  real_T xTempShifted;                 /* '<S1>/ Compute TANH' */
  real_T yTempShifted;                 /* '<S1>/ Compute TANH' */
  real_T sProduct;                     /* '<S1>/ Compute TANH' */
  real_T cProduct;                     /* '<S1>/ Compute TANH' */
  real_T sSum;                         /* '<S1>/ Compute TANH' */
  real_T cSum;                         /* '<S1>/ Compute TANH' */
  int32_T k;                           /* '<S1>/ Compute TANH' */
  int8_T idx;                          /* '<S1>/ Compute TANH' */
  int8_T mState;                       /* '<S1>/ Compute TANH' */
  boolean_T xTemp_not_empty;           /* '<S1>/ Compute TANH' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Input;                        /* '<Root>/Input' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Output;                       /* '<Root>/Output' */
} ExtY;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void Neural_network_Embedded_sys_initialize(void);
extern void Neural_network_Embedded_sys_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S2>/OR' : Eliminated since input is scalar
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Neural_network_Embedded_sys'
 * '<S1>'   : 'Neural_network_Embedded_sys/Hyperbolic Tangent HDL Optimized'
 * '<S2>'   : 'Neural_network_Embedded_sys/softmax'
 * '<S3>'   : 'Neural_network_Embedded_sys/Hyperbolic Tangent HDL Optimized/ Compute TANH'
 */
#endif                           /* RTW_HEADER_Neural_network_Embedded_sys_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
