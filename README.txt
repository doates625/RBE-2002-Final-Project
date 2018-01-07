INTRODUCTION

THis repo contains the code and report for my RBE-2002 final project at WPI. As part of a group of 3, I helped design a robot which navigates a maze, locates and extinguishes a candle, and reports its position in (X, Y, Z) distance from the robot's starting location. While I participated in most of the design process, my focus in the group was software design. I wrote 100% of the project-specific software for the robot. This included state machine design, odometry, and PID control systems implemented in C++, as well as Bluetooth robot communication and field mapping systems written in Matlab and executed on my laptop.

ORGANIZATION

This repo is divided into three folders:

- Main Board: All project-specific C++ code used in the robot (see Namespaces folder inside).
- Matlab: All project-specific Matlab code written for the UI, robot control, and mapping systems.
- Report: The final report on the robot submitted by my team.

DEPENDENCIES

This project utilized some of my general purpose Arduino libraries found at <https://github.com/doates625/ArduinoLibs> as well as my Arduino Linear Algebra library found at <https://github.com/doates625/LinearAtmel>.