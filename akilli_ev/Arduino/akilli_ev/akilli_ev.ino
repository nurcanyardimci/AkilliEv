#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define sifreUzunluk  5

char data[sifreUzunluk];
char sifre[sifreUzunluk] = "0000";
byte data_count = 0, sifre_count = 0;
int sifreDenemeSayisi = 0;

char customKey;
const byte ROWS = 4;
const byte COLS = 3;

bool girisDurum = false; 
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {25, 26, 27, 28};
byte colPins[COLS] = {24, 23, 22};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int hareketSensoru = 7;
int sesSensoru = 4;
int sicaklikSensoru = A0;
int ldr = A1;
int toprakNemSensoru = A3;
int depremSensoru = A4;
int odaIsigi = 5;
int buzzer = 2;
int klima = A2;
int panikButon = A5;

Servo servom;

void setup() {
  lcd.begin(16, 2);
  pinMode(hareketSensoru, INPUT);
  pinMode(sesSensoru, INPUT);
  pinMode(sicaklikSensoru, INPUT);
  pinMode(ldr, INPUT);
  pinMode(toprakNemSensoru, INPUT);
  pinMode(depremSensoru, INPUT);
  pinMode(odaIsigi, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(klima, OUTPUT);
  servom.attach(3);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("EVE HOSGELDINIZ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PAROLA :");
}

void loop() {
  int acilButon = digitalRead(panikButon);
  int hareket = digitalRead(hareketSensoru);
  int ses = digitalRead(sesSensoru);
  int sicaklik = analogRead(sicaklikSensoru) / 2 ;
  int titresim = analogRead(depremSensoru);
  int topraknem = digitalRead(toprakNemSensoru);
  int ldrDurum = analogRead(ldr);

  if (titresim > 500) {
    digitalWrite(buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  DIKKAT!    ");
    lcd.setCursor(0, 1);
    lcd.print(" DEPREM OLUYOR!   ");
    delay(1000);
  } else {
    digitalWrite(buzzer, LOW);
    lcd.clear();
  }  
  if (girisDurum == false)
  {
    if (hareket == HIGH) {
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" IZINSIZ GIRIS! ");
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      lcd.clear();
    }
    if (ses == HIGH) {
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      lcd.setCursor(0, 0);
      lcd.print(" SES ALGILANDI ");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" IZINSIZ GIRIS! ");
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      lcd.clear();
    }
    sifreGiris();
  }
  else {
    if (hareket == HIGH) { 
      digitalWrite(odaIsigi, HIGH);
    } else {
      digitalWrite(odaIsigi, LOW);
    }
    if (topraknem == HIGH) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  NEM MIKTARI AZ  ");
      lcd.setCursor(0, 1);
      lcd.print("CICEGI SULAYINIZ! ");
      delay(1000);
      lcd.clear();
    }
    if (acilButon == HIGH) {
      digitalWrite(buzzer, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  ACIL DURUM   ");
      lcd.setCursor(0, 1);
      lcd.print(" 112 ARANIYOR! ");
      delay(1000);
      lcd.clear();
    }
    if (sicaklik > 30) {
      digitalWrite(klima, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("SICAKLIK YUKSEK");
      lcd.setCursor(0, 1);
      lcd.print("KLIMA CALISTI");
      delay(1000);
      lcd.clear();
    }
    else {
      digitalWrite(klima, LOW);
    }
    if (ldrDurum > 710) {
      servom.write(90);
    }
    else {
      servom.write(180);
    }
  }
}
void sifreGiris() {
  while (girisDurum == false)
  {
    lcd.setCursor(0, 0);
    lcd.print("PAROLA: ");
    customKey = customKeypad.getKey();
    if (customKey) {
      data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print(data[data_count]);
      data_count++;
    }
    if (data_count == sifreUzunluk - 1) {
      lcd.clear();
      if (!strcmp(data, sifre)) {
        lcd.print(" PAROLA DOGRU");
        girisDurum = true;
        sifreDenemeSayisi=0;
        delay(500);
      }
      else {
        if (sifreDenemeSayisi >= 2)
        {
          digitalWrite(buzzer, HIGH);
          delay(250);
          digitalWrite(buzzer, LOW);
          lcd.setCursor(0, 0);
          lcd.print("IZINSIZ GIRIS!");
          lcd.setCursor(0, 1);
          lcd.print("155 ARANIYOR...");
          delay(1000);
          return;
        }
        lcd.setCursor(0, 0);
        lcd.print("PAROLA YANLIS");
        lcd.setCursor(0, 1);
        lcd.print("TEKRAR DENEYINIZ");
        sifreDenemeSayisi++;
        delay(500);
      }
      lcd.clear();
      clearData();
    }
  }
}
void clearData() {
  while (data_count != 0) {
    data[data_count--] = 0;
  }
  return;
}
