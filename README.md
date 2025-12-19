# GreenPulse

## IoT Plant Monitoring and Smart Irrigation Device

A smart IoT solution that monitors plant conditions in real-time and automates irrigation for efficient water usage. GreenPulse combines environmental sensing with intelligent watering control to keep your plants healthy while conserving water.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-1.0.0-green.svg)
[![Built with ESP32](https://img.shields.io/badge/Built%20with-ESP32-blue?logo=espressif)](https://www.espressif.com/)
[![FastAPI](https://img.shields.io/badge/API-FastAPI-green?logo=fastapi)](https://fastapi.tiangolo.com/)
[![Deployed on Pythonanywhere](https://img.shields.io/badge/API-PythonAnywhere-green?logo=pythonanywhere)](https://www.pythonanywhere.com/)

## 🌱 Features

- **Real-time Soil Monitoring**: Capacitive moisture sensor provides accurate, corrosion-resistant readings
- **Environmental Tracking**: BME280 sensor monitors temperature, humidity, and atmospheric pressure
- **OLED Display**: 0.96" screen shows current readings and system status at a glance
- **Automated Irrigation**: Smart pump control based on soil moisture thresholds
- **Portable Power**: Dual 18650 battery system with integrated charging
- **Energy Efficient**: ESP32-C6 microcontroller with low-power WiFi connectivity
- **Data Logging**: Track plant health metrics over time

## 📋 Bill of Materials

| Component | Specification | Quantity |
|-----------|--------------|----------|
| Microcontroller | ESP32-C6 | 1 |
| Soil Moisture Sensor | Capacitive Sensor | 2 |
| Environmental Sensor | BME280 (I2C) | 2 |
| Display | 0.96" SSD1306 OLED (I2C) | 1 |
| Water Pump | 5V DC Amphibious Motor | 1 |
| Pump Driver | DRV8833 Motor Driver | 1 |
| Tubing | 6/8mm, 2m length | 1 |
| Battery | 18650 Lithium-Ion | 2 |
| Charging Module | Type-C Lithium Charger | 1 |
| Power Switch | Latching Switch | 1 |
| Prototyping Board | Veroboard 9x15cm | 1 |
| Female Headers | 10x10 pins | As needed |
| Wiring | 22AWG, 5m | 1 |
| Enclosure | Custom Packaging | 1 |

## 🔧 Hardware Setup

### Pin Configuration

#### ESP32-C6 Connections
```
I2C Bus (Shared):
- SDA0: GPIO6
- SCL0: GPIO7
  - BME280 Sensor 1
  - SSD1306 OLED Display
- SDA1: GPIO8
- SCL1: GPIO9
  - BME280 Sensor 2

Analog Input:
- GPIO0: Soil Moisture Sensor (ADC) 1
- GPIO1: Soil Moisture Sensor (ADC) 2

Water Pump Control (DRV8833):
- GPIO19: AIN1 
- GPIO20: AIN2
- GPIO18: PWM Control

Power:
- 5V from battery management system
- GND (Common ground)
```

### Assembly Instructions

1. **Prepare the Veroboard**
   - Cut to size if needed (9x15cm recommended)
   - Plan component layout before soldering

2. **Mount the ESP32-C6**
   - Solder female headers for easy removal
   - Position centrally for wire routing

3. **Connect Sensors**
   - Wire BME280 and OLED to I2C bus (SDA/SCL)
   - Connect moisture sensor to analog pin
   - Use 22AWG wire for all connections

4. **Power System**
   - Wire 18650 batteries in parallel for extended runtime
   - Connect charging module with Type-C port accessible
   - Install latching switch on positive rail
   - Add voltage regulator if needed (ESP32-C6 accepts 3.3-5V)

5. **Motor Driver Setup**
   - Connect DRV8833 to ESP32 control pins
   - Wire pump to motor driver output
   - Attach tubing to pump inlet/outlet

6. **Final Assembly**
   - Mount all components in enclosure
   - Secure tubing and route for easy placement
   - Ensure OLED is visible through front panel
   - Verify all connections before power-on

## 💻 Software Setup

### Prerequisites

- Arduino IDE 2.0+
- ESP32 board support package
- Required libraries:
  - Adafruit_SSD1306
  - Adafruit_BME280
  - WiFi (ESP32 core)

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/greenpulse.git
cd greenpulse
```

2. Install dependencies:
```bash
# Using Arduino Library Manager
# Search and install: Adafruit SSD1306, Adafruit BME280
```

3. Configure WiFi credentials:
```cpp
// In config.h
#define WIFI_SSID "your-network-name"
#define WIFI_PASSWORD "your-password"
```

4. Set moisture thresholds:
```cpp
// Adjust based on your soil type
#define DRY_THRESHOLD 30    // Start watering
#define WET_THRESHOLD 60    // Stop watering
```

5. Upload to ESP32-C6:
```bash
# Select board: ESP32C6 Dev Module
# Upload via USB-C connection
```

## 🚀 Usage

### Initial Calibration

1. **Soil Moisture Calibration**
   - Insert sensor in dry soil, note reading
   - Insert sensor in saturated soil, note reading
   - Update threshold values in code

2. **Pump Testing**
   - Run manual pump test to verify flow rate
   - Adjust PWM duty cycle if needed (50-100%)
   - Check for leaks in tubing connections

### Operation Modes

**Automatic Mode** (Default)
- System monitors soil moisture continuously
- Activates pump when below dry threshold
- Deactivates when wet threshold reached
- Updates display every 5 seconds

**Manual Mode**
- Trigger irrigation via web interface
- Override automatic thresholds temporarily
- Monitor sensor readings in real-time

### Display Information

The OLED shows:
- Soil moisture percentage
- Temperature (°C)
- Humidity (%)
- Pump status (ON/OFF)
- WiFi connection status
- Battery level indicator

## 📊 Data Monitoring

Access real-time data through:
- **Web Dashboard**: `http://[ESP32-IP-ADDRESS]`
- **MQTT Broker**: Publish sensor data for home automation integration
- **Serial Monitor**: Debug output at 115200 baud

## 🔋 Power Management

- **Battery Life**: Approximately 7-14 days per charge (usage dependent)
- **Charging Time**: 4-6 hours for dual 18650 cells
- **Low Battery Warning**: Display shows alert below 20%
- **Deep Sleep Mode**: Available for extended battery life (optional)

## 🛠️ Troubleshooting

**Display not working**
- Check I2C address (usually 0x3C for SSD1306)
- Verify SDA/SCL connections
- Test with I2C scanner sketch

**Pump not activating**
- Verify DRV8833 power supply (needs 5V)
- Check GPIO pin assignments
- Test pump separately with 5V

**Inconsistent moisture readings**
- Ensure sensor is fully inserted in soil
- Clean sensor contacts with isopropyl alcohol
- Verify stable power supply to sensor

**WiFi connection issues**
- Check credentials in config.h
- Verify 2.4GHz network (ESP32-C6 doesn't support 5GHz)
- Move device closer to router during setup

## 🔐 Security Notes

- Change default WiFi credentials before deployment
- Use WPA2/WPA3 encryption on your network
- Consider adding authentication to web dashboard
- Keep firmware updated for security patches

## 📈 Future Enhancements

- [ ] Mobile app for iOS/Android
- [ ] Machine learning for optimal watering schedules
- [ ] Multi-plant support with individual sensors
- [ ] Weather API integration for smart scheduling
- [ ] Solar panel charging option
- [ ] Cloud data logging and analytics
- [ ] Nutrient level monitoring

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- Onyilimba Dubemchukwu - Firmware & Hardware Engineering

## 🙏 Acknowledgments

- Adafruit for sensor libraries
- ESP32 community for development support
- Open source contributors

## 📞 Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Email: support@greenpulse.dev
- Discord: [Project Discord Server]

---

**🌍 Environmental Impact**: By optimizing water usage, GreenPulse helps conserve this precious resource while maintaining healthy plants. Join us in building sustainable IoT solutions!
