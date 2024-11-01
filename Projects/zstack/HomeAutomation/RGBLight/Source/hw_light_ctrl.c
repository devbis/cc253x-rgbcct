/**************************************************************************************************
  Filename:       hw_light_ctrl.c
  Revised:        $Date: 2013-12-02 13:17:39 -0800 (Mon, 02 Dec 2013) $
  Revision:       $Revision: 36338 $


  Description:    Application physical lighting device control and color engine
                  demonstration, provided as a source for inspiration.


  Copyright 2011-2013 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This device will be like a Light device.  This application is not
  intended to be a Light device, but will use the device description
  to implement this sample code.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "onboard.h"

/* HAL */
#include "hal_timer.h"
//#include "hal_lcd.h"
//#include "hal_oled.h"
#include "hal_led.h"

#ifdef ZCL_COLOR_CTRL
#include "hueToXyTable.c"
#include "zcl_color_ctrl.h"
#endif //ZCL_COLOR_CTRL

#include "hw_light_ctrl.h"
#include "zcl_rgblight.h"

#include "math.h"

//#include "hal_board_cfg.h"


/*********************************************************************
 * MACROS
 */
#define WHITE_POINT_X 0x5000
#define WHITE_POINT_Y 0x5555

#define MAX3(a,b,c) (((a) > (b)) ? ( ((a) > (c)) ? (a) : (c) ) : ( ((b) > (c)) ? (b) : (c) ))
#define MIN3(a,b,c) (((a) > (b)) ? ( ((b) > (c)) ? (c) : (b) ) : ( ((a) > (c)) ? (c) : (a) ))
#define DISTANCE(a,b)  (((a) > (b)) ? ((a) - (b)) : ((b)-(a)))

/*********************************************************************
 * CONSTANTS
 */
#define GAMMA_VALUE 2

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
void halTimer3SetChannelDuty (uint8 channel, uint16 promill);

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#ifdef ZCL_COLOR_CTRL
static void hwLight_UpdateColor(void);
static uint8 hwLight_XyToSat(uint16 x, uint16 y, uint8 hue);
static void hwLight_satToXy(uint16 *x, uint16 *y, uint8 sat);
#ifdef ZLL_HW_LED_LAMP
static void hwLight_Convert_xyY_to_RGB(float x, float y, float Y, uint16 *R, uint16 *G,uint16 *B);
static void hwLight_GammaCorrectRGB(float *R, float *G, float *B);
static void hwLight_TempCompensateRGB(float *R, float *G, float *B);
static void hwLight_TempCompensateRGB(float *R, float *G, float *B);
static void hwLight_GroupWaveLenCompensateRGB(float *R, float *G, float *B);
static void hwLight_GroupLuminCompensateRGB(float *R, float *G, float *B);
static void hwLight_RGB_to_RGBW( uint16 *R, uint16 *G, uint16 *B, uint16 *W );
static void hwLight_UpdateLampColor( uint16 colorX, uint16 colorY, uint8 level);
#endif //ZLL_HW_LED_LAMP
#else  //ZCL_COLOR_CTRL
#ifdef ZCL_LEVEL_CTRL
static void hwLight_UpdateLevel( uint8 level );
#endif //ZCL_LEVEL_CTRL
#endif //ZCL_COLOR_CTRL

#ifdef ZCL_COLOR_CTRL
#ifdef ZLL_HW_LED_LAMP
/*********************************************************************
* @fn      zclSimpleLight_UpdateLampColor()
*
* @brief   Update color and level of lamp
*
* @param   ColorX, ColorY 0... 0xFFFF, level 0...0xFF
*
* @return  none
*/
static void hwLight_UpdateLampColor( uint16 colorX, uint16 colorY, uint8 level)
{
  uint16 redP, greenP, blueP, whiteP;

  hwLight_Convert_xyY_to_RGB((float)colorX/0xFFFF, (float)colorY/0xFFFF, (float) level*(100/(float)0xFF),
                     &redP, &greenP, &blueP);

  hwLight_RGB_to_RGBW( &redP, &greenP, &blueP, &whiteP );

  halTimer1SetChannelDuty (RED_LED,   redP);
  halTimer1SetChannelDuty (GREEN_LED, greenP);
  halTimer1SetChannelDuty (BLUE_LED,  blueP);
  halTimer1SetChannelDuty (WARM_WHITE_LED, whiteP);
//  halTimer3SetChannelDuty (COLD_WHITE_LED, whiteP);
}

