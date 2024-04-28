/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Neural_network_Embedded_sys.c
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

#include "Neural_network_Embedded_sys.h"
#include <string.h>
#include <math.h>
#include "rtwtypes.h"
#include <stddef.h>
#define NumBitsPerChar                 8U

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
extern real_T rt_roundd_snf(real_T u);
static real_T rtGetInf(void);
static real32_T rtGetInfF(void);
static real_T rtGetMinusInf(void);
static real32_T rtGetMinusInfF(void);
static real_T rtGetNaN(void);
static real32_T rtGetNaNF(void);

#define NOT_USING_NONFINITE_LITERALS   1

extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static void rt_InitInfAndNaN(size_t realSize);
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetNaNF(void)
{
  IEEESingle nanF = { { 0.0F } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
static void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  boolean_T result = (boolean_T) 0;
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  if (bitsPerReal == 32U) {
    result = rtIsNaNF((real32_T)value);
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.fltVal = value;
    result = (boolean_T)((tmpVal.bitVal.words.wordH & 0x7FF00000) == 0x7FF00000 &&
                         ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                          (tmpVal.bitVal.words.wordL != 0) ));
  }

  return result;
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  IEEESingle tmp;
  tmp.wordL.wordLreal = value;
  return (boolean_T)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                     (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/* Model step function */
void Neural_network_Embedded_sys_step(void)
{
  int64_T tmp;
  real_T rtb_y;
  real_T tanhTable;
  int32_T i;
  static const real_T tanhTable_0[52] = { 0.76159415595576485, 0.964027580075817,
    0.99505475368673058, 0.99932929973906692, 0.999909204262595,
    0.99998771165079559, 0.99999833694394469, 0.999999774929676,
    0.99999996954004089, 0.99999999587769262, 0.99999999944210649,
    0.99999999992449717, 0.99999999998978184, 0.99999999999861722,
    0.99999999999981282, 0.9999999999999748, 0.99999999999999656,
    0.99999999999999956, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

  static const real_T atanhTable[52] = { 0.54930614433405478,
    0.25541281188299536, 0.12565721414045306, 0.062581571477003009,
    0.031260178490666993, 0.015626271752052213, 0.0078126589515404212,
    0.0039062698683968262, 0.00195312748353255, 0.000976562810441036,
    0.00048828128880511288, 0.00024414062985063861, 0.00012207031310632982,
    6.1035156325791221E-5, 3.05175781344739E-5, 1.5258789063684237E-5,
    7.62939453139803E-6, 3.8146972656435034E-6, 1.9073486328148128E-6,
    9.53674316406539E-7, 4.768371582031611E-7, 2.38418579101567E-7,
    1.1920928955078181E-7, 5.9604644775390691E-8, 2.9802322387695319E-8,
    1.4901161193847656E-8, 7.4505805969238281E-9, 3.7252902984619141E-9,
    1.862645149230957E-9, 9.3132257461547852E-10, 4.6566128730773926E-10,
    2.3283064365386963E-10, 1.1641532182693481E-10, 5.8207660913467407E-11,
    2.9103830456733704E-11, 1.4551915228366852E-11, 7.2759576141834259E-12,
    3.637978807091713E-12, 1.8189894035458565E-12, 9.0949470177292824E-13,
    4.5474735088646412E-13, 2.2737367544323206E-13, 1.1368683772161603E-13,
    5.6843418860808015E-14, 2.8421709430404007E-14, 1.4210854715202004E-14,
    7.1054273576010019E-15, 3.5527136788005009E-15, 1.7763568394002505E-15,
    8.8817841970012523E-16, 4.4408920985006262E-16, 2.2204460492503131E-16 };

  /* MATLAB Function: '<S1>/ Compute TANH' */
  if (!rtDW.xTemp_not_empty) {
    memcpy(&rtDW.tpX[0], &tanhTable_0[0], 52U * sizeof(real_T));
    memcpy(&rtDW.tpY[0], &tanhTable_0[0], 52U * sizeof(real_T));
    rtDW.xTemp_not_empty = true;
  }

  rtb_y = rtDW.yOut;
  switch (rtDW.mState) {
   case 0:
    rtDW.mState = 0;
    break;

   case 1:
    rtDW.xTempShifted = ldexp(rtDW.xTemp, -rtDW.idx);
    rtDW.yTempShifted = ldexp(rtDW.yTemp, -rtDW.idx);
    rtDW.mState = 2;
    break;

   case 2:
    if (rtDW.z >= 0.0) {
      rtDW.xTemp += rtDW.yTempShifted;
      rtDW.yTemp += rtDW.xTempShifted;
      rtDW.z -= atanhTable[rtDW.idx - 1];
    } else {
      rtDW.xTemp -= rtDW.yTempShifted;
      rtDW.yTemp -= rtDW.xTempShifted;
      rtDW.z += atanhTable[rtDW.idx - 1];
    }

    if (rtDW.idx == rtDW.k) {
      rtDW.mState = 3;
    } else if (rtDW.idx == 52) {
      rtDW.mState = 5;
    } else {
      rtDW.mState = 1;
      i = rtDW.idx + 1;
      if (rtDW.idx + 1 > 127) {
        i = 127;
      }

      rtDW.idx = (int8_T)i;
    }
    break;

   case 3:
    rtDW.xTempShifted = ldexp(rtDW.xTemp, -rtDW.idx);
    rtDW.yTempShifted = ldexp(rtDW.yTemp, -rtDW.idx);
    rtDW.mState = 4;
    break;

   case 4:
    if (rtDW.z >= 0.0) {
      rtDW.xTemp += rtDW.yTempShifted;
      rtDW.yTemp += rtDW.xTempShifted;
      rtDW.z -= atanhTable[rtDW.idx - 1];
    } else {
      rtDW.xTemp -= rtDW.yTempShifted;
      rtDW.yTemp -= rtDW.xTempShifted;
      rtDW.z += atanhTable[rtDW.idx - 1];
    }

    if (rtDW.idx == 52) {
      rtDW.mState = 5;
    } else {
      tmp = 3LL * rtDW.k;
      if (tmp > 2147483647LL) {
        tmp = 2147483647LL;
      } else if (tmp < -2147483648LL) {
        tmp = -2147483648LL;
      }

      if ((int32_T)tmp > 2147483646) {
        rtDW.k = MAX_int32_T;
      } else {
        rtDW.k = (int32_T)tmp + 1;
      }

      rtDW.mState = 1;
      i = rtDW.idx + 1;
      if (rtDW.idx + 1 > 127) {
        i = 127;
      }

      rtDW.idx = (int8_T)i;
    }
    break;

   case 5:
    for (i = 0; i < 52; i++) {
      tanhTable = tanhTable_0[i];
      rtDW.tpX[i] = tanhTable * rtDW.xTemp;
      rtDW.tpY[i] = tanhTable * rtDW.yTemp;
    }

    rtDW.mState = 6;
    break;

   case 6:
    tanhTable = rt_roundd_snf(rtDW.q);
    if (tanhTable < 2.147483648E+9) {
      if (tanhTable >= -2.147483648E+9) {
        i = (int32_T)tanhTable;
      } else {
        i = MIN_int32_T;
      }
    } else {
      i = MAX_int32_T;
    }

    if (i == 0) {
      rtDW.sProduct = 0.0;
      rtDW.cProduct = 0.0;
    } else if (i < -52) {
      rtDW.sProduct = -rtDW.yTemp;
      rtDW.cProduct = -rtDW.xTemp;
    } else if (i > 52) {
      rtDW.sProduct = rtDW.yTemp;
      rtDW.cProduct = rtDW.xTemp;
    } else if (i > 0) {
      rtDW.sProduct = rtDW.tpY[i - 1];
      rtDW.cProduct = rtDW.tpX[i - 1];
    } else {
      rtDW.sProduct = -rtDW.tpY[-i - 1];
      rtDW.cProduct = -rtDW.tpX[-i - 1];
    }

    rtDW.mState = 7;
    break;

   case 7:
    rtDW.sSum = rtDW.yTemp + rtDW.cProduct;
    rtDW.cSum = rtDW.xTemp + rtDW.sProduct;
    rtDW.mState = 8;
    rtDW.idx = 1;
    rtDW.zFinal = 0.0;
    break;

   case 8:
    rtDW.cTemp = ldexp(rtDW.cSum, -rtDW.idx);
    rtDW.oneTemp = ldexp(1.0, -rtDW.idx);
    rtDW.mState = 9;
    break;

   case 9:
    if (rtDW.sSum < 0.0) {
      rtDW.sSum += rtDW.cTemp;
      rtDW.zFinal -= rtDW.oneTemp;
    } else {
      rtDW.sSum -= rtDW.cTemp;
      rtDW.zFinal += rtDW.oneTemp;
    }

    if (rtDW.idx == 51) {
      rtDW.mState = 10;
    } else {
      rtDW.mState = 8;
      i = rtDW.idx + 1;
      if (rtDW.idx + 1 > 127) {
        i = 127;
      }

      rtDW.idx = (int8_T)i;
    }
    break;

   case 10:
    rtDW.mState = 0;
    rtDW.yOut = rtDW.zFinal;
    rtb_y = rtDW.zFinal;
    break;

   default:
    rtDW.mState = 0;
    break;
  }

  /* End of MATLAB Function: '<S1>/ Compute TANH' */

  /* Math: '<S2>/Exp'
   *
   * About '<S2>/Exp':
   *  Operator: exp
   */
  rtb_y = exp(rtb_y);

  /* DataTypeConversion: '<S2>/convert' incorporates:
   *  RelationalOperator: '<S2>/Is Inf'
   */
  tanhTable = (rtb_y == (rtInf));

  /* Switch: '<S2>/Switch' incorporates:
   *  DataTypeConversion: '<S2>/convert'
   *  Product: '<S2>/Product1'
   *  Signum: '<S2>/Sign'
   */
  if (tanhTable > 0.5) {
    /* Signum: '<S2>/Sign' */
    if (rtIsNaN(rtb_y)) {
      rtb_y = (rtNaN);
    } else {
      rtb_y = (rtb_y > 0.0);
    }

    rtb_y *= tanhTable;
  }

  /* End of Switch: '<S2>/Switch' */

  /* Switch: '<S2>/Switch1' incorporates:
   *  Constant: '<S2>/Constant'
   *  Sum: '<S2>/Sum'
   */
  if (rtb_y != 0.0) {
    tanhTable = rtb_y;
  } else {
    tanhTable = 1.0;
  }

  /* Outport: '<Root>/Output' incorporates:
   *  Math: '<S2>/Reciprocal'
   *  Product: '<S2>/Product'
   *  Switch: '<S2>/Switch1'
   *
   * About '<S2>/Reciprocal':
   *  Operator: reciprocal
   */
  rtY.Output = 1.0 / tanhTable * rtb_y;
}

/* Model initialize function */
void Neural_network_Embedded_sys_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* SystemInitialize for MATLAB Function: '<S1>/ Compute TANH' */
  rtDW.k = 4;
  rtDW.idx = 1;
  rtDW.xTemp = 1.0;
  rtDW.xTempShifted = 1.0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
