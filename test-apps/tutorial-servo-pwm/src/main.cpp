#include "mbed.h"

constexpr uint32_t clockHz                 = 48'000'000;  // assumed
constexpr uint32_t servoHz                 = 50;          // assumed
constexpr uint32_t servoperiod_in_microsec = 1000000 / servoHz;
constexpr uint32_t deadband_in_microsec    = 2;
constexpr uint32_t ticklength_in_microsec  = deadband_in_microsec;

constexpr uint32_t servo_PSC = clockHz * ticklength_in_microsec / 1'000'000 - 1;
constexpr uint32_t servo_ARR = servoperiod_in_microsec / ticklength_in_microsec - 1;

constexpr uint32_t minpwm_in_microsec = 750;
constexpr uint32_t maxpwm_in_microsec = 2250;
constexpr uint32_t minpwm_in_ticks    = minpwm_in_microsec / ticklength_in_microsec;
constexpr uint32_t maxpwm_in_ticks    = maxpwm_in_microsec / ticklength_in_microsec;

AnalogIn potVoltageIn(PA_0);

int main() {
  float p             = 0.5f;  // position will default to 50% (180deg) on startup
  uint32_t servo_CCR2 = minpwm_in_ticks + (maxpwm_in_ticks - minpwm_in_ticks) * p;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;  // give GPIOA a clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;   // give TIM2 a clock

  GPIOA->MODER &= ~GPIO_MODER_MODER1_Msk;
  GPIOA->MODER |= GPIO_MODER_MODER1_1;  // put PA1 in alternate function mode

  TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;                    // clear output compare mode bits
  TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);  // set to 110 -> mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC2PE_Msk;                    // preload enable
  TIM2->CCER &= ~TIM_CCER_CC2P;                          // polarity
  TIM2->CCER |= TIM_CCER_CC2E;                           // capture compare enable

  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL1_Msk;
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL1_0;  // alternate function on PA1

  TIM2->PSC  = servo_PSC;
  TIM2->ARR  = servo_ARR;
  TIM2->CCR2 = servo_CCR2;

  TIM2->EGR |= TIM_EGR_UG;   // copy preload register into shadow register
  TIM2->CR1 |= TIM_CR1_CEN;  // start counting

  while (true) {
    p = potVoltageIn.read();

    servo_CCR2 = minpwm_in_ticks + (maxpwm_in_ticks - minpwm_in_ticks) * p;
    TIM2->CCR2 = servo_CCR2;
    ThisThread::sleep_for(20ms);
  }
}
