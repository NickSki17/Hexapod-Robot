# Hexapod Tripod Gait Robot

A six-legged, ant-inspired robot developed around an alternating tripod gait, passive lower-leg mechanisms, and six servo-driven actuated joints. The project combines concept selection, Inventor CAD, preliminary actuator sizing, Arduino firmware, fabrication, and a physical walking demonstration.

**Tools:** Autodesk Inventor, Arduino C++, `Servo.h`, MDF fabrication

## Project Overview

| **Feature**            | **Specification / Detail**                           | **Evidence Class**  |
| :--------------------- | :--------------------------------------------------- | :------------------ |
| **Architecture**       | 6-legged hexapod with tripod gait                    | Designed            |
| **Microcontroller**    | Arduino Uno                                          | Physical Prototype  |
| **Actuators**          | 6 × Hitec HS-425BB standard servos                   | Physical Prototype  |
| **Chassis Material**   | 1/4″ (6.35 mm) MDF                                   | Physical Prototype  |
| **Degrees of Freedom** | 6 total (1 actuated DOF per leg)                     | Designed            |
| **Knee Joints**        | Passive mechanical stops (~7.5°) with elastic assist | Designed / Physical |
| **Walking Speed**      | Not measured in this repository                       | Unverified          |
| **Power Supply**       | 6 V battery pack                                     | Physical Prototype  |

The robot uses six legs arranged into two alternating tripod groups. Each leg features one servo-driven shoulder joint paired with a passive lower-leg mechanism. During stance, a mechanical stop makes the lower leg act as a rigid support member. During swing, the lower linkage folds forward to reduce toe dragging before returning via elastic assist.

### Concept Selection

The hexapod design was selected over an inchworm-inspired alternative based on preliminary design trade-offs:

* **Static Stability:** Provides a three-point support polygon during each tripod phase without requiring active balancing.
* **Mechanical Simplicity:** Uses single-DOF active joints with passive compliant knees to reduce mechanical and control complexity.

## Mechanical Design & Fabrication

The physical robot is constructed from 1/4″ (6.35 mm) MDF for the main chassis, upper legs, and lower legs, with six Hitec HS-425BB servos controlled by an Arduino Uno.

* **Leg Geometry:** Upper leg length: ~60 mm | Lower leg length: 40 mm
* **Body Offset:** Legs offset 26 mm from chassis centerline
* **Knee Joints:** Passive mechanical stops limiting forward rotation to ~7.5°, with elastic return assist
* **Traction:** Rubber toe grips

## Engineering Sizing & Analysis

The preliminary design calculations evaluated the static torque demands on the shoulder servos under simplified load conditions.

| **Parameter**                    | **Value**               | **Evidence Class**  | **Notes / Source**                                     |
| :------------------------------- | :---------------------- | :------------------ | :----------------------------------------------------- |
| **Calculated Robot Mass**        | 0.69 kg                 | Calculated          | Sum of chassis, servos, battery, and leg components    |
| **Total Weight ($W$)**           | 6.77 N                  | Calculated          | Derived from $0.69\text{ kg} \times 9.81\text{ m/s}^2$ |
| **Assumed Leg Load ($F_{leg}$)** | 3.38 N                  | Assumed             | Load assumption used in original sizing calculation    |
| **Required Torque ($T_{req}$)**  | 0.203 N·m (2.07 kgf·cm) | Calculated          | Static moment arm check at 60 mm                       |
| **Servo Stall Torque**           | 4.10 kgf·cm             | Specified           | Hitec HS-425BB datasheet rating at 6 V                 |
| **Static Factor of Safety**      | ~1.98                   | Preliminary theoretical calculation | Ratio of rated/stall torque to required static torque under the documented simplified load assumption |
| **Walking Speed**                | 0.24 m/s (1.3 BL/s)     | Reported benchmark  | Instructor-evaluated physical benchmark                |

> **Analysis Note:** Equal static load sharing across an ideal 3-leg tripod stance yields ~2.26 N per leg ($6.77\text{ N}/3$). The 3.38 N value was retained from the original sizing calculation as the documented design load assumption. The ~1.98 factor of safety is therefore a preliminary/theoretical static calculation based on that assumption and the servo's rated/stall torque; it is not an experimentally validated operating margin. These calculations do not model dynamic acceleration, joint friction, or voltage drop under load.

