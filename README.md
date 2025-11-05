# 🚦 Traffic Light Controller Using STM32F103C6

## 📌 Project Overview
This project implements an **automated traffic light controller** on the **STM32F103C6** microcontroller.  
The system uses **Timer Interrupts** and a **Finite State Machine (FSM)** to manage LED transitions between **Red**, **Yellow**, and **Green** states.  
In addition, a **button input** allows the user to **adjust the timing duration** dynamically during operation.

---

## 🛠️ Technical Details
- **Microcontroller**: STM32F103C6  
- **Programming Language**: C  
- **Framework / SDK**: STM32 HAL (STM32CubeIDE)  
- **Control Method**: FSM + Timer Interrupt (TIMx Update Event)  
- **I/O Configuration**:
  - **Output GPIO**:  
    - Red LED  
    - Yellow LED  
    - Green LED  
  - **Input GPIO**:  
    - Push button for increasing or modifying light duration  

---

## ⚙️ Features
1. **Automatic Light Cycling**
   - Red → Green → Yellow → Red (continuous loop).
2. **Adjustable Timing**
   - Press button to increase or reset timing values.
3. **Stable Timing Using Timer Interrupt**
   - No delay-based blocking — system remains responsive.
4. **Debounce Handling**
   - Ensures smooth and reliable user input behavior.
5. **Scalable FSM Structure**
   - Easy to extend or switch to multiple intersections.

---

## 🔄 State Machine
| State  | LED Active      | Default Duration |
|-------|-----------------|-----------------|
| RED   | 🔴 Red          | ~5–10 seconds    |
| GREEN | 🟢 Green        | ~5–10 seconds    |
| YELLOW| 🟡 Yellow       | ~2–3 seconds     |

Transitions occur when **Timer interrupt** triggers a state timeout.

---

## 🧪 Testing & Validation
- Verified **accurate timing** via hardware timer calibration.
- Confirmed **smooth LED transitions** with no flickering.
- Push button tested with **software debounce**.
- System remains responsive even during long light durations.

---

## 👨‍💻 Author
**Tran Duc Anh**  
Computer Engineering Student — HCMUT  
Focus Areas: Embedded Systems, Digital Design, MCU Programming  

📫 Contact:  
- Facebook: https://www.facebook.com/anh.tran.78639  
- LinkedIn: https://www.linkedin.com/in/anh-tr%E1%BA%A7n-%C4%91%E1%BB%A9c-84116b368/  
- Email: **20211.tranducanh.dbk@gmail.com**

