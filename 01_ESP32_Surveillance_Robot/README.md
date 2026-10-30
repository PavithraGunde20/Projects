# ESP32 Surveillance Rover

A Wi-Fi-controlled, four-wheel-drive surveillance robot developed using ESP32, ESP32-CAM, DC motors, servo motors, and a web-based control interface.

## 1. Project Overview

The ESP32 Surveillance Rover is a mobile robotic platform designed for remote movement and camera-based monitoring. It uses an ESP32 Development Board as the main controller and an ESP32-CAM module for video surveillance.

The robot consists of four DC geared motors connected through an L298N motor driver. Two servo motors provide pan-and-tilt movement for camera positioning. The robot can be controlled through a web-based interface hosted by the ESP32.

The project integrates robotics, embedded systems, wireless communication, motor control, camera interfacing, and web-based operation.

## 2. Hardware Components

| S. No. | Component | Quantity |
|---|---|---:|
| 1 | Robot chassis | 2 |
| 2 | DC geared motors | 4 |
| 3 | Wheels | 4 |
| 4 | L298N motor driver | 1 |
| 5 | ESP32 expansion board | 1 |
| 6 | ESP32 Development Board | 1 |
| 7 | Rocker switch | 1 |
| 8 | Servo motors | 2 |
| 9 | ESP32-CAM module | 1 |
| 10 | Voltage regulator / buck converter | 1 |
| 11 | Three-battery socket | 1 |
| 12 | Batteries (18650) | 3 |
| 13 | Antenna | 1 |
| 14 | Jumper cables | As required |
| 15 | USB cable | 1 |

## 3. Software and Development Tools

- **Arduino IDE:** Used to write, compile, and upload the ESP32 program.
- **Arduino C/C++:** Used to program the robot's control functions.
- **ESP32 Web Interface:** Provides a browser-based page for remote robot control.
- **ESP32-CAM:** Provides camera functionality and video streaming.

## 4. Main Features

- Four-wheel-drive robotic movement.
- Wireless control through an ESP32-hosted webpage.
- Camera-based surveillance using ESP32-CAM.
- Two servo motors for pan-and-tilt camera positioning.
- Independent PWM speed control for the left and right motor groups.
- Battery-powered operation.
- Integrated motor, camera, and servo control.

## 5. Working Principle

1. The ESP32 Development Board acts as the main controller.
2. The user accesses the robot's web-based control interface.
3. Control commands are processed by the ESP32.
4. The L298N motor driver controls the four DC motors according to the received commands.
5. The two servo motors control the pan and tilt movements.
6. The ESP32-CAM provides video streaming for remote monitoring.
7. The battery pack supplies power to the motor driver and the regulated power circuit supplies the electronics.

## 6. ESP32 GPIO Pin Configuration

The following pin assignments correspond to the `SurveillanceRobot.ino` program.

### Motor Driver Connections

| ESP32 GPIO | Board Label | L298N Pin | Function |
|---|---|---|---|
| GPIO 25 | D25 | ENA | Left motor PWM / speed |
| GPIO 26 | D26 | IN1 | Left motor direction |
| GPIO 27 | D27 | IN2 | Left motor direction |
| GPIO 14 | D14 | IN3 | Right motor direction |
| GPIO 13 | D13 | IN4 | Right motor direction |
| GPIO 33 | D33 | ENB | Right motor PWM / speed |
| GND | GND | GND | Common ground |

The motor speed is controlled through ENA and ENB using PWM values in the range of 0–255.

### Servo Motor Connections

| Servo Motor | ESP32 GPIO | Board Label | Function |
|---|---|---|---|
| Pan Servo | GPIO 18 | D18 | Horizontal camera movement |
| Tilt Servo | GPIO 19 | D19 | Vertical camera movement |

## 7. L298N Motor Driver and Motor Connections

The four DC geared motors are arranged into two motor groups:

| Motor Group | L298N Output | Motor Arrangement |
|---|---|---|
| Left side | OUT1 and OUT2 | Two left-side motors |
| Right side | OUT3 and OUT4 | Two right-side motors |

The two motors on each side are connected in parallel to their respective motor-driver output pair.

```text
             L298N MOTOR DRIVER

       OUT1 ─── Left Motor 1 ─── OUT2
         │                         │
         └──── Left Motor 2 ───────┘


       OUT3 ─── Right Motor 1 ─── OUT4
         │                         │
         └──── Right Motor 2 ─────┘
```

**Note:** Check the motor driver's current capacity before connecting two motors in parallel. If a motor rotates in the opposite direction, reverse its two motor wires.

## 8. Power Supply and Distribution

The robot uses a three-cell 18650 battery pack as its main power source.

The recommended power arrangement separates the motor supply from the regulated electronics supply.

### Power Connections

