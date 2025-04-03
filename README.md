# SWheels - Smart Autonomous Voice-Controlled Wheelchair

## Overview
SWheels is an intelligent, voice-controlled wheelchair designed for individuals with partial or complete paralysis. Our primary goal is to implement a robust autopilot mode, allowing the wheelchair to navigate independently from point A to point B. Additionally, SWheels supports multiple control mechanisms, including voice commands and a joystick, ensuring ease of use. It also features automatic speed adjustment for inclines and real-time obstacle detection for enhanced safety.

## Features
### Phase 1 (Current Implementation)
- **Voice and Joystick Control**: Users can operate the wheelchair using voice commands or a joystick.
- **Automatic Speed Adjustment**: The wheelchair adjusts its speed dynamically when going uphill or downhill for safety and comfort.
- **Incline Lock Mechanism**: Ensures stability on slopes.
- **Obstacle Detection and Avoidance**: Uses ultrasonic sensors for real-time obstacle avoidance.
- **Fall Detection & Alert System**: Sends alerts in case of unexpected falls.
- **Incline Detection**: Uses the MPU6050 sensor to measure the wheelchair's angle and adjust speed accordingly.

### Phase 2 (Planned Features)
- **Autopilot Mode**: Allows the wheelchair to autonomously navigate from point A to point B.
- **Path Learning**: The system learns and remembers frequently traveled paths.

### Phase 3 (Optional Enhancements)
- **Dash Cam Facility**: Records the wheelchair’s journey for security and review purposes.

## Technical Stack
- **Hardware**:
  - Raspberry Pi 5 (for voice processing, sensor fusion, and motor control)
  - ESP32 (for real-time motor control and sensor integration, coded in Arduino IDE using C++)
  - Ultrasonic and IR sensors (for obstacle detection)
  - Web Camera (for object detection and path understanding using YOLOv11, integrated with Raspberry Pi)
  - MPU6050 (for incline detection and stability adjustments)
  - Electric motors with speed regulation
- **Software**:
  - Python (for voice command processing, AI/ML models, and system control)
  - YOLOv11 (for real-time object detection and path understanding)
  - Gemini API (for natural language understanding)
  - Flask (for the Progressive Web App interface)
  - MQTT (for communication between modules)

## How It Works
SWheels integrates multiple technologies to ensure seamless and intelligent mobility for users. Below is a breakdown of its core functionalities:

### 1. Voice and Joystick Control
- Users can issue voice commands like "move forward" or "turn left," which are processed using the **Gemini API** on Raspberry Pi.
- The joystick provides an alternative control mechanism for manual navigation.

### 2. Obstacle Detection and Avoidance
- A **web camera** captures real-time video, and **YOLOv11** detects objects such as doors, walls, and pathways.
- **Ultrasonic sensors** provide additional real-time distance measurements for close-range obstacle detection.
- The system fuses data from YOLOv11 and ultrasonic sensors to make precise navigation decisions.

### 3. Incline Detection and Speed Adjustment
- The **MPU6050 sensor** continuously monitors the wheelchair’s tilt and angle.
- If the system detects an incline or decline, it automatically adjusts motor speed for a safe and stable ride.

### 4. Autopilot Mode (Planned Feature)
- The system will use **path learning algorithms** to memorize frequently traveled routes.
- It will allow autonomous navigation from point A to point B, considering obstacles and terrain conditions.

### 5. Web Interface (PWA)
- A **Progressive Web App (PWA)** enables remote monitoring and control.
- Users can access wheelchair diagnostics, battery status, and navigation logs.

## Setup & Installation
### Hardware Setup
1. Assemble the motors, sensors, and microcontrollers as per the provided circuit diagrams.
2. Flash the ESP32 firmware using PlatformIO or Arduino IDE.
3. Set up Raspberry Pi 5 with PiOS and install the required dependencies.

## Contributing
We welcome contributions! If you'd like to contribute, follow these steps:
1. Fork the repository
2. Create a new branch (`feature-new-functionality`)
3. Commit your changes
4. Push to your branch and submit a pull request

## License
This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Contact
For any questions or collaborations, feel free to reach out:
- GitHub: [your-username](https://github.com/joshuaj03)
- LinkedIn: [Joshua Johnson](https://www.linkedin.com/in/joshua-johnson-63b560253/)

