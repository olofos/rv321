#ifndef SYSTICK_H_
#define SYSTICK_H_

extern volatile uint32_t systick;

void systick_init(void);
void delay(uint32_t millis);

void systick_pause(void);
void systick_resume(void);

#endif
