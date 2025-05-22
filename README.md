# 🌞 Solar Panel Defect Detection and Monitoring System

An IoT-based system designed to detect and monitor faults in solar photovoltaic (PV) panels using real-time sensor data and cloud integration. This project improves solar panel performance by identifying issues like dust accumulation, overheating, and electrical anomalies.

## 📌 Features

- Real-time monitoring of:
  - Ambient & panel temperature
  - Humidity
  - Voltage & current
  - Light intensity
  - Dust levels
- Fuzzy logic-based fault detection
- LCD display for on-site monitoring
- Buzzer alerts for critical conditions
- Data upload to ThingSpeak cloud
- High accuracy (97%+) fault detection
- Cost-effective and scalable solution

## 🛠️ Technologies Used

- **Microcontroller:** ESP32 WROOM-32
- **Sensors:**
  - DS18B20 – Panel Temperature
  - DHT22 – Ambient Temperature & Humidity
  - INA219 – Voltage and Current
  - BH1750 – Light Intensity
  - GP2Y1010AU0F – Dust Detection
- **Display:** 16x2 I2C LCD
- **Cloud:** ThingSpeak
- **Language:** Embedded C (Arduino IDE)

## 📡 Working Principle

1. Sensors gather real-time data from the solar panel and environment.
2. ESP32 processes this data and calculates power efficiency.
3. A fuzzy logic algorithm evaluates the fault probability.
4. Fault status and system efficiency are shown on the LCD.
5. Alerts are triggered via a buzzer for critical faults.
6. Data is sent to ThingSpeak for remote monitoring and analysis.

## 📈 Applications

- Residential solar installations
- Commercial & industrial solar farms
- Remote monitoring stations
- Green energy systems in smart cities

## 🚀 Future Enhancements

- Automatic panel cleaning system based on dust level
- Mobile app integration for fault alerts
- Predictive maintenance using machine learning
- Solar tracking system integration