## Control System & Firmware

The control software is implemented in Arduino C++ using the standard `Servo.h` library.

### Servo Pin Assignment

| **Servo Leg Position** | **Arduino Pin** | **Neutral Position** | **Target Stance Angle** |
| :--------------------- | :-------------- | :------------------- | :---------------------- |
| **Left Front (LF)**    | Pin 2           | 90°                  | 70°                     |
| **Left Middle (LM)**   | Pin 4           | 90°                  | 110°                    |
| **Left Rear (LR)**     | Pin 7           | 90°                  | 68°                     |
| **Right Front (RF)**   | Pin 8           | 90°                  | 70°                     |
| **Right Middle (RM)**  | Pin 10          | 90°                  | 110°                    |
| **Right Rear (RR)**    | Pin 12          | 90°                  | 67°                     |

### Gait Execution

1. **Startup:** Attaches all six servos and smoothly transitions from 90° neutral to the initial gait position over 1000 ms.
2. **Stance Hold:** Holds the initial position for 7000 ms.
3. **Tripod Gait:** Alternates between two tripod groups:

   * **Tripod A:** LF, RM, LR
   * **Tripod B:** RF, LM, RR
4. **Smooth Interpolation:** Uses 25 sub-steps over a nominal 225 ms phase to coordinate servo motion.

The current control system is **open-loop** and does not use position feedback or closed-loop gait control.

## Physical Prototype & Media

The physical robot was fabricated, wired, and tested as a walking prototype.

* [Front Profile View](Media/hexapod_front.jpg)
* [Rear Profile View](Media/hexapod_rear.jpg)
* [Chassis & Servo Layout](Media/hexapod_body.jpg)
* [Arduino & Power Wiring](Media/hexapod_wiring.jpg)
* [Walking Video Demonstration](Media/hexapod_walking_demo.mp4)

## Documentation & CAD

* [Arduino Firmware](Code/hexapod_controller.ino)
* [Engineering / Design Analysis](Documentation/hexapod_design_analysis.pdf)
* [Mechanical Drawings](Documentation/hexapod_engineering_drawings.pdf)
* [Inventor Assembly](CAD/hexapod_assembly.iam)
* [Inventor Presentation](CAD/hexapod_presentation.ipn)
* [Inventor Parts](CAD/CAD%20Parts/)

## Limitations & Future Work

* **Open-Loop Control:** No ground-contact, IMU, or position feedback is currently implemented.
* **Dynamic Torque Modeling:** The actuator sizing uses a simplified static moment-arm calculation and does not model dynamic leg loading.
* **Gait Optimization:** Stride amplitude and timing are currently fixed firmware parameters.
* **Quantitative Testing:** The repository does not include an independently documented walking-speed measurement, distance testing, physical mass measurement, or servo calibration data. The 0.24 m/s value in the analysis table is an instructor-evaluated benchmark reported by the project documentation.
* **Firmware Notes:** The gait uses fixed angle, timing, and interpolation parameters; no feedback or automatic gait adaptation is implemented.

## Repository Structure

```text
Hexapod-Robot/
├── README.md
├── .gitignore
├── Code/
│   └── hexapod_controller.ino
├── CAD/
│   ├── hexapod_assembly.iam
│   ├── hexapod_presentation.ipn
│   ├── CAD Parts/
│   └── OldVersions/
├── Documentation/
│   ├── hexapod_design_analysis.pdf
│   └── hexapod_engineering_drawings.pdf
└── Media/
    ├── hexapod_front.jpg
    ├── hexapod_rear.jpg
    ├── hexapod_body.jpg
    ├── hexapod_wiring.jpg
    └── hexapod_walking_demo.mp4
```


## Tools & Skills

**Autodesk Inventor · Arduino C++ · Servo Control · Tripod Gait · Mechanism Design · Actuator Sizing · Static Torque Analysis · CAD · MDF Fabrication · Physical Prototyping**
