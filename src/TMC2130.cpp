/*
 * TMC2130.cpp
 *
 *  Created on: 03 nov 2019
 *      Author: Tim Evers
 */


#include "TMC2130.h"

#if defined(FARMDUINO_EXP_V20) || defined(FARMDUINO_V30) || defined(FARMDUINO_V32)

void loadTMC2130ParametersMotor(TMC2130_Basics *tb, int microsteps, int current, int sensitivity, bool stealth)
{
  //loadingParameters = true;

  uint32_t data = 0;
  uint32_t value = 0;

  tb->init();

  // Set the micro steps
  switch (microsteps) {
  case 1:
    data = 8;
    break;
  case 2:
    data = 7;
    break;
  case 4:
    data = 6;
    break;
  case 8:
    data = 5;
    break;
  case 16:
    data = 4;
    break;
  case 32:
    data = 3;
    break;
  case 64:
    data = 2;
    break;
  case 128:
    data = 1;
    break;
  }

  tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(data) << FB_TMC_CHOPCONF_MRES, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_MRES] << FB_TMC_CHOPCONF_MRES);

  // Set the current

  uint16_t mA = current;
  float multiplier = 0.5;
  float RS = 0.11;
  uint8_t CS = 32.0*1.41421*mA / 1000.0*(RS + 0.02) / 0.325 - 1;
  if (CS < 16) {
    CS = 32.0*1.41421*mA / 1000.0*(RS + 0.02) / 0.180 - 1;
  }
  data = ((uint32_t(CS)&FB_TMC_IHOLD_MASK) << FB_TMC_IHOLD);
  data |= ((uint32_t(CS)&FB_TMC_IRUN_MASK) << FB_TMC_IRUN);
  data |= ((uint32_t(16)&FB_TMC_IHOLDDELAY_MASK) << FB_TMC_IHOLDDELAY);
  tb->write_REG(FB_TMC_REG_IHOLD_IRUN, data);

  // Chop and cool conf

  if (!stealth)
  {
    tb->set_GCONF(FB_TMC_GCONF_I_SCALE_ANALOG, 1);

    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(3) << FB_TMC_CHOPCONF_TOFF, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TOFF] << FB_TMC_CHOPCONF_TOFF);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(4) << FB_TMC_CHOPCONF_HSTRT, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_HSTRT] << FB_TMC_CHOPCONF_HSTRT);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(1) << FB_TMC_CHOPCONF_HEND, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_HEND] << FB_TMC_CHOPCONF_HEND);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(2) << FB_TMC_CHOPCONF_TBL, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TBL] << FB_TMC_CHOPCONF_TBL);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0) << FB_TMC_CHOPCONF_CHM, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_CHM] << FB_TMC_CHOPCONF_CHM);

    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(1) << FB_TMC_COOLCONF_SEMIN, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEMIN] << FB_TMC_COOLCONF_SEMIN);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(2) << FB_TMC_COOLCONF_SEMAX, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEMAX] << FB_TMC_COOLCONF_SEMAX);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b01) << FB_TMC_COOLCONF_SEDN, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEDN] << FB_TMC_COOLCONF_SEDN);

    tb->write_REG(FB_TMC_REG_TPWMTHRS, uint32_t(0xFFFFFFFF));
    tb->write_REG(FB_TMC_REG_TCOOLTHRS, 0xFFFFF & FB_TMC_TCOOLTHRS_MASK);
    tb->write_REG(FB_TMC_REG_THIGH, 10000 & FB_TMC_THIGH_MASK);

    //dcStep settings
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b01) << FB_TMC_CHOPCONF_VHIGHCHM, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHCHM] << FB_TMC_CHOPCONF_VHIGHCHM);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b01) << FB_TMC_CHOPCONF_VHIGHFS, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHFS] << FB_TMC_CHOPCONF_VHIGHFS);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(8) << FB_TMC_CHOPCONF_TOFF, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TOFF] << FB_TMC_CHOPCONF_TOFF);
    tb->alter_REG(FB_TMC_REG_DCCTRL, uint32_t(100) << FB_TMC_DCCTRL_DC_TIME, FB_TMC_DCCTRL_DC_TIME_MASK << FB_TMC_DCCTRL_DC_TIME);
    tb->alter_REG(FB_TMC_REG_DCCTRL, uint32_t(8) << FB_TMC_DCCTRL_DC_SG, FB_TMC_DCCTRL_DC_SG_MASK << FB_TMC_DCCTRL_DC_SG);

    tb->write_REG(FB_TMC_REG_THIGH, uint32_t(10000));

    // Enable diagnostics
    tb->set_GCONF(FB_TMC_GCONF_DIAG0_ERROR, 1);
    tb->set_GCONF(FB_TMC_GCONF_DIAG1_STEPS_SKIPPED, 1);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(sensitivity) << FB_TMC_COOLCONF_SGT, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SGT] << FB_TMC_COOLCONF_SGT);

    // Disable stealth
    tb->set_GCONF(FB_TMC_GCONF_EN_PWM_MODE, uint32_t(1));

  }

  if (stealth)
  {
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(3) << FB_TMC_CHOPCONF_TOFF, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TOFF] << FB_TMC_CHOPCONF_TOFF);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(1) << FB_TMC_CHOPCONF_TBL, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TBL] << FB_TMC_CHOPCONF_TBL);

    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0) << FB_TMC_CHOPCONF_HSTRT, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_HSTRT] << FB_TMC_CHOPCONF_HSTRT);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0) << FB_TMC_CHOPCONF_HEND, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_HEND] << FB_TMC_CHOPCONF_HEND);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(1) << FB_TMC_CHOPCONF_INTPOL, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_INTPOL] << FB_TMC_CHOPCONF_INTPOL);

    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0) << FB_TMC_CHOPCONF_CHM, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_CHM] << FB_TMC_CHOPCONF_CHM);

    // Disable settings from non-stealth mode
    tb->set_GCONF(FB_TMC_GCONF_I_SCALE_ANALOG, 0);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b00) << FB_TMC_CHOPCONF_VHIGHCHM, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHCHM] << FB_TMC_CHOPCONF_VHIGHCHM);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b00) << FB_TMC_CHOPCONF_VHIGHFS, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHFS] << FB_TMC_CHOPCONF_VHIGHFS);

    // Disable dcStep settings for non-stealth mode
    tb->alter_REG(FB_TMC_REG_DCCTRL, uint32_t(0) << FB_TMC_DCCTRL_DC_TIME, FB_TMC_DCCTRL_DC_TIME_MASK << FB_TMC_DCCTRL_DC_TIME);
    tb->alter_REG(FB_TMC_REG_DCCTRL, uint32_t(0) << FB_TMC_DCCTRL_DC_SG, FB_TMC_DCCTRL_DC_SG_MASK << FB_TMC_DCCTRL_DC_SG);

    // Enbable stealth
    tb->alter_REG(FB_TMC_REG_PWMCONF, uint32_t(1) << FB_TMC_PWMCONF_PWM_AUTOSCALE, FB_TMC_PWMCONF_MASKS[FB_TMC_PWMCONF_PWM_AUTOSCALE] << FB_TMC_PWMCONF_PWM_AUTOSCALE);
    tb->alter_REG(FB_TMC_REG_PWMCONF, uint32_t(0) << FB_TMC_PWMCONF_PWM_FREQ, FB_TMC_PWMCONF_MASKS[FB_TMC_PWMCONF_PWM_FREQ] << FB_TMC_PWMCONF_PWM_FREQ);
    tb->alter_REG(FB_TMC_REG_PWMCONF, uint32_t(1) << FB_TMC_PWMCONF_PWM_GRAD, FB_TMC_PWMCONF_MASKS[FB_TMC_PWMCONF_PWM_GRAD] << FB_TMC_PWMCONF_PWM_GRAD);
    tb->alter_REG(FB_TMC_REG_PWMCONF, uint32_t(128) << FB_TMC_PWMCONF_PWM_AMPL, FB_TMC_PWMCONF_MASKS[FB_TMC_PWMCONF_PWM_AMPL] << FB_TMC_PWMCONF_PWM_AMPL);

    tb->set_GCONF(FB_TMC_GCONF_EN_PWM_MODE, uint32_t(1));

    // Set up cool conf
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(1) << FB_TMC_COOLCONF_SEMIN, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEMIN] << FB_TMC_COOLCONF_SEMIN);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(2) << FB_TMC_COOLCONF_SEMAX, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEMAX] << FB_TMC_COOLCONF_SEMAX);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b01) << FB_TMC_COOLCONF_SEDN, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SEDN] << FB_TMC_COOLCONF_SEDN);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b00) << FB_TMC_CHOPCONF_VHIGHCHM, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHCHM] << FB_TMC_CHOPCONF_VHIGHCHM);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0b00) << FB_TMC_CHOPCONF_VHIGHFS, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_VHIGHFS] << FB_TMC_CHOPCONF_VHIGHFS);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(8) << FB_TMC_CHOPCONF_TOFF, FB_TMC_CHOPCONF_MASKS[FB_TMC_CHOPCONF_TOFF] << FB_TMC_CHOPCONF_TOFF);




    // Disable diagnostics
    tb->set_GCONF(FB_TMC_GCONF_DIAG0_ERROR, 0);
    tb->set_GCONF(FB_TMC_GCONF_DIAG1_STEPS_SKIPPED, 0);
    tb->alter_REG(FB_TMC_REG_CHOPCONF, uint32_t(0) << FB_TMC_COOLCONF_SGT, FB_TMC_CHOPCONF_MASKS[FB_TMC_COOLCONF_SGT] << FB_TMC_COOLCONF_SGT);

  }

  //loadingParameters = false;

  delay(100);
}

#endif
