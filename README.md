# Проект: Керування замком за допомогою Arduino, RFID та кнопки

## Опис

Цей проект реалізує систему керування замком за допомогою Arduino, RFID-карт, кнопки та мотора. Замок може відкриватися або закриватися залежно від стану дверей та наявності однієї з чотирьох допустимих RFID-карт або натискання кнопки. Звукова індикація використовується для підтвердження успішних та помилкових операцій.

## Компоненти

- **Arduino (наприклад, Uno, Mega, Nano)** - мікроконтролер, який керує всією системою.
- **RFID-модуль (MFRC522)** - модуль для зчитування RFID-карт.
- **Двигун (з двопровідним драйвером)** - використовується для механічного керування замком.
- **Кнопка** - для ручного керування замком.
- **Геркон (датчик дверей)** - для визначення стану дверей (відчинено/зачинено).
- **П'єзоелектричний динамік** - для звукової індикації.
- **Різні RFID-карти** - для ідентифікації користувачів.

## Підключення

- **RFID-модуль**: Підключіть відповідно до пінів Arduino (SS_PIN - 10, RST_PIN - 9).
- **Мотор**: Підключіть до пінів 4 та 5.
- **Кнопка**: Підключіть до піну 8.
- **Геркон**: Підключіть до піну 2.
- **П'єзоелектричний динамік**: Підключіть до піну 3.

## Програмне забезпечення

Код для Arduino написаний на мові C++ з використанням бібліотек **SPI.h**, **MFRC522.h** та **GyverMotor2.h**. Код дозволяє керувати замком за допомогою натискання кнопки або піднесення RFID-карти.

### Функціональність

- **Відкриття/закриття замка**: При натисканні кнопки або піднесенні однієї з чотирьох допустимих RFID-карт замок відкривається або закривається в залежності від стану дверей.
- **Звукова індикація**: Після кожної успішної операції видається короткий звуковий сигнал. Якщо піднесена карта не є допустимою, видається звуковий сигнал помилки.
- **Перевірка стану дверей**: Замок закривається тільки якщо двері зачинені, і відкривається, якщо двері також зачинені.
- автоматичне замикання замка було прибране з метою запобігання ситуації коли вийшли і забули картку