/*********************************************************************
* @fn      hwLight_Convert_xyY_to_RGB
*
* @brief   Process ZCL Foundation incoming message
*
* input parameters
*
* @param  x - x value in xyY from 0 to 1
* @param  y - y value in xyY from 0 to 1
* @param  LinearY - Y value in xyY from 0 to 100
*
* output parameters
*
* @param  R - pointer to red value from 0 to PWM_FULL_DUTY_CYCLE
* @param  G - pointer to green value from 0 to PWM_FULL_DUTY_CYCLE
* @param  B - pointer to blue value from 0 to PWM_FULL_DUTY_CYCLE
*
* @return  none
*/
void hwLight_Convert_xyY_to_RGB(float x, float y, float LinearY, uint16 *R, uint16 *G, uint16 *B)
{
  float Y;
  float X;
  float Z;
  float f_R;
  float f_G;
  float f_B;

  //gamma correct the level
  Y = pow( ( LinearY / LEVEL_MAX ), (float)GAMMA_VALUE ) * (float)LEVEL_MAX;


  // from xyY to XYZ
  if (y != 0)
  {
    do
    {
      X = x * ( Y / y );
      Z = ( 1 - x - y ) * ( Y / y );
    }
    while( ((X > 95.047) || (Z > 108.883)) && ((uint8)(Y--) > 0));
    // normalize variables:
    // X from 0 to 0.95047
    X = (X > 95.047 ? 95.047 : (X < 0 ? 0 : X));
    X = X / 100;
    // Z from 0 to 1.08883
    Z = (Z > 108.883 ? 108.883 : (Z < 0 ? 0 : Z));
    Z = Z / 100;
  }
  else
  {
    X = 0;
    Z = 0;
  }
  // Y from 0 to 1
  Y = (Y > 100 ? 100 : (Y < 0 ? 0 : Y));
  Y = Y / 100;

  // transformation according to standard illuminant D65.
  f_R = X *  3.2406 + Y * -1.5372 + Z * -0.4986;
  f_G = X * -0.9689 + Y *  1.8758 + Z *  0.0415;
  f_B = X *  0.0557 + Y * -0.2040 + Z *  1.0570;

  //color correction
  hwLight_GammaCorrectRGB(&f_R, &f_G, &f_B);
  hwLight_TempCompensateRGB(&f_R, &f_G, &f_B);
  hwLight_GroupWaveLenCompensateRGB(&f_R, &f_G, &f_B);
  hwLight_GroupLuminCompensateRGB(&f_R, &f_G, &f_B);

  // truncate results exceeding 0..1
  f_R = (f_R > 1.0 ? 1.0 : (f_R < 0 ? 0 : f_R));
  f_G = (f_G > 1.0 ? 1.0 : (f_G < 0 ? 0 : f_G));
  f_B = (f_B > 1.0 ? 1.0 : (f_B < 0 ? 0 : f_B));

  *R = (uint16)(f_R * PWM_FULL_DUTY_CYCLE);
  *G = (uint16)(f_G * PWM_FULL_DUTY_CYCLE);
  *B = (uint16)(f_B * PWM_FULL_DUTY_CYCLE);
}

