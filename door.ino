#include <SPI.h>
#include <MFRC522.h>
#include <GyverMotor2.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN); // Ініціалізація RFID модуля

GMotor2<DRIVER2WIRE> motor(4, 5); // Ініціалізація мотора на пінах 4 і 5
const int buttonPin = 8;          // Пін кнопки
const int doorSensorPin = 2;      // Пін геркона (датчика дверей)
const int piezoPin = 3;           // Пін для п'єзоелектричного динаміка

bool lockState = false;           // Стан замка (false - відкритий, true - закритий)

const String validCards[4] = {"00112233", "44556677", "8899AABB", "CCDDEEFF"}; // Список допустимих карт

void setup() {
  Serial.begin(9600);
  SPI.begin();        // Ініціалізація SPI для RFID модуля
  rfid.PCD_Init();    // Ініціалізація RFID модуля

  motor.setMinDuty(70);    // Мін. ШІМ
  motor.smoothMode(0);     // Плавний режим 0 (вимкнено)

  pinMode(buttonPin, INPUT);       // Налаштування піна кнопки як вхідного
  pinMode(doorSensorPin, INPUT_PULLUP); // Налаштування піна геркона (підтяжка до живлення)
  pinMode(piezoPin, OUTPUT);       // Налаштування піна для п'єзоелектричного динаміка як вихідного
}

void loop() {
  motor.tick();

  if (digitalRead(buttonPin) == HIGH) { // Якщо кнопка натиснута
    delay(200); // Антидребезг
    handleLockAction(); // Обробка дії за кнопкою
  }

  // Перевірка наявності даних від RFID модуля
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      cardUID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(rfid.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    if (isValidCard(cardUID)) {
      Serial.println("Valid card detected: " + cardUID);
      handleLockAction(); // Обробка дії за картою
    } else {
      Serial.println("Invalid card detected: " + cardUID);
      errorBeep(); // Звуковий сигнал помилки
    }

    // Затримка перед наступною перевіркою карти
    delay(1000);
  }
}

// Функція перевірки допустимості карти
bool isValidCard(String cardUID) {
  for (int i = 0; i < 4; i++) {
    if (cardUID == validCards[i]) {
      return true;
    }
  }
  return false;
}

// Функція обробки дій за картою або кнопкою
void handleLockAction() {
  if (digitalRead(doorSensorPin) == LOW) { // Якщо двері зачинені
    if (lockState) {
      unlock(); // Відкриття замка
    } else {
      lock(); // Закриття замка
    }
  }
}

// Функція закриття замка
void lock() {
  Serial.println("Locking the door...");
  motor.setSpeedPerc(100); // Закриття замка
  delay(2000); // Обертання мотора 2 секунди для закриття замка
  motor.stop();
  lockState = true;
  beep(); // Звуковий сигнал успішної операції
}

// Функція відкриття замка
void unlock() {
  Serial.println("Unlocking the door...");
  motor.setSpeedPerc(-100); // Відкриття замка
  delay(2000); // Обертання мотора 2 секунди для відкриття замка
  motor.stop();
  lockState = false;
  beep(); // Звуковий сигнал успішної операції
}

// Функція відтворення звукового сигналу
void beep() {
  digitalWrite(piezoPin, HIGH);
  delay(50); // Тривалість звукового сигналу
  digitalWrite(piezoPin, LOW);
  delay(50); // Пауза між звуковими сигналами
}

// Функція відтворення звукового сигналу помилки
void errorBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(piezoPin, HIGH);
    delay(100); // Тривалість звукового сигналу
    digitalWrite(piezoPin, LOW);
    delay(100); // Пауза між звуковими сигналами
  }
}
