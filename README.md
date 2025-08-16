# robo-rail-football-game
Robo Rail Football Game

📌 Overview

The Robo Rail Football Game is an innovative, sensor-driven tabletop football system that redefines conventional play by blending physical interactivity with modern automation and embedded technology.

Designed for two players, the game allows participants to control motorized kicking arms to shoot and defend, while smart sensors and microcontrollers handle scoring, ball detection, and automation. A web application powered by ESP32 ensures that live scores are displayed online, bridging the gap between physical gameplay and digital connectivity.

This project not only delivers an immersive gaming experience but also serves as an educational platform to demonstrate concepts in robotics, real-time systems, sensor-actuator coordination, and embedded programming.


🎮 Key Features

Automated Scoring System – LDR sensors detect goals with high accuracy.

Ball Tracking – Ultrasonic sensors monitor ball position and detect stagnation.

Automated Side-Kicking Arms – Servo-controlled arms kick the ball back into play when stuck.

Player Control – Joystick-driven DC motors for attacking and defending actions.

Live Score Display – Dual OLED screens show real-time scores.

Web Application – ESP32 sends live scores to an online scoreboard for remote monitoring.

Buzzer Feedback – Audio alert whenever a goal is scored, adding excitement.

Seamless Gameplay – Automated resets and real-time feedback ensure uninterrupted fun.


⚡Components Used

Microcontrollers:

Arduino Mega 2560 (main controller)

ESP32 (data communication + web integration)

Sensors:

LDRs (Light Dependent Resistors) for goal detection

Ultrasonic Sensors (HC-SR04) for ball tracking

Actuators:

Servo Motors (side-kicking arms)

DC Motors (horizontal player movement)

Stepper Motor + TB6600 Driver (precise vertical movement)

Drivers:

L298N Motor Driver (for DC motors)

TB6600 Driver (for stepper motor)

Displays & Feedback:

Dual OLED Displays (I2C) for scores

Buzzer for goal alerts

Web Application for live score display

Inputs:

Joystick Modules (player control)

Power Supply:

12V Battery + Buck Converter for regulated logic and motor power


Circuit Diagram
<img width="2603" height="1120" alt="image" src="https://github.com/user-attachments/assets/319de9f6-a541-460a-8566-4341c562bffd" />


PCB Design
<img width="1706" height="1288" alt="image" src="https://github.com/user-attachments/assets/807491f7-2443-4a03-82ee-08529d89ba7c" />


Schematic Diagram of the PCB
<img width="1799" height="1405" alt="image" src="https://github.com/user-attachments/assets/80ec1be5-42a8-4ddd-be63-32d3abb1f4fb" />


Website Interface
<img width="500" height="900" alt="image" src="https://github.com/user-attachments/assets/74b480fa-73f0-4d4b-8120-45ec86fb0acd" />



👥Team Members

Ashmitha Udayachandran: 
 Role (Ultrasonic sensors integration with resetting arm mechanism, Servo motors inegration, Implementation of the website)

Arulanantharasa Nirsanth:
 Role (Programmed the controled code for the Y axis stepper motor operation, Connecting OLED display to show the scores)

Fahma Rizan:
 Role (Implementing scoring system using laser and LDR, Implementing game levels; Level 1, Level 2, Level 3)

Janaka Sachin Dissanayaka
 Role (Designing the PCB, Programmed the controled code for the X axis stepper motor operation, Connecting and inegrating WI Fi module with the arduino, programed for selecting game levels using joystick, implementing pause,resume, undo buttons)

W.P Imasha Dilshani
 Role (Integrating IR sensor and servo motors, developing code for hitting arm mechanism)
