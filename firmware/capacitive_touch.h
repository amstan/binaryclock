#pragma once

volatile unsigned int tau=0; //time it takes to discharge to vref
volatile unsigned int comparator_channel; //comparator channel
volatile unsigned char done=0;

void cb_init(unsigned char on) {
	CBCTL1 = CBON*on; //Turn on
	 CBCTL2 |= CBRSEL;             // VREF is applied to -terminal 
  CBCTL2 |= CBRS_3+CBREFL_3;    // R-ladder off; bandgap ref voltage (1.2V)
                                // supplied ref amplifier to get Vcref=2.0V (CBREFL_2) 
	__delay_cycles(1000);
	CBCTL3 = (1<<comparator_channel)*on; //disable all input buffers for all analog channels
	CBCTL0 = (CBIPEN + comparator_channel)*on;
	//CBCTL2 = CBREFL_1 + CBRSEL + CBRS_1; //2.5V ref
	
	//CBINT &= ~(CBIFG + CBIIFG);
	CBINT=CBIE*on;
}

void ta0_init(unsigned char on) {
	TA0CTL = 0;
	TA0CTL |= TASSEL_2; //Internal osc
	TA0CTL |= ID_0; //1x prescaler
	TA0CTL |= MC_2*on; //Start the timer in continous mode
	TA0CCTL0 |= CCIE; //Timer interrupt, in case it overflows
}
void __attribute__((interrupt (COMP_B_VECTOR))) capacitor_reached_tau(void) {
	CBINT &= ~(CBIFG + CBIIFG);
	tau=TA0R;
	ta0_init(0);
	cb_init(0);
	//write_ws2811_hs(white,18*3,1<<LED_DATA);
// 	__bic_SR_register_on_exit(LPM0_bits); //Wakeup capacitance_read
	done=1;
}

void __attribute__((interrupt (TIMER0_A0_VECTOR))) we_got_bored() {
	cb_init(0);
	ta0_init(0);
	tau=0xffff;
// 	__bic_SR_register_on_exit(LPM0_bits); //Wakeup capacitance_read
	done=1;
}

unsigned int capacitance_read(unsigned char channel) {
	comparator_channel=channel;
	
	//Discharge
	clear_bit(P6OUT,channel);
	set_bit(P6DIR,channel);
	
	//Charge through the paired channel
	clear_bit(P6OUT,channel^0x01);
	set_bit(P6DIR,channel^0x01);
	
	//Prepare charge
	done=0;
	ta0_init(1);
	cb_init(1);
	TA0R=0;
	set_bit(P6OUT,channel^0x01); //let channel start charging
// 	__bis_SR_register(LPM0_bits + GIE); //sleep until discharge
	while(!done);
	
	ta0_init(0);
	cb_init(0);
	
	set_bit(P6DIR,channel);
	clear_bit(P6OUT,channel^0x01);
// 	//clear_bit(P6DIR,channel^0x01); //let paired channel rest
	
	return tau;
}