/*********************************************************************
* @fn      hwLight_RGB_to_RGBW
*
* @brief   Convert RGB LED values to RGBW. The white value is written to W,
*          the original RGB values are overwritten.
*
* @param   R - pointer to red input and output value from 0 to PWM_FULL_DUTY_CYCLE
* @param   G - pointer to green input and output value from 0 to PWM_FULL_DUTY_CYCLE
* @param   B - pointer to blue input and output value from 0 to PWM_FULL_DUTY_CYCLE
* @param   W - pointer to white output value from 0 to PWM_FULL_DUTY_CYCLE
*
* @return  none
*/
static void hwLight_RGB_to_RGBW( uint16 *R, uint16 *G, uint16 *B, uint16 *W )
{
  uint16 w;
  if ( MAX3(*R,*G,*B) == 0 )
  {
    *W = 0;
    return;
  }
  w = (uint16)( ( (uint32)MIN3(*R,*G,*B) * ( *R + *G + *B ) ) / ( MAX3(*R,*G,*B) * 3 ) );
  *R += w;
  *G += w;
  *B += w;
  *W = MIN3(*R,*G,*B);
  *W = (*W > PWM_FULL_DUTY_CYCLE) ? PWM_FULL_DUTY_CYCLE : *W;
  *R -= *W;
  *G -= *W;
  *B -= *W;
}

/*********************************************************************
* @fn      hwLight_GammaCorrectRGB()
*
* @brief   Gamma correct the RGB values for specific LEDS
*
*          R - pointer to red value from 0 to 1
*          G - pointer to green value from 0 to 1
*          B - pointer to blue value from 0 to 1
*
* @return  none
*/
void hwLight_GammaCorrectRGB(float *R, float *G, float *B)
{
  if ( *R > 0.003 ) *R = (1.22 * ( pow(*R, ( 1/1.5 ) )) - 0.040);
  else                      *R = 0; //1.8023 * var_R;
  if ( *G > 0.003) *G = (1.22 * ( pow(*G, ( 1/1.5 ) )) - 0.040);
  else                     *G = 0; //1.8023 * var_G;
  if ( *B > 0.003 ) *B = (1.09 * ( pow(*B, ( 1/1.5 ) )) - 0.050);
  else                     *B = 0; //1.8023 * var_B;
}

/*********************************************************************
* @fn      hwLight_TempCompensateRGB()
*
* @brief   correct the RGB values for current temp
*
*          R - pointer to red value from 0 to 1
*          G - pointer to green value from 0 to 1
*          B - pointer to blue value from 0 to 1
*
* @return  none
*/
void hwLight_TempCompensateRGB(float *R, float *G, float *B)
{
  // Fill in your code here...
}

/*********************************************************************
* @fn      hwLight_GroupWaveLenCompensateRGB()
*
* @brief   correct the specific Group WaveLength (stored in NV)
*
*          R - pointer to red value from 0 to 1
*          G - pointer to green value from 0 to 1
*          B - pointer to blue value from 0 to 1
*
* @return  none
*/
void hwLight_GroupWaveLenCompensateRGB(float *R, float *G, float *B)
{
  // Fill in your code here...
}

/*********************************************************************
* @fn      hwLight_GroupLuminCompensateRGB()
*
* @brief   correct the specific Group Lumin (stored in NV)
*
*          R - pointer to red value from 0 to 1
*          G - pointer to green value from 0 to 1
*          B - pointer to blue value from 0 to 1
*
* @return  none
*/
void hwLight_GroupLuminCompensateRGB(float *R, float *G, float *B)
{
  // Fill in your code here...
}


/*********************************************************************
* @fn      hwLight_UpdateLampColorHueSat()
*
* @brief   Update color and level of lamp
*
* @param   hue, sat, level 0...0xFF
*
* @return  none
*/
void hwLight_UpdateLampColorHueSat( uint8 hue, uint8 sat, uint8 level )
{
  uint16 colorX, colorY;

  colorX = hueToX[hue];
  colorY = hueToY[hue];

  hwLight_satToXy(&colorX, &colorY, sat);

  //color table is 0-PWM_FULL_DUTY_CYCLE
  hwLight_UpdateLampColor( colorX*(0xFFFF/COLOR_XY_MAX), colorY*(0xFFFF/COLOR_XY_MAX), level);
}
#endif //ZLL_HW_LED_LAMP

