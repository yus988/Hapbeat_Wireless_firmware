/////////////////////// define pins /////////////////
#ifndef PIN_ASSIGNMENT_H
#define PIN_ASSIGNMENT_H

#if defined(NECKLACE_V2)
  // Audio pins
  #define BCLK_PIN 39
  #define LRCK_PIN 38
  #define DOUT_PIN 40
  #define I2S_MLCK_PIN 41
  #define EN_I2S_DAC_PIN 11
  // ピン番号はv1_3で変更予定
  #define EN_MOTOR_PIN 9
  // analog amp
  #define EN_VIBAMP_PIN 10
  #define AIN_VIBVOL_PIN 1
  #define AOUT_VIBVOL_PIN 6
  // _display pins
  #define SCLK_PIN 17
  #define MOSI_PIN 18
  #define MISO_PIN 37
  #define OLED_DC_PIN 16
  #define OLED_RESET_PIN 15
  #define CS_PIN 14
  #define EN_OLED_PIN 21
  // Button pins
  #define SW1_VOL_P_PIN 26
  #define SW2_VOL_N_PIN 47
  // 基板上はこちらが正しいが、旧版に合わせるため逆にする
  // #define SW3_SEL_P_PIN 33
  // #define SW4_SEL_N_PIN 34
  #define SW3_SEL_P_PIN 34
  #define SW4_SEL_N_PIN 33
  //
  #define SW5_ENTER_PIN 48
  // LED
  #define LED_PIN 8

  // update from v1_2
  #define BQ27x_PIN 2
  #define SDA_PIN 3
  #define SCL_PIN 4
  #define BAT_CURRENT_PIN 5
  #define R5_BATVOL_PIN 7
  #define DETECT_ANALOG_IN_PIN 45
#endif

#if defined(GENERAL_V2)
  // Audio pins
  #define BCLK_PIN 39
  #define LRCK_PIN 38
  #define DOUT_PIN 40
  #define I2S_MLCK_PIN 48
  #define EN_I2S_DAC_PIN 33
  #define EN_MOTOR_PIN 47
  // analog amp
  #define EN_VIBAMP_PIN 26
  #define AOUT_VIBVOL_PIN 18
  // _display pins
  #define SCLK_PIN 45
  #define MOSI_PIN 46
  //   #define MISO_PIN 37
  #define OLED_DC_PIN 44
  #define OLED_RESET_PIN 43
  #define CS_PIN 42
  #define EN_OLED_PIN 2
  // Button pins
  #define SW1_VOL_P_PIN 13
  #define SW2_VOL_N_PIN 14
  //   #define SW3_ENTER_PIN 0
  // LED
  #define LED_PIN 34
  // current sensing
  #define BQ27x_PIN 37
  #define SDA_PIN 35
  #define SCL_PIN 36
#endif

#if defined(NECKLACE)
  // Audio pins
  #define BCLK_PIN 39
  #define LRCK_PIN 38
  #define DOUT_PIN 40
  #define I2S_MLCK_PIN 41
  #define EN_I2S_DAC_PIN 7
  // ピン番号はv1_3で変更予定
  #define EN_MOTOR_PIN 44
  // analog amp
  #define EN_VIBAMP_PIN 43
  #define AIN_VIBVOL_PIN 1
  #define AOUT_VIBVOL_PIN 4
  // _display pins
  #define SCLK_PIN 17
  #define MOSI_PIN 18
  #define MISO_PIN 37
  #define OLED_DC_PIN 16
  #define OLED_RESET_PIN 15
  #define CS_PIN 14
  #define EN_OLED_PIN 47
  // Button pins
  #define SW1_VOL_P_PIN 13
  #define SW2_VOL_N_PIN 12
  #define SW3_SEL_P_PIN 2
  #define SW4_SEL_N_PIN 34
  #define SW5_ENTER_PIN 21
  // LED
  #define LED_PIN 3
#endif

#endif