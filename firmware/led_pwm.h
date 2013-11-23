#pragma once

void led_pwm_set(unsigned int duty) {
	set_bit(P1SEL,LED_PWM); //enable pwm on the pin
	TA0CCR0 = 64-1; //PWM Period/2
	TA0CCTL2 = OUTMOD_7; //CCR1 toggle/set
	TA0CCR2 = duty; //CCR1 PWM duty cycle
	TA0CTL = TASSEL_2 + MC_1 + TACLR; //SMCLK, up-down mode, clear TAR
}