/*********************************************************************
* @fn      hwLight_satToXy
*
* @brief   Adjust x,y values from xyY gamut perimeter (fully saturated),
*          towards pre-defined white-point, according to saturation.
*
* @param   x - pointer to x value of fully saturated hue, to be overwritten
* @param   x - pointer to y value of fully saturated hue, to be overwritten
* @param   sat - saturation input
*
* @return  none
*/
static void hwLight_satToXy(uint16 *x, uint16 *y, uint8 sat)
{
  float xDelta, yDelta;
  uint16 localSat;

  localSat = 255 - sat;

  xDelta = (int32)*x - WHITE_POINT_X;
  yDelta = (int32)*y - WHITE_POINT_Y;

  xDelta = xDelta * (float)localSat/0xFF;
  yDelta = yDelta * (float)localSat/0xFF;

  *x = (uint16) (*x - xDelta);
  *y = (uint16) (*y - yDelta);

  return;
}

/*********************************************************************
* @fn      hwLight_XyToSat
*
* @brief   Set saturation parameter according to x,y coordinates
*          relative distance from white point.
*
* @param   x - x of xyY
* @param   y - y of xyY
* @param   hue - current saturated hue matching xy
*
* @return  saturation value to be set
*/
static uint8 hwLight_XyToSat(uint16 x, uint16 y, uint8 hue)
{
  uint32 xyDeltas, hueDeltas;

  //we try to avoid polor to cartesan conversion using proportional scaling
  xyDeltas = ((uint32)DISTANCE(x, WHITE_POINT_X) + DISTANCE(y, WHITE_POINT_Y));
  hueDeltas = ((uint32)DISTANCE(hueToX[hue], WHITE_POINT_X) + DISTANCE(hueToY[hue], WHITE_POINT_Y));

  return ( (hueDeltas) ? ((xyDeltas * COLOR_SAT_MAX) / hueDeltas) : COLOR_SAT_MAX );
}

/*********************************************************************
 * @fn      hwLight_UpdateColorMode
 *
 * @brief   Calculates the new xy of hue/sat value on color mode change
 *
 * @param   NewColorMode - Color Mode Attribute value
 *
 * @return  none
 */
void hwLight_UpdateColorMode(uint8 NewColorMode)
{
  uint8 idx, chosenIdx=0;
  uint32 currDist=0, minDist = 0xFFFFF;

  if( NewColorMode != zclColor_ColorMode )
  {
    if( NewColorMode == COLOR_MODE_CURRENT_X_Y )
    {
      //update the current xy values from hue and sat
      zclColor_CurrentX = hueToX[zclColor_CurrentHue];
      zclColor_CurrentY = hueToY[zclColor_CurrentHue];

      hwLight_satToXy( &zclColor_CurrentX, &zclColor_CurrentY, zclColor_CurrentSaturation );
    }
    else if( NewColorMode == COLOR_MODE_CURRENT_HUE_SATURATION )
    {
      //update the current hue/sat values from xy
      //Loop thrugh hueToX/Y tables and look for the value closest to the
      //zclColor_CurrentX and zclColor_CurrentY
      for( idx = 0; idx != 0xFF; idx++ )
      {
        currDist = ( (uint32)DISTANCE(hueToX[idx], zclColor_CurrentX)
                     + DISTANCE(hueToY[idx], zclColor_CurrentY) );

        if ( currDist < minDist )
        {
          chosenIdx = idx;
          minDist = currDist;
        }
      }
      zclColor_CurrentHue = chosenIdx;

      zclColor_EnhancedCurrentHue = (uint16)zclColor_CurrentHue << 8;

      zclColor_CurrentSaturation = hwLight_XyToSat( zclColor_CurrentX, zclColor_CurrentY, zclColor_CurrentHue );
    }
    else //do nothing COLOR_MODE_COLOR_TEMPERATURE not supported
    {
    }
  }

  return;
}


/*********************************************************************
 * @fn      hwLight_ApplyUpdate16b
 *
 * @brief
 *
 * @param   pCurrentVal - pointer to current 16bit attr (i.e. zclColor_CurrentHue) value
 * @param   pCurrentVal_256  - pointer to current attr value at hi resolution (x256)
 * @param   pStepVal_256 - pointer to the required step value of CurrentVal_256
 * @param   pRemainingTime - pointer to the time remaining,  in 100ms,  to get to the target value
 * @param   minLevel - Minimun val of attr
 * @param   maxLevel - Maximun val of attr
 *
 * @return  none
 */
