#include <LiquidCrystal.h>
#include <math.h>
#define aref_voltage 5


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int tempPinAir = 0;        //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int tempPinDrink = 1;
int tempReadingAir;        // the analog reading from the sensor
int tempReadingDrink;
float T0 = 0;
int optimalTemp = 85;    // 136F is the optimal drinking temperature; occurs when air above coffee is at about 85F
int unsafeTemp = 110;     //drinking beverages above 160F can cause scalding or burning; occurs at about 110F air temperature
float timeToOptimal;

int peltier = 6; //The N-Channel MOSFET is on digital pin 6
int power = 0; //Power level fro 0 to 99%
int peltier_level = map(power, 0, 99, 0, 255); //This is a value from 0 to 255 that actually controls the MOSFET
int pot = 0;

void setup(void) {
  Serial.begin(9600);   
  lcd.begin(16, 2);
  pinMode(pot, INPUT);
  //lcd.display();
}
 
 
void loop(void) {
  
  delay(3000);
  
  int airTemp = analogRead(tempPinAir);
  int drinkTemp = analogRead(tempPinDrink);

  float airTempF = fahrenheit(airTemp);
  float drinkTempF = fahrenheit(drinkTemp);
  //Serial.println(drinkTempF);

  Serial.print("Drink = "); Serial.println(drinkTempF);

  //Use potentiometer data to adjust the power level of the Peltier Device
  int potValue = analogRead(pot);
  power = potValue/10.24;

  if(power > 99) power = 99;
  if(power < 0) power = 0;
  Serial.print(potValue); Serial.println(" ");
  Serial.print("Peltier power = ");Serial.print(power);Serial.println("%");

  analogWrite(peltier, peltier_level); //Write this new value out to the port
  ////////////////////////////////////////////////////////////////////////

  if(T0 == 0){    //Write a new value to T0 only if T0 has not been set
    T0 = drinkTempF;
  }

  int optimalTime = newton(T0, airTempF);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Beverage: ");lcd.print(drinkTempF);lcd.print(" deg");
  lcd.setCursor(0,1);
  lcd.print("Peltier: ");lcd.print(power);lcd.print("%");

}

//not used in this module but could be useful for further improvements!
float newton(float T0, float airTempF) { 
  int calTempAnalog = analogRead(tempPinDrink); 
  float calTime = millis()/1000.0;    //20 seconds for calibration
  float calTempF = fahrenheit(calTempAnalog);
  float k = - log((calTempF - airTempF)/(T0 - airTempF)) / calTime;
  if (k == 0){
    return 0.0;           //this will indicate to the main loop that the desired temperature has been reached
  }
  //Serial.print("calTempF "); Serial.println(calTempF);
  //Serial.print("T0 "); Serial.println(T0);
  float optimalTime = - log((optimalTemp - airTempF)/(T0 - airTempF)) / k;
  optimalTime = int (optimalTime);
  return optimalTime;
}
/////////////////////////////////////////////////////////////////

float fahrenheit(int analog) {    //converts analog input from temp sensor to degrees F
  float voltage = analog * aref_voltage;
  voltage /= 1024.0;
  float tempC = (voltage - 0.5) * 100;
  float tempF = (tempC * 9.0/5.0) + 32.0;
  return tempF;
}

/*void power(){

delay(500);
}*/

