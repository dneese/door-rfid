#include <SPI.h>
#include <MFRC522.h>
#include <GyverMotor2.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN); // Инициализация RFID модуля

GMotor2<DRIVER2WIRE> motor(4, 5); // Инициализация мотора на пинах 4 и 5
const int buttonPin = 8;          // Пин кнопки
const int doorSensorPin = 7;      // Пин геркона (датчика двери)
const int piezoPin = 3;           // Пин для пьезоизлучателя

bool lockState = false;           // Состояние замка (false - открыт, true - закрыт)

const String validCards[4] = {"6365C807", "0E047004794415", "04FC8FE2735980", "030449FA"}; // Список допустимых карт

void setup() {
  Serial.begin(9600);
  SPI.begin();        // Инициализация SPI для RFID модуля
  rfid.PCD_Init();    // Инициализация RFID модуля

  motor.setMinDuty(70);    // мин. ШИМ
  motor.smoothMode(0);     // плавный режим 0 (отключен)

  pinMode(buttonPin, INPUT);       // Настройка пина кнопки
  pinMode(doorSensorPin, INPUT_PULLUP); // Настройка пина геркона (подтяжка к питанию)
  pinMode(piezoPin, OUTPUT);       // Настройка пина для пьезоизлучателя
}

void loop() {
  motor.tick();

  if (digitalRead(buttonPin) == HIGH) { // Если кнопка нажата
    delay(200); // Антидребезг
    handleLockAction(); // Обработка действия по кнопке
  }

  // Проверка наличия данных от RFID модуля
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      cardUID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(rfid.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    if (isValidCard(cardUID)) {
      Serial.println("Valid card detected: " + cardUID);
      handleLockAction(); // Обработка действия по карте
    } else {
      Serial.println("Invalid card detected: " + cardUID);
      errorBeep(); // Звуковой сигнал ошибки
    }

    // Задержка перед следующей проверкой карты
    delay(1000);
  }
}

// Функция проверки допустимости карты
bool isValidCard(String cardUID) {
  for (int i = 0; i < 4; i++) {
    if (cardUID == validCards[i]) {
      return true;
    }
  }
  return false;
}

// Функция обработки действий по карте или кнопке
void handleLockAction() {
  if (digitalRead(doorSensorPin) == LOW) { // Если дверь закрыта
    if (lockState) {
      unlock(); // Открытие замка
    } else {
      lock(); // Закрытие замка
    }
  }
}

// тайминги для двойного моторчика, подбирай индивидуально, скетч test.ino
//  B1002260   back
//  F1002000   forvard
//  F070300     forvard mini

// тайминг для одинарного моторчика,

// Функция закрытия замка
void lock() {
  Serial.println("Locking the door...");
  motor.setSpeedPerc(100); // Закрытие замка
  delay(2000); // Вращаем мотор 2 секунды для закрытия замка
  motor.stop(); // Остановка
  lockState = true;
  beep(); // Звуковой сигнал успешной операции
}

// Функция открытия замка
void unlock() {
  Serial.println("Unlocking the door...");
  motor.setSpeedPerc(-100); // Открытие замка
  delay(2250); // Вращаем мотор 2 секунды для открытия замка
  motor.stop(); // Остановка
  delay(300);
  motor.setSpeedPerc(70); //возввращаю язычок замка
  delay(300);
  motor.stop(); // Остановка
  
  lockState = false;
  beep(); // Звуковой сигнал успешной операции
}

// Функция воспроизведения звукового сигнала
void beep() {
  digitalWrite(piezoPin, HIGH);
  delay(50); // Длительность звукового сигнала
  digitalWrite(piezoPin, LOW);
  delay(50); // Пауза между звуковыми сигналами
}

// Функция воспроизведения звукового сигнала ошибки
void errorBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(piezoPin, HIGH);
    delay(100); // Длительность звукового сигнала
    digitalWrite(piezoPin, LOW);
    delay(100); // Пауза между звуковыми сигналами
  }
}
