# 🧬 GameOfLifePocket — User Manual  
*A pocket-sized cellular world you can draw, run, and evolve.*

---

## 🔎 What is it?
**GameOfLifePocket** is a tiny handheld device that runs **Conway’s Game of Life** and a few **rule variants** on a monochrome screen.  
You can **draw cells manually** with a cursor, or let the simulation **run automatically** with adjustable speed and optional “injection”.

---

## 🧭 Device layout

### 🟦 Left side
- **Height knob (potentiometer)** → moves the cursor **Up / Down** (Y axis)

### 🟦 Top side
- **Width knob (potentiometer)** → moves the cursor **Left / Right** (X axis)

### 🟥 Top-right side
- **ON/OFF** → powers the device (cuts power)

### 🟩 Front
- **Screen**
- **T (Toggle)** → button on the **right** of the screen
- **M (Manual/Auto)** → **bottom-left** button
- **P (Play/Pause)** → **bottom-right** button

### 🟨 Right side
- **USB-C charging port**

---

## 🖥️ What you see on screen

### 🎯 Cursor (MANUAL)
In **MANUAL**, a small **square frame** shows the cursor position.

### 🏷️ Overlay (optional)
When enabled, the overlay shows:
- **M** = MANUAL / **A** = AUTO (top-left)
- **Generation counter** (how many steps since last reset)

> Overlay can be hidden completely (see shortcuts).

---

## 🚀 Quick start (30 seconds)
1. Turn **ON**
2. You land in **SETUP**
3. Adjust speed/injection (and rule if you want)
4. Press **T** to **START**

---

## ⚙️ SETUP mode (before starting)
This is where you choose how the world will behave.

### 🎛 Adjust settings
- **Delay / Speed**: sets how fast AUTO runs  
- **Injection**: how many random cells get added periodically (AUTO)

*(The exact pot mapping depends on your build/settings, but it’s shown on screen.)*

### 🧪 Change rule
- **P short press** → next rule  
The **rule name** is displayed at the top of the screen (replaces “SETUP”).

### ▶️ Start
- **T short press** → start the simulation

---

## ✍️ MANUAL mode (draw & experiment)
MANUAL is perfect to **build patterns** and test them step-by-step.

### Move the cursor
- **Left knob (Height)** → Up/Down  
- **Top knob (Width)** → Left/Right

### Toggle a cell
- **T short press** → cell under the cursor becomes **alive ↔ dead**

### Step one generation
- **P short press** → advance by **1 generation**

### Reset (clear the screen)
- **M long press** → clears all cells + resets generation count

---

## ⚡ AUTO mode (let it run)
AUTO makes the world evolve continuously.

### Play / Pause
- **P short press** → **Play ↔ Pause**

### Injection (keeps it alive)
If injection is > 0, AUTO will periodically add random live cells  
to keep motion going and avoid “dead worlds”.

---

## 🌍 Global shortcuts (work anywhere)

### 👁️ Show / hide overlay (letter + generations)
- **T long press** → overlay **ON ↔ OFF**  
*(hides both the letter and the number)*

### 🧹 Reset everything (wipe the world)
- **M long press** → reset (clear + gen=0)

### ↩️ Back to SETUP
- **P long press** → return to SETUP

---

## 💡 Tips (fun patterns)
Try these in MANUAL:
- **Blinker**: 3 cells in a line → press **P** a few times
- **Glider**: small moving pattern (search “Game of Life glider”)
- **Small clusters** then switch to AUTO for chaos

---

## 🛠️ Troubleshooting

### I can’t see the counter / M/A letter
You probably hid the overlay:  
➡️ **T long press** to show it again.

### Nothing moves
- In **MANUAL**: press **P short** to step  
- In **AUTO**: press **P short** to play (if paused)

### I want a clean slate fast
➡️ **M long press** (reset)

---

## 🧾 Cheat sheet (super short)
- **T short**: START (SETUP) / Toggle cell (MANUAL)  
- **T long**: Overlay ON/OFF  
- **P short**: Next rule (SETUP) / Play↔Pause (AUTO) / Step (MANUAL)  
- **P long**: Back to SETUP  
- **M short**: MANUAL↔AUTO  
- **M long**: RESET