void hwLight_ApplyUpdate16b( uint16 *pCurrentVal, uint32 *pCurrentVal_256, int32 *pStepVal_256, uint16 *pRemainingTime, uint16 minLevel, uint16 maxLevel, bool wrap )
{
  if( (*pStepVal_256 > 0) && ((((int32)*pCurrentVal_256 + *pStepVal_256) / 256) > maxLevel) )
  {
    if(wrap)
    {
      *pCurrentVal_256 = ( (uint32)minLevel * 256 ) + ( ( *pCurrentVal_256 + *pStepVal_256 ) - ( (uint32)maxLevel * 256 ) ) - 256;
    }
    else
    {
      *pCurrentVal_256 = (uint32)maxLevel * 256;
    }
  }
  else if( (*pStepVal_256 < 0) && ((((int32)*pCurrentVal_256 + *pStepVal_256) / 256 ) < minLevel) )
  {
    if(wrap)
    {
      *pCurrentVal_256 = ( (uint32)maxLevel * 256 ) - ( ( (uint32)minLevel * 256 ) - ((int32)*pCurrentVal_256 + *pStepVal_256) ) + 256;
    }
    else
    {
      *pCurrentVal_256 = (uint32)minLevel * 256;
    }
  }
  else
  {
    *pCurrentVal_256 += *pStepVal_256;
  }

  if (*pStepVal_256 > 0)
  {
    //fraction step compensation
    *pCurrentVal = ( *pCurrentVal_256 + 127 ) / 256;
  }
  else
  {
    *pCurrentVal = ( *pCurrentVal_256 / 256 );
  }

  if (*pRemainingTime == 0x0)
  {
    // align variables
    *pCurrentVal_256 = ((uint32)*pCurrentVal) * 256;
    *pStepVal_256 = 0;
  }
  else if (*pRemainingTime != 0xFFFF)
  {
    *pRemainingTime = *pRemainingTime-1;
  }

  hwLight_Refresh( REFRESH_AUTO );
  SCENE_INVALID();
}

/*********************************************************************
 * @fn      hwLight_UpdateColor
 *
 * @brief   Update light output according to color attributes
 *
 * @param   none
 *
 * @return  none
 */
static void hwLight_UpdateColor(void)
{

  if(zclColor_ColorMode == COLOR_MODE_CURRENT_X_Y)
  {
#ifdef ZLL_HW_LED_LAMP
    hwLight_UpdateLampColor(zclColor_CurrentX, zclColor_CurrentY, zclSampleLight_LevelCurrentLevel);
//#else //ZLL_HW_LED_LAMP #else
//  #if defined ZCL_LEVEL_CTRL
//    HalLcdWriteStringValue( "Level:", zclSampleLight_LevelCurrentLevel, 10, HAL_LCD_LINE_1 );
//  #endif //ZCL_LEVEL_CTRL
//    HalLcdWriteStringValue( "x: 0x", zclColor_CurrentX, 16, HAL_LCD_LINE_2 );
//    HalLcdWriteStringValue( "y: 0x", zclColor_CurrentY, 16, HAL_LCD_LINE_3 );
#endif //ZLL_HW_LED_LAMP
  }
  else
  {
    if(zclColor_EnhancedColorMode == ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION)
    {
      //add enhancement here for color Zoom.
      zclColor_CurrentHue = (zclColor_EnhancedCurrentHue&0xFF00)>>8;
      if ( zclColor_CurrentHue > COLOR_HUE_MAX )
      {
        zclColor_CurrentHue = COLOR_HUE_MAX;
      }
#ifdef ZLL_HW_LED_LAMP
      hwLight_UpdateLampColorHueSat( zclColor_CurrentHue, zclColor_CurrentSaturation, zclSampleLight_LevelCurrentLevel);
//#else //ZLL_HW_LED_LAMP #else
//  #if defined ZCL_LEVEL_CTRL
//      HalLcdWriteStringValue( "Level:", zclSampleLight_LevelCurrentLevel, 10, HAL_LCD_LINE_1 );
//  #endif //ZCL_LEVEL_CTRL
//      HalLcdWriteStringValue( "EnhHue: 0x", zclColor_EnhancedCurrentHue, 16, HAL_LCD_LINE_2 );
//      HalLcdWriteStringValue( "Sat:", zclColor_CurrentSaturation, 10, HAL_LCD_LINE_3 );
#endif //ZLL_HW_LED_LAMP
    }
    else
    {
      zclColor_EnhancedCurrentHue = zclColor_CurrentHue<<8;
#ifdef ZLL_HW_LED_LAMP
      hwLight_UpdateLampColorHueSat( zclColor_CurrentHue, zclColor_CurrentSaturation, zclSampleLight_LevelCurrentLevel);
//#else //ZLL_HW_LED_LAMP #else
//  #if defined ZCL_LEVEL_CTRL
//      HalLcdWriteStringValue( "Level:", zclSampleLight_LevelCurrentLevel, 10, HAL_LCD_LINE_1 );
//  #endif //ZCL_LEVEL_CTRL
//      HalLcdWriteStringValue( "Hue:", zclColor_CurrentHue, 10, HAL_LCD_LINE_2 );
//      HalLcdWriteStringValue( "Sat:", zclColor_CurrentSaturation, 10, HAL_LCD_LINE_3 );
#endif //ZLL_HW_LED_LAMP
    }
  }
}

