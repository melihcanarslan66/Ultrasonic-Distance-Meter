# Ultrasonic Distance Meter (ESP32 + HC-SR04 + 16x2 LCD)

**Stack:** ESP32 · C++ · PlatformIO · HC-SR04 · 16x2 LCD (HD44780, 4-bit) · Median filter · Non-blocking loop

ESP32 ile **HC-SR04** ultrasonik sensörden mesafe ölçüp **16x2 LCD**’de gösteren proje.  
Ölçüm kararlılığı için **Median filtre** ve geçersiz ölçümler için **retry + NAN yönetimi** içerir. Loop yapısı `millis()` ile **bloklamadan** çalışır.

---

## Özellikler
- Mesafe ölçümü (**cm**)
- `pulseIn(..., timeout)` ile **echo yoksa timeout** → `NAN`
- `maxTry` kadar **retry** (geçerli ölçüm bulana kadar dener)
- **Kayan pencere (ring buffer)** ile son `Median` örnekten filtreleme
- **Median-3** (uç değerleri bastırır)
- LCD’de sabit format: `snprintf("%6.2f")` → ekranda karakter artığı bırakmaz
- Ayrı periyotlar:
  - Sensör okuma: `intervalSense_t = 100 ms`
  - LCD güncelleme: `intervalLCD_t = 250 ms`

---

## Donanım
- ESP32 DevKit (ESP32-WROOM-32)
- HC-SR04 Ultrasonic Sensor
- 16x2 LCD (HD44780 uyumlu) — 4-bit mod
- 10k Pot (LCD kontrast için)
- (Önerilir) **ECHO için gerilim bölücü** (HC-SR04 ECHO 5V olabilir)

> Not: Birçok HC-SR04 modülü ECHO pininden 5V TTL verir. ESP32 GPIO **3.3V** toleranslıdır. Bu yüzden ECHO hattına gerilim bölücü kullanmak iyi pratiktir.

---

## Pinler / Bağlantılar

### HC-SR04 → ESP32
| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO2 |
| ECHO | GPIO4 *(bölücü önerilir)* |

### LCD 16x2 → ESP32 (4-bit)
| LCD | ESP32 |
|---|---|
| RS | GPIO14 |
| E  | GPIO13 |
| D4 | GPIO5 |
| D5 | GPIO18 |
| D6 | GPIO19 |
| D7 | GPIO21 |
| RW | GND |
| V0 | Pot orta uç (kontrast) |

---

## Çalışma Mantığı

### 1) Ölçüm (`MesafeCM()`)
- TRIG 10 µs tetiklenir
- `pulseIn(ECHO, HIGH, 12000)` ile echo süresi alınır  
  - **0 dönerse** (timeout) → `NAN`
- Mesafe hesabı:
  - `mesafe_cm = (sure * 0.0343) / 2`

### 2) Retry (`mesafeCM_Try(maxTry)`)
- Geçerli ölçüm gelene kadar en fazla `maxTry` kez dener
- Hepsi `NAN` ise `NAN` döndürür

### 3) Kayan pencere + Median
- Son `Median` ölçüm `win[]` dizisinde tutulur (ring buffer)
- LCD güncelleme anında:
  - `win[]` içinden `NAN` olmayanlar `temp[]` içine alınır
  - Geçerli ölçüm sayısına göre çıktı:
    - `valid == 1` → tek değer
    - `valid == 2` → ortalama
    - `valid == 3` → sıralayıp median (`temp[1]`)

---

## Parametreler (Kolay Ayar)
Kod başından:
- `const int maxTry = 3;`  → retry sayısı
- `const int Median = 3;`  → pencere boyutu (**Median-3**)
- `intervalSense_t = 100;` → sensör okuma periyodu (ms)
- `intervalLCD_t = 250;`   → LCD yenileme periyodu (ms)
- `pulseIn(..., 12000);`   → timeout (µs) ≈ 2m menzil hedefi

---

## Kurulum (PlatformIO)
1. Projeyi klonla
2. PlatformIO ile aç
3. `LiquidCrystal` kütüphanesi kurulu olsun (PlatformIO Library Manager)
4. Yükle: `Upload`
5. Serial: `115200`

---

## Ekran Çıktısı
LCD satır 1:
- `Mesafe:  12.34 cm`
- Ölçüm yoksa / geçersizse:
  - `Mesafe:---.-- cm`

---

## Geliştirilecekler
- Median değerini opsiyonel büyütme: Median-5 / Median-7 (daha stabil ama daha yavaş tepki)
- Outlier rejection ek kuralı: Örn. 0–200 cm dışını direkt `NAN` saymak
- LCD’ye ikinci satırda durum/debug (valid sayısı, NAN sayısı vb.)
- Seri porttan ham ölçüm + filtrelenmiş ölçüm loglama
