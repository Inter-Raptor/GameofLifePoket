# 🧬 GameOfLifePoket — Build Guide (EN)

> **Goal:** build a pocket device that runs **Conway’s Game of Life** on a small screen with **physical controls** (pots + buttons).  
> Repo: https://github.com/Inter-Raptor/GameofLifePoket  
> Presentation video: https://youtu.be/aNUI-kbQdfw

---

## 0) What you get

- A compact ESP32 device displaying the Game of Life
- **2 potentiometers** to adjust **Width** and **Height**
- **3 front buttons**: **Toggle**, **Manu/Auto**, **Play/Pause**
- **ON/OFF switch**
- **USB‑C** power / charging (depending on your power module)

---

## 1) Bill of Materials (BOM)

### Core
- **ESP32 board** (recommended: ESP32 DevKit v1 / ESP32‑WROOM module board)
- **Display** *(choose one, based on your code)*:
  - I²C OLED: SSD1306 128×64 (very common), **or**
  - SPI TFT: ST7735 / ILI9341 / etc. *(only if your firmware supports it)*

### Controls
- 2× **Potentiometer** (10k is a safe default; 5k–50k usually ok)
- 3× **Momentary push buttons**
- 1× **Power switch** (latching ON/OFF)

### Power (pick ONE of these approaches)
- **Simple (no battery):** USB‑C 5V → ESP32 5V pin
- **With battery + charging:**  
  - 1× Li‑ion / LiPo battery (ex: 500–2500 mAh)  
  - 1× **USB‑C charger module** (TP4056 USB‑C or similar)  
  - 1× **5V boost** (if you need 5V), or run ESP32 from **3.3V regulated** supply depending on your setup

### Nice-to-have
- 3× 10k resistors (optional pullups/pulldowns if you don’t use internal pulls)
- Small perfboard / prototype PCB
- Wires, heatshrink, screws, standoffs

---

## 2) Tools

- Soldering iron + solder
- Wire stripper, cutters
- Multimeter (highly recommended)
- PC + USB cable
- Arduino IDE **or** PlatformIO (depending on your firmware)

---

## 3) Pin mapping (IMPORTANT)

I **don’t have your exact GPIO list** from the firmware here.  
So below you get:

1) A **recommended example wiring** for ESP32 DevKit, and  
2) A **quick method to match it to your code**.

### 3.1 Recommended example (ESP32 DevKit)

#### Buttons (digital GPIO)
- **TOGGLE** → GPIO **27**
- **MANU/AUTO** → GPIO **26**
- **PLAY/PAUSE** → GPIO **25**

Wiring for each button:
- One side of button → the GPIO
- Other side → **GND**
- In firmware: use **INPUT_PULLUP** (so pressed = LOW)

#### Potentiometers (ADC)
- **Width pot (Top)** wiper → GPIO **34** (ADC1)
- **Height pot (Left side)** wiper → GPIO **35** (ADC1)

Pot wiring (each pot):
- One outer pin → **3.3V**
- Other outer pin → **GND**
- Middle pin (wiper) → ADC GPIO (34/35)

✅ Using **ADC1 pins** (GPIO 32–39) is recommended because ADC2 can conflict with Wi‑Fi.

#### Display
Pick the wiring based on your display type:

**Option A — SSD1306 I²C OLED (common)**
- VCC → 3.3V
- GND → GND
- SDA → GPIO **21**
- SCL → GPIO **22**

**Option B — SPI TFT (example ST7735)**
- VCC → 3.3V (or 5V depending on module)
- GND → GND
- SCK → GPIO **18**
- MOSI → GPIO **23**
- CS → GPIO **5**
- DC → GPIO **16**
- RST → GPIO **17** (or tie to EN/RST)
- BL (backlight) → 3.3V or a PWM GPIO (optional)

> If your screen is different (ILI9341 etc.), keep SPI principle but adapt pins to the library config.

---

## 4) How to match pins to your firmware (fast)

Open your project and search for keywords like:
- `#define` (ex: `BTN_PLAY`, `POT_W`, `TFT_CS`)
- `pinMode(`
- `analogRead(`
- display init like `Wire.begin(SDA,SCL)` or `tft.begin(...)`