#else //ZCL_COLOR_CTRL
//for this file to be included ZCL_LEVEL_CTRL most be defined and if
//ZCL_COLOR_CTRL is not defined then we are only controller a dimmable light
#ifdef ZCL_LEVEL_CTRL
/*********************************************************************
 * @fn      hwLight_UpdateLevel
 *
 * @brief   Update light output according to level argument
 *
 * @param   level
 *
 * @return  none
 */
static void hwLight_UpdateLevel( uint8 level )
{
#ifdef ZLL_HW_LED_LAMP

  hwLight_UpdateLampLevel( level );

//#else //ZLL_HW_LED_LAMP #else
//
//  HalLcdWriteStringValue( "Level:", zclSampleLight_LevelCurrentLevel, 10, HAL_LCD_LINE_1 );
//
#endif //ZLL_HW_LED_LAMP
}

#ifdef ZLL_HW_LED_LAMP
/*********************************************************************
 * @fn      hwLight_UpdateLampLevel
 *
 * @brief   Update lamp level output with gamma compensation
 *
 * @param   level
 *
 * @return  none
 */
void hwLight_UpdateLampLevel( uint8 level )
{
  uint16 gammaCorrectedLevel;

  //gamma correct the level
  gammaCorrectedLevel = pow( ( (float)level / LEVEL_MAX ), (float)GAMMA_VALUE ) * (float)LEVEL_MAX;

  halTimer1SetChannelDuty (WHITE_LED11, (uint16)(((uint32)gammaCorrectedLevel*PWM_FULL_DUTY_CYCLE)/LEVEL_MAX) );
}
#endif //ZLL_HW_LED_LAMP
#endif //ZCL_LEVEL_CTRL
#endif //ZCL_COLOR_CTRL

/*********************************************************************
 * @fn      hwLight_UpdateOnOff
 *
 * @brief   Update light output according to On/Off argument
 *
 * @param   state - LIGHT_ON or LIGHT_OFF
 *
 * @return  none
 */
void hwLight_UpdateOnOff( uint8 state )
{
  if ( state == LIGHT_ON )
  {
#ifdef ZLL_HW_LED_LAMP
    ENABLE_LAMP;
#else
    // On default platform, we use LED4 to simulate the Light
    HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
#endif
  }
  else
  {
#ifdef ZLL_HW_LED_LAMP
    DISABLE_LAMP;
#else
    HalLedSet( HAL_LED_4, HAL_LED_MODE_OFF );
    HalLedSet( HAL_LED_2, HAL_LED_MODE_OFF );
#endif
  }
}

