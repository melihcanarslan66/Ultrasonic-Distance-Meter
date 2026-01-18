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


  unsigned long sure = pulseIn(ECHO_PIN, HIGH, 30000); //yaklaşık 5m menzil.

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
    delay(25);
  }
  return NAN;
}

void sort(float g[]) { //küçükten büyüğe sıralıyor.

  for (int i = 1; i < 5; i++) {

    float key = g[i];
    int j = i - 1;

    while (j >= 0 && g[j] > key) {

      g[j + 1] = g[j];
      j--;
    }

    g[j + 1] = key;
  }
}

void loop() {

  lcd.setCursor(7, 0);

  float g[5];
  const int maxTry=5;

  for (int i = 0; i < 5; i++){ //Ölçülen değeri sırasıyla g[] dizisine tanımladık.

    float m=mesafeCM_Try(maxTry);

    if(isnan(m)){          //
      lcd.print("---.--"); // Eğer 5 deneme sonunda NAN alıyorsak "---.--" yazdırdık.
      return;              //
    }

    g[i]=m;
  }

  sort(g);

  char buf[10];                              //
  snprintf(buf, sizeof(buf), "%6.2f", g[2]); // 6 karakterlik yer ayırdık ve "."dan sonra 2 hane olacak şekilde yazdırdık. 
  lcd.print(buf);                            //

  delay(100);
}