Then:
- Put your actual GPIO numbers into the table below (and wire accordingly).

### 4.1 Your pin table (fill this once)
| Function | GPIO |
|---|---|
| Button Toggle | __ |
| Button Manu/Auto | __ |
| Button Play/Pause | __ |
| Pot Width (ADC) | __ |
| Pot Height (ADC) | __ |
| Display SDA (I²C) | __ |
| Display SCL (I²C) | __ |
| (SPI) SCK | __ |
| (SPI) MOSI | __ |
| (SPI) CS | __ |
| (SPI) DC | __ |
| (SPI) RST | __ |
| (SPI) BL | __ |

---

## 5) Wiring step-by-step (safe method)

### Step 1 — Power & ground first
1. Connect all **GND** together (ESP32 GND, screen GND, buttons GND, pots GND)
2. Connect **3.3V** to screen VCC (if it’s a 3.3V screen) and to both pots outer pins.

✅ **Check with multimeter**: continuity on GND; 3.3V is correct.

### Step 2 — Display
- I²C: connect SDA/SCL
- SPI: connect SCK/MOSI/CS/DC/RST (+ optional BL)

### Step 3 — Buttons
For each button:
- GPIO → one side
- Other side → GND  
Then in code: `pinMode(pin, INPUT_PULLUP);`

### Step 4 — Potentiometers
For each pot:
- Outer pin → 3.3V
- Other outer pin → GND
- Middle pin → ADC GPIO

✅ Quick test: in firmware (or a small test sketch) print `analogRead()` values; turning should move smoothly.

---

## 6) Power options (choose your setup)

### Option 1 — USB only (simplest)
- USB‑C 5V → ESP32 **5V/VIN** (through the board’s regulator)

### Option 2 — Battery + charging (common DIY)
Typical chain:
- USB‑C charger module (TP4056) charges Li‑ion
- Battery output → **boost** to 5V (if needed) or **3.3V regulator** to feed ESP32 3.3V
- Add an ON/OFF switch on the **battery output** (or on the regulator input)

⚠️ Safety:
- Use a protected cell or protection board
- Insulate everything (no short circuits)

---

## 7) Flashing the firmware

### Arduino IDE (generic)
1. Install ESP32 board support (Boards Manager → Espressif ESP32)
2. Open the `.ino`
3. Select board (ex: **ESP32 Dev Module**)
4. Select the right COM port
5. Upload

### PlatformIO (if you use it)
1. Open project folder in VS Code
2. PlatformIO: **Build** then **Upload**

---

## 8) First boot & calibration

- Power ON
- Move both pots slowly:
  - confirm width/height values change on screen (or in serial logs if available)
- Try buttons:
  - Manu/Auto switches behavior
  - Play/Pause stops/resumes
  - Toggle changes the assigned option/action

If something is inverted:
- Buttons: check if your code expects pullup (pressed=LOW) or pulldown (pressed=HIGH)
- Pots: swap 3.3V/GND on the outer pins to reverse direction

---

## 9) Troubleshooting

### Screen black
- Wrong VCC (3.3V vs 5V)
- Wrong SDA/SCL or SPI pins
- Wrong display driver in code (SSD1306 vs SH1106 vs TFT)
- Missing reset/backlight wiring (SPI TFT)

### Pots don’t react
- ADC pin not compatible
- Using ADC2 pin while Wi‑Fi is enabled (prefer ADC1)
- Middle pin not connected to ADC (wiring mistake)

### Buttons act randomly
- Floating input: enable `INPUT_PULLUP` (recommended) or add external resistors
- Bad ground connection

---

## 10) Recommended repository files (for a clean “pro” look)

Add these to GitHub:

- `README.md` — Main page: project overview, video link, quick start
- `BUILD_GUIDE.md` — Full build tutorial (parts list, wiring, flashing, troubleshooting)
- `WIRING.md` *(optional)* — Pin table + a simple wiring diagram
- `images/` — Photos of the device, wiring, and front panel
- `LICENSE` — **MIT** (recommended if you want others to reuse/modify the project)

> If you only want the essentials: keep **README.md + BUILD_GUIDE.md + images/**.

---

