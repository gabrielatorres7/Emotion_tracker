# Emotion_tracker
This project explores a screenless emotional decision-making device built with Arduino, designed to help individuals recognize whether their choices originate from fear-based or inspiration-based emotional states. 
# Screenless Emotional Decision-Making Device (Arduino)

This project is a screenless emotional guidance device designed to help individuals understand whether their current decisions are driven by **fear-based** or **inspiration-based** emotional states. It uses an Arduino with an LCD keypad shield and two LEDs to provide immediate, tactile feedback without relying on traditional screens.

---

## 🌱 Project Purpose
The device interprets emotional inputs through a simple rating model and displays guidance using:
- 🔴 **Red LED — Dark Master**  
  Represents fear-driven emotions: Shame, Guilt, Apathy, Anger, Desire  

- 🟢 **Green LED — Light Being**  
  Represents inspiration-driven emotions: Courage, Acceptance, Love, Joy, Peace  

The goal is to support clearer emotional awareness and inspired decision-making in everyday life.

---

## 🎛️ How It Works
1. The user selects or rates their emotional state (1–5 intensity scale).
2. The Arduino interprets the level of “dark” vs “light” emotions.
3. The LEDs respond:
   - If fear-based emotions dominate → red LED glows  
   - If inspiration-based emotions dominate → green LED glows  
4. The LCD provides simple, text-based emotional feedback.

---

## 🧠 Emotion Model
The device uses the following emotional categories:

### **Fear-based (Dark Master)**  
- Shame  
- Guilt  
- Apathy  
- Anger  
- Desire  

### **Inspiration-based (Light Being)**  
- Courage  
- Acceptance  
- Love  
- Joy  
- Peace  

Each is rated from **1 to 5**, where 1 = slight and 5 = dominant.

---

## 🛠️ Hardware Requirements
- Arduino Uno  
- LCD Keypad Shield (DFRobot-compatible)  
- Red LED  
- Green LED  
- Jumper wires  

---

## 📁 Project Structure