| From | Connection | To | Purpose |
|---|---|---|---|
| Battery positive | Through rocker switch | L298N motor supply (+12V/VS) | Motor power |
| Battery positive | Through rocker switch | Buck converter IN+ | Regulator input |
| Battery negative | Direct | L298N GND | Motor ground |
| Battery negative | Direct | Buck converter IN- | Regulator ground |
| Buck converter OUT+ | Regulated 5V | ESP32 expansion board 5V | ESP32 power |
| Buck converter OUT- | Ground | ESP32 expansion board GND | ESP32 ground |
| Buck converter OUT+ | Regulated 5V | Pan servo VCC | Servo power |
| Buck converter OUT+ | Regulated 5V | Tilt servo VCC | Servo power |
| Buck converter OUT- | Ground | Both servo GND pins | Servo ground |
| Regulated 5V supply | 5V | ESP32-CAM 5V | Camera power |
| Regulated ground | GND | ESP32-CAM GND | Camera ground |

### Power Supply Diagram

```text
             3 × 18650 BATTERY PACK
                       |
                 ROCKER SWITCH
                       |
              +--------+--------+
              |                 |
              |                 |
              v                 v
         L298N MOTOR       BUCK CONVERTER
         DRIVER INPUT      INPUT (+ and -)
              |                 |
         FOUR DC MOTORS         |
                                |
                         REGULATED 5V
                                |
                    +-----------+-----------+
                    |           |           |
                    v           v           v
                 ESP32       SERVO       ESP32-CAM
               EXPANSION    MOTORS       MODULE
                 BOARD
```

### Important Power Safety Notes

- Confirm the battery pack configuration and voltage before connecting it.
- If the three 18650 cells are connected in series (3S), the pack is nominally 11.1V and can reach 12.6V when fully charged.
- The L298N motor supply must be compatible with the motor voltage and battery voltage.
- Set the buck converter output to exactly 5V before connecting the ESP32, servos, or ESP32-CAM.
- Do not connect the battery's full voltage directly to the ESP32 5V input.
- Ensure that the buck converter can provide sufficient current for the ESP32, camera, and both servos, including their peak current demand.
- All grounds must be connected together.
- Use an appropriately rated battery protection and charging arrangement.

### Common Ground Connections

The following components must share a common ground:

```text
Battery Negative
      |
      +---- Buck Converter IN-
      |
      +---- Buck Converter OUT-
      |
      +---- L298N GND
      |
      +---- ESP32 GND
      |
      +---- Pan Servo GND
      |
      +---- Tilt Servo GND
      |
      +---- ESP32-CAM GND
```

## 9. ESP32-CAM Connections

The ESP32-CAM module is used for camera-based monitoring.

| ESP32-CAM Pin | Power Connection |
|---|---|
| 5V | Regulated 5V supply |
| GND | Common ground |

The camera stream URL follows this format:

```text
http://CAMERA_IP:81/stream
```

Replace `CAMERA_IP` with the actual IP address assigned to the ESP32-CAM.

The ESP32-CAM must be configured with suitable camera firmware and network settings for the stream to work.

## 10. Web-Based Control Interface

The project includes a web-based control interface in the ESP32 program.

The user can connect to the robot's control webpage using a compatible web browser. The webpage provides the interface for sending control commands to the robot.

The ESP32 processes the commands and controls the motor driver and servo motors accordingly.

The ESP32-CAM provides the camera stream separately, using the configured camera IP address.

## 11. Source Code

The Arduino source code for this project is available in the following folder:

[`Code/`](Code/)

Main program:

[`SurveillanceRobot.ino`](Code/SurveillanceRobot.ino)

The source code includes the ESP32 programming and web-based control implementation.

## 12. Project Images

### Front View
![Front View](Images/robot_front_view.jpg)

### Side View
![Side View](Images/robot_side_view.jpg)

### Top View
![Top View](Images/robot_top_view.jpg)

### Working Prototype
![Working Robot](Images/robot_working.jpg)

## 13. Circuit Diagram

![Circuit Diagram](Circuit_Diagram/ESP32_Robot_Circuit.png)

## 14. Applications

- Remote surveillance and monitoring.
- Mobile robotic platforms.
- Wireless robot operation.
- Embedded systems demonstrations.
- Robotics and automation projects.
- Educational and engineering applications.

## 15. Future Improvements

- Add obstacle detection and automatic avoidance.
- Improve camera streaming and monitoring.
- Enhance the web control interface.
- Add battery-level monitoring.
- Improve chassis stability and mechanical design.
- Add additional autonomous navigation features.

## 16. Learning Outcomes

This project provided practical experience in:

- ESP32 microcontroller programming.
- Arduino IDE development.
- DC motor and L298N motor driver interfacing.
- PWM-based motor speed control.
- Servo motor interfacing.
- ESP32-CAM integration.
- Web-based wireless robot control.
- Battery power distribution and voltage regulation.
- Hardware assembly and system integration.

## 17. Author
Pavithra Gunde

*This project demonstrates the practical integration of hardware, programming, and web-based control to develop a mobile surveillance robot.*
