# Ultrasonic Distance Meter (ESP32 + HC-SR04 + 16x2 LCD)

ESP32 ile HC-SR04 ultrasonik sensörden mesafe ölçümü yapıp 16x2 LCD’de gösteren proje.
Ölçüm kararlılığı için **Median-5 filtre** ve geçersiz ölçümler için **retry** mekanizması içerir.

## Özellikler
- Mesafe ölçümü (cm)
- `NAN` tabanlı geçersiz ölçüm yönetimi
- `maxTry` kadar tekrar deneme (retry)
- 5 geçerli örnekten **Median-5** (uç değerleri bastırır)
- LCD’de sabit genişlik format (`snprintf("%6.2f")`) → karakter kalıntısı bırakmaz

## Demo
- `media/setup.jpg`
- `media/demo.gif`

## Donanım
- ESP32 DevKit (ESP32-WROOM-32)
- HC-SR04 Ultrasonic Sensor
- 16x2 LCD (HD44780) 4-bit
- Pot (10k) (LCD kontrast)
- (Önerilir) ECHO için gerilim bölücü

## Pinler

### HC-SR04 -> ESP32
| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO2 |
| ECHO | GPIO4 (bölücü önerilir) |

### LCD 16x2 -> ESP32 (4-bit)
| LCD | ESP32 |
|---|---|
| RS | GPIO14 |
| E  | GPIO13 |
| D4 | GPIO5 |
| D5 | GPIO18 |
| D6 | GPIO19 |
| D7 | GPIO21 |
| RW | GND |
| V0 | Pot orta uç |

## Algoritma
1) `MesafeCM()` ölçüm alır, echo yoksa `NAN`
2) `mesafeCM_Try(maxTry)` geçerli ölçüm için tekrar dener
3) 5 geçerli değer `g[5]` içine alınır
4) Sıralanır, median = `g[2]`
5) LCD’ye `%6.2f` formatıyla yazdırılır

## Geliştirilecekler
- `delay()` yerine `millis()` (non-blocking)
- Kayan median-5 (sliding window)
- ESP32 Web arayüz (HTTP ile canlı değer)
