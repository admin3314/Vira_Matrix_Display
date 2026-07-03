# 🚀 Vira_Matrix_Library

Advanced 8x8 LED Matrix Library for ESP8266 / ESP32  
Developed by **Mostafa Mir Mosavi (@viramedar)**

---

## 📌 Overview

**Vira_Matrix_Library** is a lightweight yet powerful Arduino library designed for controlling **8x8 LED matrix displays**.

It provides:
- Emoji animation engine 😊
- Built-in 5x7 font (English + partial Persian support) 🔤
- Real-time WebSocket control 🌐
- Web-based drawing interface ✏️
- Smooth scrolling text system 🌀

This library is optimized for **ESP8266 performance constraints** while keeping the API simple and expandable.

---

## ✨ Features

- 🎮 8x8 LED Matrix control (74HC595 compatible)
- 😊 Predefined emoji animations (smile, heart, spinner, etc.)
- 🔤 5x7 pixel font rendering system
- 🌀 Smooth scrolling text engine
- 🌐 WebSocket real-time communication
- ✏️ Browser-based pixel drawing interface
- ⚡ Lightweight and fast execution
- 🔧 Designed for future scalability (width/height support ready)

---

## 📦 Installation

### Arduino IDE

1. Download or clone this repository
2. Copy folder to:

Documents/Arduino/libraries/

OR
3. Use:

Sketch → Include Library → Add .ZIP Library


---

## ⚙️ Basic Example

```cpp
#include "Vira_Matrix.h"

ViraMatrix matrix(14, 13, 12, 8, 8);

void setup() {
matrix.begin();
}

void loop() {
matrix.setPixel(3, 3, true);
matrix.refresh();
}
😊 Emoji Animation Example
runSmileAnimation();
matrix.refresh();
🌐 Web Control Example (ESP8266)

Upload the provided example:

examples/BasicDisplay/BasicDisplay.ino

Then open your browser:

http://<device-ip>
Web Features:
🎨 Draw pixels directly from browser
💬 Send text to matrix
🎞 Run animations (smile, heart, spinner, etc.)
⚡ Real-time updates via WebSocket
🧠 Architecture Notes
Designed for 8x8 LED matrix systems
Frame buffer is currently fixed-size for performance
width/height parameters are reserved for future multi-matrix support
Optimized for ESP8266 memory constraints
📁 Project Structure
Vira_Matrix_Library/
│
├── src/
│   ├── Vira_Matrix.cpp
│   ├── Vira_Matrix.h
│   ├── Vira_Emoji.cpp
│   ├── Vira_Emoji.h
│   ├── Vira_Font5x7.h
│   ├── Vira_Utils.h
│
├── examples/
│   └── BasicDisplay/
│
├── library.properties
├── README.md
├── LICENSE
👨‍💻 Author

Mostafa Mir Mosavi (@viramedar)

GitHub: https://github.com/admin3314
YouTube: https://youtube.com/@viramedar
Instagram: https://instagram.com/viramedar
Telegram Channel: @viramedar
Personal Telegram: @viraep
📚 Tutorials & Files

📺 Full tutorials available on YouTube:
https://youtube.com/@viramedar

📦 PCB layouts, schematics, and project files:
Available in Telegram channel: @viramedar

⚠️ Notes
This library is optimized for 8x8 LED matrix hardware
Performance tuned for ESP8266 devices
Designed as a base for future multi-panel expansion
📜 License

MIT License