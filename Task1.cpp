#include "mbed.h"
Serial pc(SERIAL_TX, SERIAL_RX);
//green blue red green blue red (Done)
//Use ticker function to call the blinking function (Done)
//Define the blinking function (Done)
//Use interrupt function to call RecordColour function
//Record colour function helps to check what colour is on and append to the list
//Once N is reached, call the NewBlinkingFunction

// Green LED
DigitalOut led1(LED1);
// Blue LED
DigitalOut led2(LED2);
// Red LED
DigitalOut led3(LED3);
InterruptIn button(USER_BUTTON);
Timeout button_debounce_timeout;

float debounce_time_interval = 1;
const int N = 3;
const int N1 = 4;
int colour;
int t=0;
int t1=0;
int i=0;
int led_cycle[N]={1,2,3};
int blink_time_interval = 1;
int final_led_cycle[N1]={};
bool exceed = false;
Ticker blink_ticker;

void onButtonStopDebouncing(void);



void onButtonPress()
{
    if (led1 == true) {
        final_led_cycle[i]=1;
    }

    else if (led2 == true) {
        final_led_cycle[i]=2;
    }

    else if (led3 == true) {
        final_led_cycle[i]=3;
    }  
    button.rise(NULL);
    button_debounce_timeout.attach(onButtonStopDebouncing, debounce_time_interval); 
    i++; 
}   

void onButtonStopDebouncing(void)
{
        button.rise(onButtonPress);
}
    

void select_led()
{
    if (i < N1){
    t=(t+1)%N;
        if (led_cycle[t]==1) {
                // pc.printf("Blink1\r\n");
                led1 = true;
                led2 = false;
                led3 = false;
        }
        else if (led_cycle[t]==2) {
                // pc.printf("Blink2\r\n");
                led1 = false;
                led2 = true;
                led3 = false;
        }
        else if (led_cycle[t]==3) {
                // pc.printf("Blink3\r\n");
                led1 = false;
                led2 = false;
                led3 = true;
        }
    }
    else if (i >= 4){
        t1=(t1+1)%N1;
        if (final_led_cycle[t1]==1) {
                led1 = true;
                led2 = false;
                led3 = false;
        }
        else if (final_led_cycle[t1]==2) {
                led1 = false;
                led2 = true;
                led3 = false;
        }
        else if (final_led_cycle[t1]==3) {
                led1 = false;
                led2 = false;
                led3 = true;
        }
    }
}

int main() {
    // pc.baud(9600);
    // pc.printf("Starts!\r\n");
    
    blink_ticker.attach(&select_led, blink_time_interval);
    button.rise(onButtonPress);
        }
