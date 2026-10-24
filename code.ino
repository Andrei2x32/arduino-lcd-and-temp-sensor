#include <Wire.h>
#include <math.h>
#include<LiquidCrystal.h>
const int tmp = A0;
volatile int ore = 18;
volatile int minute = 9;
volatile byte secunde = 3;
byte ultima_secunda = -1;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //creare obiect de tip LiquidCrystal
//ordinea este: RS, E, DB4, DB5, DB6, DB7
void setup()
{
  	Serial.begin(9600);
  	Wire.begin();
  lcd.begin(16, 2);
  // Print a message to the LCD.
  ADCSRA = B10000111; // ADEN=1 (ADC activat), biții 2..0=111 (Prescaler 128)
  ADMUX = B00000000;
  ADMUX = B01000000;  // REFS0=1 (Referință 5V), MUX=0000 (Canal A0) 
  
  cli();
  TCCR1A = 0; // Reset timer1 control reg A
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 62499;
  // 3. Setarea modului CTC și a Prescaler-ului la 256 (folosind doar =)
  TCCR1B = (1 << WGM12) | (1 << CS12); 
  // 4. Activarea întreruperii
  TIMSK1 = (1 << OCIE1A); 
  
  sei(); // Repornește întreruperile globale
}
void loop()
{
  byte s=secunde;
  if(s!= ultima_secunda)
  {
   	ultima_secunda = secunde;
    ADCSRA |= B01000000;
    while(bit_is_set(ADCSRA,ADSC));
    int reading = ADCL | (ADCH << 8);
    float voltage = (reading * 5.0) / 1024.0;
    float temperatureC = (voltage - 0.5) * 100 ;

    //afisarea pe seriala
    Serial.println(reading);
    Serial.println(temperatureC);

    //afisarea pe display lcd
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(temperatureC);
    lcd.setCursor(0,1);
    lcd.print("Time:");
    lcd.print(ore);
    lcd.print(":");
    lcd.print(minute);
    lcd.print(":");
    if (secunde < 10) lcd.print('0');
	lcd.print(secunde);
  }
 

}

ISR(TIMER1_COMPA_vect)
{
  secunde++;
  if (secunde >= 60) {
    secunde = 0;
    minute++;
    if (minute >= 60) {
      minute = 0;
      ore++;
      if (ore >= 24) ore = 0;
    }
  }
}