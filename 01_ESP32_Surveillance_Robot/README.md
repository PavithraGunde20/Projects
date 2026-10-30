# ESP32 Surveillance Robot

An ESP32-based surveillance robot designed to provide mobile monitoring through a web-based interface. The robot combines a four-wheel drive chassis, motor control, servo mechanisms, and an ESP32-CAM module to create a remotely operated surveillance platform.

## Project Overview

The ESP32 Surveillance Robot is developed using an ESP32 Development Board and an ESP32-CAM module. It uses four DC motors connected to a motor driver to move the robot in different directions. Two servo motors provide additional movement for adjustable positioning of the camera or other mounted components, depending on the mechanical arrangement.

The ESP32 hosts a web-based control interface, allowing the user to access the robot's control webpage and operate its functions. The ESP32-CAM module supports camera-based monitoring.

This project demonstrates the integration of embedded systems, robotics, wireless communication, motor control, and web-based control.

## Hardware Components

The following components were used to build the robot:

| S. No. | Component | Quantity |
|---|---|---:|
| 1 | Robot chassis | 2 |
| 2 | DC motors | 4 |
| 3 | Wheels | 4 |
| 4 | Motor driver | 1 |
| 5 | ESP32 expansion board | 1 |
| 6 | ESP32 Development Board | 1 |
| 7 | Rocker switch | 1 |
| 8 | Servo motors | 2 |
| 9 | ESP32-CAM module | 1 |
| 10 | Voltage regulator | 1 |
| 11 | Three-battery socket | 1 |
| 12 | Batteries | 3 |
| 13 | Antenna | 1 |
| 14 | Jumper cables | As required |
| 15 | USB cable | 1 |

## Software and Tools

- **Arduino IDE** – Used to write, compile, and upload the ESP32 program.
- **Arduino C/C++** – Used for programming the robot's control functions.
- **ESP32 Web Interface** – Provides a webpage for remote robot operation.
- **ESP32-CAM** – Used for camera-based surveillance.

## Main Features

- Four-wheel drive movement using four DC motors.
- Motor control through a motor driver.
- Wireless operation through an ESP32-based web interface.
- Camera-based surveillance using the ESP32-CAM module.
- Two servo motors for additional controlled movement.
- Battery-powered operation.
- Integrated embedded hardware and software.

## Working Principle

1. The ESP32 Development Board acts as the main controller of the robot.
2. The ESP32 receives control commands through the web-based interface.
3. Based on the received commands, the motor driver controls the four DC motors.
4. The four wheels provide movement for the robot.
5. The two servo motors provide additional movement according to their programmed functions.
6. The ESP32-CAM module provides the camera functionality for surveillance.
7. The battery supply powers the system, while the voltage regulator provides the required regulated voltage.

## Project Images

### Front View
![Front View](Images/robot_front_view.jpg)

### Side View
![Side View](Images/robot_side_view.jpg)

### Top View
![Top View](Images/robot_top_view.jpg)

### Working Prototype
![Working Robot](Images/robot_working.jpg)


## Circuit Diagram

![Circuit Diagram](Circuit_Diagram/ESP32_Robot_Circuit.png)


## Source Code

The Arduino source code for this project is available in the following folder:

[`Code`](Code/)

The source code includes the ESP32 programming and web-based control implementation.


## Applications

- Remote surveillance and monitoring.
- Robotics and embedded systems demonstrations.
- Wireless robot control.
- Educational and engineering projects.
- Experimental mobile monitoring systems.

## Learning Outcomes

Through this project, I gained practical experience in:

- ESP32 microcontroller programming.
- DC motor and motor driver interfacing.
- Servo motor control.
- ESP32-CAM integration.
- Web-based robot control.
- Power supply and voltage regulation.
- Hardware assembly, wiring, and system integration.

## Future Improvements

- Improve the robot's mechanical design and stability.
- Add obstacle detection and avoidance.
- Improve the web-based control interface.
- Add additional monitoring and automation features.
- Optimize battery performance and operating time.

## Author

Pavithra Gunde

*This project demonstrates the practical integration of hardware, programming, and web-based control to develop a mobile surveillance robot.*