/*********************************************************************
 * @fn      hwLight_ApplyUpdate
 *
 * @brief
 *
 * @param   pCurrentVal - pointer to current 8bit attr (i.e. zclColor_CurrentHue) value
 * @param   pCurrentVal_256  - pointer to current attr value at hi resolution (x256)
 * @param   pStepVal_256 - pointer to the required step value of CurrentVal_256
 * @param   pRemainingTime - pointer to the time remaining,  in 100ms,  to get to the target value
 * @param   minLevel - Minimun val of attr
 * @param   maxLevel - Maximun val of attr
 *
 * @return  none
 */
void hwLight_ApplyUpdate( uint8 *pCurrentVal, uint16 *pCurrentVal_256, int32 *pStepVal_256, uint16 *pRemainingTime, uint8 minLevel, uint8 maxLevel, bool wrap )
{
  if( (*pStepVal_256 > 0) && ((((int32)*pCurrentVal_256 + *pStepVal_256)/256) > maxLevel) )
  {
    if(wrap)
    {
      *pCurrentVal_256 = (uint16)minLevel*256 + ( ( *pCurrentVal_256 + *pStepVal_256 ) - (uint16)maxLevel*256 ) - 256;
    }
    else
    {
      *pCurrentVal_256 = (uint16)maxLevel*256;
    }
  }
  else if( (*pStepVal_256 < 0) && ((((int32)*pCurrentVal_256 + *pStepVal_256)/256) < minLevel) )
  {
    if(wrap)
    {
      *pCurrentVal_256 = (uint16)maxLevel*256 - ( (uint16)minLevel*256 - ((int32)*pCurrentVal_256 + *pStepVal_256) ) + 256 ;
    }
    else
    {
      *pCurrentVal_256 = (uint16)minLevel*256;
    }
  }
  else
  {
    *pCurrentVal_256 += *pStepVal_256;
  }

  if (*pStepVal_256 > 0)
  {
     //fraction step compensation
    *pCurrentVal = ( *pCurrentVal_256 + 127 ) / 256;
  }
  else
  {
    *pCurrentVal = ( *pCurrentVal_256 / 256 );
  }

  if (*pRemainingTime == 0x0)
  {
    // align variables
    *pCurrentVal_256 = ((uint16)*pCurrentVal)*256 ;
    *pStepVal_256 = 0;
  }
  else if (*pRemainingTime != 0xFFFF)
  {
    *pRemainingTime = *pRemainingTime-1;
  }

  hwLight_Refresh( REFRESH_AUTO );
}

/*********************************************************************
 * @fn      hwLight_Refresh
 *
 * @brief   Refresh the light output with the updated attributes values.
 *          Could be set to be bypassed, to allow temporary output manipulation
 *          by effects.
 *
 * @param   refreshState - REFRESH_AUTO, REFRESH_BYPASS or REFRESH_RESUME
 *
 * @return  none
 */
void hwLight_Refresh( uint8 refreshState )
{
  static bool dont_refresh = FALSE;
  if ( refreshState == REFRESH_BYPASS )
  {
    dont_refresh = TRUE;
  }
  else if ( refreshState == REFRESH_RESUME )
  {
    dont_refresh = FALSE;
  }
  if ( dont_refresh )
  {
    return;
  }
#ifdef ZCL_COLOR_CTRL
  //Update the xy/color attribute. Even though we are not using the color mode
  //the attr needs to be correct
  if(zclColor_ColorMode == COLOR_MODE_CURRENT_X_Y)
  {
    hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  }
  else
  {
    hwLight_UpdateColorMode(COLOR_MODE_CURRENT_X_Y);
  }
  hwLight_UpdateColor();
#else //ZCL_COLOR_CTRL
#ifdef ZCL_LEVEL_CTRL
  hwLight_UpdateLevel( zclSampleLight_LevelCurrentLevel );
#endif //ZCL_LEVEL_CTRL
#endif //ZCL_COLOR_CTRL
  hwLight_UpdateOnOff( zclSampleLight_OnOff );
}

/****************************************************************************
****************************************************************************/


