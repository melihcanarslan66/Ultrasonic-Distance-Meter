#include <Arduino.h>
#include <LiquidCrystal.h>
#include <math.h> //ne olur ne olmaz "NAN" için ekledik.

//Pinleri tanımladık.
const int TRIG_PIN = 2; //HC-SR04
const int ECHO_PIN = 4; //HC-SR04
const int RS = 14;      //LCD ekran
const int E = 13;       //LCD ekran
const int D4 = 5;       //LCD ekran
const int D5 = 18;      //LCD ekran
const int D6 = 19;      //LCD ekran
const int D7 = 21;      //LCD ekran
LiquidCrystal lcd (RS, E, D4, D5 , D6, D7); //LCD ekran

// Global değerler //
unsigned long lastLCD_t = 0;
const unsigned long intervalLCD_t = 250;
unsigned long lastSense_t = 0;
const unsigned long intervalSense_t = 100;

const int maxTry = 3; //Duruma bağlı değiştirilebilir.
const int Median = 3; //Duruma bağlı değiştirilebilir.
float win[Median];
float temp[Median];
char buf[10];
int count = 0;

void setup() {
  Serial.begin(115200);

  //HC-SR04 pinlerinin görevlerini tanımladık.
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //LCD ekranının değişmeyen yazılarını hazırladık.
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Mesafe:");
  lcd.setCursor(14, 0);
  lcd.print("cm");
}

float MesafeCM() { //mesafeyi ölçüyor cm cinsinden.

  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);


  unsigned long sure = pulseIn(ECHO_PIN, HIGH, 12000); //yaklaşık 2m menzil.

  if (sure == 0) {
    return NAN;
  }

  float mesafe = (sure * 0.0343f) / 2.0f;
  return mesafe;
}

float mesafeCM_Try(int maxTry){ //geçerli sonuç alana kadar deniyor ve 5 deneme sonunda geçerli sonuç alamazsa "NAN" döndürüyor.

  for (int t = 0; t < maxTry; t++)
  {
    float x = MesafeCM();

    if(!isnan(x)) return x;
  }
  return NAN;
}

void sort(float temp[]) { //küçükten büyüğe sıralıyor.

  for (int i = 1; i < Median; i++) {

    float key = temp[i];
    int j = i - 1;

    while (j >= 0 && temp[j] > key) {

      temp[j + 1] = temp[j];
      j--;
    }

    temp[j + 1] = key;
  }
}

void loop() {

  unsigned long now = millis();

  if (now-lastSense_t >= intervalSense_t) // Sensörü zamanı geldiğinde çalıştırıyor.
  {

    float m = mesafeCM_Try(maxTry);

    win[count]=m;
    count=(count + 1) % Median;

    lastSense_t = now;
  }

  if (now-lastLCD_t >= intervalLCD_t) // Ekranı zamanı geldiğinde güncelliyor.
  {
    int valid = 0;
    lcd.setCursor(7, 0);

    for (int i = 0; i < Median; i++) //Kayan değerlerimizi(win) geçerliyse geçici(temp) değere kopyaladık.
    {
      if (!isnan(win[i]))
      {
        temp[valid]=win[i];
        valid++;
      }
      
    }

    if(valid == 0) //Eğer hepsini NAN alıyorsak "---.--" yazdırdık.
    { 
      lcd.print("---.--");
    }

    if (valid == 1)
    {
      snprintf(buf, sizeof(buf), "%6.2f", temp[0]);
      lcd.print(buf);
    }
    
    if (valid == 2)
    {
      snprintf(buf, sizeof(buf), "%6.2f", (temp[0]+temp[1])/2);
      lcd.print(buf);
    }
    
    if (valid == 3) // 6 karakterlik yer ayırdık ve "."dan sonra 2 hane olacak şekilde yazdırdık. 
    {
      sort(temp);

      snprintf(buf, sizeof(buf), "%6.2f", temp[Median/2]);
      lcd.print(buf);
    }

    lastLCD_t = now;
  }
  
}