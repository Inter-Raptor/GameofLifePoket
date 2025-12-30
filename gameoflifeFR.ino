/************************************************************
 *  ESP32-C3 + Nokia 5110 (PCD8544) — Automate Cellulaire
 *
 *  IMPORTANT (Arduino IDE):
 *  L’IDE Arduino auto-crée des prototypes de fonctions.
 *  Si un prototype utilise un type déclaré plus bas (ex: RuleId),
 *  ça provoque : "RuleId was not declared in this scope".
 *
 *  => On met l'enum RuleId TOUT EN HAUT (avant les #include),
 *     et on évite le nom "ruleName" -> "ruleNameStr".
 *
 *  Règles “Life-like” :
 *   - Conway    : B3/S23 (classique)
 *   - HighLife  : B36/S23 (replicateurs)
 *   - Seeds     : B2/S0 (explosif, rien ne survit)
 *   - DayNight  : B3678/S34678 (organique)
 *   - LWD       : B3/S012345678 (Life Without Death)
 *   - 2x2       : B36/S125 (motifs en blocs)
 *
 *  Contrôles RUNNING :
 *   - T court : (MANUAL) toggle cellule sous curseur
 *   - T long  : afficher/masquer overlay (A/M + générations)
 *   - P court : (AUTO) play/pause | (MANUAL) step 1 génération
 *   - P long  : aller au SETUP
 *   - M court : MANUAL <-> AUTO
 *   - M long  : RESET (efface tout + gen=0)
 *
 *  Contrôles SETUP :
 *   - POT_X : Delay 15..5000 ms
 *   - POT_Y : Inject 0..100 (chez toi droite=ADC faible => inversé)
 *   - M court : MANUAL/AUTO
 *   - P court : changer de règle (cycle)
 *   - T court : START
 ************************************************************/

// ✅ On met l'enum ici (avant #include) pour éviter le bug de prototypes Arduino
enum RuleId {
  RULE_CONWAY,
  RULE_HIGHLIFE,
  RULE_SEEDS,
  RULE_DAYNIGHT,
  RULE_LWD,
  RULE_2X2,
  RULE_COUNT
};

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#ifdef ESP32
  #include <WiFi.h>
#endif

// =====================
// PINS (ESP32-C3)
// =====================
// LCD Nokia 5110 - Software SPI
static const int PIN_LCD_SCLK = 7;
static const int PIN_LCD_DIN  = 6;
static const int PIN_LCD_DC   = 4;
static const int PIN_LCD_CS   = 5;
static const int PIN_LCD_RST  = 3;

// Potars (ADC)
static const int PIN_POT_X = 0; // GPIO0
static const int PIN_POT_Y = 1; // GPIO1

// Boutons (INPUT_PULLUP) : bouton entre GPIO et GND
static const int PIN_BTN_TOGGLE = 20; // T
static const int PIN_BTN_PLAY   = 21; // P
static const int PIN_BTN_MODE   = 10; // M

// =====================
// CALIB POTARS (tes mesures)
// =====================
static const int POTX_MIN = 4;
static const int POTX_MAX = 4095;
static const int POTY_MIN = 4;
static const int POTY_MAX = 4095;

// Tu as dit : POT_X min -> bas ; POT_Y min -> droite
static const bool SWAP_AXES = true;  // potY->X, potX->Y
static const bool INVERT_X  = true;  // ADC faible -> droite
static const bool INVERT_Y  = true;  // ADC faible -> bas

// écran retourné 180°
static const uint8_t LCD_ROTATION = 2;

// =====================
// LCD
// =====================
Adafruit_PCD8544 display(PIN_LCD_SCLK, PIN_LCD_DIN, PIN_LCD_DC, PIN_LCD_CS, PIN_LCD_RST);

// =====================
// Helpers ADC
// =====================
int readADCavg(int pin) {
  long s = 0;
  for (int i = 0; i < 8; i++) s += analogRead(pin);
  return (int)(s / 8);
}

int mapClamp(int v, int inMin, int inMax, int outMin, int outMax) {
  if (v < inMin) v = inMin;
  if (v > inMax) v = inMax;
  long r = (long)(v - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
  return (int)r;
}

// =====================
// Buttons (debounce + long)
// =====================
static const unsigned long DEBOUNCE_MS = 25;
static const unsigned long LONG_MS     = 800;

static const int BTN_COUNT  = 3;
static const int BTN_TOGGLE = 0;
static const int BTN_PLAY   = 1;
static const int BTN_MODE   = 2;

static const int btnPins[BTN_COUNT] = { PIN_BTN_TOGGLE, PIN_BTN_PLAY, PIN_BTN_MODE };

int btnStable[BTN_COUNT];
int btnLastRead[BTN_COUNT];
unsigned long btnLastChange[BTN_COUNT];
unsigned long btnPressedAt[BTN_COUNT];
bool btnRelShort[BTN_COUNT];
bool btnRelLong[BTN_COUNT];

void buttonsInitSimple() {
  for (int i = 0; i < BTN_COUNT; i++) {
    pinMode(btnPins[i], INPUT_PULLUP);
    int r = digitalRead(btnPins[i]);
    btnStable[i] = r;
    btnLastRead[i] = r;
    btnLastChange[i] = millis();
    btnPressedAt[i] = 0;
    btnRelShort[i] = false;
    btnRelLong[i] = false;
  }
}

void buttonsUpdateSimple() {
  unsigned long now = millis();
  for (int i = 0; i < BTN_COUNT; i++) {
    int reading = digitalRead(btnPins[i]);

    if (reading != btnLastRead[i]) {
      btnLastRead[i] = reading;
      btnLastChange[i] = now;
    }

    if ((now - btnLastChange[i]) > DEBOUNCE_MS) {
      if (reading != btnStable[i]) {
        btnStable[i] = reading;

        if (btnStable[i] == LOW) {
          btnPressedAt[i] = now;
        } else {
          unsigned long dur = now - btnPressedAt[i];
          if (dur >= LONG_MS) btnRelLong[i] = true;
          else btnRelShort[i] = true;
        }
      }
    }
  }
}

bool btnConsumeShort(int id) { bool v = btnRelShort[id]; btnRelShort[id] = false; return v; }
bool btnConsumeLong(int id)  { bool v = btnRelLong[id];  btnRelLong[id]  = false; return v; }

// =====================
// World (Game of Life)
// =====================
static const int W = 84;
static const int H = 48;
static const bool WRAP_EDGES = false;

static const uint16_t INJECT_EVERY = 30;

static const int GRID_BITS  = W * H;
static const int GRID_BYTES = (GRID_BITS + 7) / 8;
uint8_t gridA[GRID_BYTES];
uint8_t gridB[GRID_BYTES];

inline int bitIndex(int x, int y) { return y * W + x; }

inline bool getCell(const uint8_t* g, int x, int y) {
  int idx = bitIndex(x, y);
  return (g[idx >> 3] >> (idx & 7)) & 1;
}

inline void setCell(uint8_t* g, int x, int y, bool v) {
  int idx = bitIndex(x, y);
  uint8_t &b = g[idx >> 3];
  uint8_t m = 1 << (idx & 7);
  if (v) b |= m; else b &= ~m;
}

inline void toggleCell(uint8_t* g, int x, int y) {
  int idx = bitIndex(x, y);
  g[idx >> 3] ^= (1 << (idx & 7));
}

void clearGrid(uint8_t* g) { memset(g, 0, GRID_BYTES); }

int neighbors(const uint8_t* g, int x, int y) {
  int n = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue;

      int nx = x + dx;
      int ny = y + dy;

      if (WRAP_EDGES) {
        if (nx < 0) nx += W; if (nx >= W) nx -= W;
        if (ny < 0) ny += H; if (ny >= H) ny -= H;
      } else {
        if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
      }

      if (getCell(g, nx, ny)) n++;
    }
  }
  return n;
}

// =====================
// Règles : sélection + nom
// =====================

// ✅ règle courante (changeable en SETUP via P court)
RuleId currentRule = RULE_CONWAY;

// Nom de règle (affiché à la place de "SETUP")
const char* ruleNameStr(RuleId r) {
  switch (r) {
    case RULE_CONWAY:   return "CONWAY";    // B3/S23
    case RULE_HIGHLIFE: return "HIGHLIFE";  // B36/S23
    case RULE_SEEDS:    return "SEEDS";     // B2/S0
    case RULE_DAYNIGHT: return "DAYNIGHT";  // B3678/S34678
    case RULE_LWD:      return "LWD";       // B3/S012345678
    case RULE_2X2:      return "2x2";       // B36/S125
    default:            return "RULE";
  }
}

// Règle : renvoie l'état futur selon currentRule
inline bool nextAliveForRule(bool alive, int n) {
  switch (currentRule) {
    case RULE_CONWAY:   // B3/S23
      if (!alive && n == 3) return true;
      if ( alive && (n == 2 || n == 3)) return true;
      return false;

    case RULE_HIGHLIFE: // B36/S23
      if (!alive && (n == 3 || n == 6)) return true;
      if ( alive && (n == 2 || n == 3)) return true;
      return false;

    case RULE_SEEDS:    // B2/S0 (aucune survie)
      if (!alive && n == 2) return true;
      return false;

    case RULE_DAYNIGHT: // B3678/S34678
      if (!alive && (n == 3 || n == 6 || n == 7 || n == 8)) return true;
      if ( alive && (n == 3 || n == 4 || n == 6 || n == 7 || n == 8)) return true;
      return false;

    case RULE_LWD:      // Life Without Death : B3/S012345678
      if (!alive && n == 3) return true;
      if (alive) return true;
      return false;

    case RULE_2X2:      // B36/S125
      if (!alive && (n == 3 || n == 6)) return true;
      if ( alive && (n == 1 || n == 2 || n == 5)) return true;
      return false;

    default:
      return false;
  }
}

// =====================
// UI / States
// =====================
static const int MODE_MANUAL = 0;
static const int MODE_AUTO   = 1;

static const int ST_SETUP   = 0;
static const int ST_RUNNING = 1;

int state = ST_SETUP;
int modeSel = MODE_MANUAL;

bool autoRunning = true;
uint32_t genCount = 0;

int delayMs = 80;     // 15..5000
int injectCount = 10; // 0..100

int cursorX = W / 2;
int cursorY = H / 2;

unsigned long lastStepMs = 0;

// Overlay : si false => on masque TOUT (lettre + nombre)
bool showGenOverlay = true;

// Format compteur en k/M/G (lisible)
void formatGen(char* out, size_t outSize, uint32_t v) {
  if (v < 1000UL) {
    snprintf(out, outSize, "%lu", (unsigned long)v);
  } else if (v < 1000000UL) {
    float f = v / 1000.0f;
    snprintf(out, outSize, "%.2fk", f);
  } else if (v < 1000000000UL) {
    float f = v / 1000000.0f;
    snprintf(out, outSize, "%.2fM", f);
  } else {
    float f = v / 1000000000.0f;
    snprintf(out, outSize, "%.2fG", f);
  }
}

// =====================
// Draw
// =====================
void drawSetupScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  // ✅ Le nom de la règle remplace "SETUP"
  display.setCursor(0, 0);
  display.print(ruleNameStr(currentRule));

  display.setCursor(0, 10);
  display.print("Delay:");
  display.print(delayMs);
  display.print("ms");

  display.setCursor(0, 20);
  display.print("Inject:");
  display.print(injectCount);

  display.setCursor(0, 30);
  display.print("Mode:");
  display.print(modeSel == MODE_AUTO ? "AUTO" : "MANUAL");

  display.setCursor(0, 40);
  display.print("T=START P=RULE");

  display.display();
}

void drawWorld(bool overlay) {
  display.clearDisplay();

  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      if (getCell(gridA, x, y)) display.drawPixel(x, y, BLACK);
    }
  }

  // Curseur en MANUAL (cadre 3x3)
  if (modeSel == MODE_MANUAL && state == ST_RUNNING) {
    int x = cursorX, y = cursorY;
    int rx = max(0, x - 1), ry = max(0, y - 1);
    int rw = (x == 0 || x == W - 1) ? 2 : 3;
    int rh = (y == 0 || y == H - 1) ? 2 : 3;
    display.drawRect(rx, ry, rw, rh, BLACK);
  }

  // Overlay : si masqué => rien du tout
  if (overlay && showGenOverlay) {
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(0, 0);
    display.print(modeSel == MODE_AUTO ? "A" : "M");

    char gbuf[12];
    formatGen(gbuf, sizeof(gbuf), genCount);
    display.setCursor(10, 0);
    display.print(gbuf);
  }

  display.display();
}

// =====================
// Cursor from pots
// =====================
void updateCursorFromPots() {
  int ax = readADCavg(PIN_POT_X);
  int ay = readADCavg(PIN_POT_Y);

  int rawX = SWAP_AXES ? ay : ax;
  int rawY = SWAP_AXES ? ax : ay;

  int inMinX = SWAP_AXES ? POTY_MIN : POTX_MIN;
  int inMaxX = SWAP_AXES ? POTY_MAX : POTX_MAX;
  int inMinY = SWAP_AXES ? POTX_MIN : POTY_MIN;
  int inMaxY = SWAP_AXES ? POTX_MAX : POTY_MAX;

  cursorX = mapClamp(rawX, inMinX, inMaxX, INVERT_X ? (W - 1) : 0, INVERT_X ? 0 : (W - 1));
  cursorY = mapClamp(rawY, inMinY, inMaxY, INVERT_Y ? (H - 1) : 0, INVERT_Y ? 0 : (H - 1));
}

// =====================
// Simu
// =====================
void injectNoise(uint8_t* g) {
  if (injectCount <= 0) return;
  for (int i = 0; i < injectCount; i++) {
    int x = random(0, W);
    int y = random(0, H);
    setCell(g, x, y, true);
  }
}

void stepLife() {
  clearGrid(gridB);

  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      bool alive = getCell(gridA, x, y);
      int n = neighbors(gridA, x, y);
      if (nextAliveForRule(alive, n)) setCell(gridB, x, y, true);
    }
  }

  memcpy(gridA, gridB, GRID_BYTES);
  genCount++;

  if (modeSel == MODE_AUTO && autoRunning && (genCount % INJECT_EVERY == 0)) {
    injectNoise(gridA);
  }
}

void seedRandom(uint8_t* g, int densityPercent = 30) {
  clearGrid(g);
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      if (random(0, 100) < densityPercent) setCell(g, x, y, true);
    }
  }
}

// =====================
// Flow
// =====================
void enterSetup() {
  state = ST_SETUP;
  autoRunning = true;
  drawSetupScreen();
}

void startFromSetup() {
  genCount = 0;

  if (modeSel == MODE_MANUAL) {
    clearGrid(gridA);
    autoRunning = false;
  } else {
    seedRandom(gridA, 30);
    autoRunning = true;
  }

  state = ST_RUNNING;
  drawWorld(true);
}

void resetWorld() {
  clearGrid(gridA);
  genCount = 0;
  drawWorld(true);
}

void nextRule() {
  int r = (int)currentRule;
  r = (r + 1) % (int)RULE_COUNT;
  currentRule = (RuleId)r;
}

// =====================
// Arduino
// =====================
void setup() {
#ifdef ESP32
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
#endif

  analogReadResolution(12);
  buttonsInitSimple();

  display.begin();
  display.setRotation(LCD_ROTATION);
  display.setContrast(55);
  display.clearDisplay();
  display.display();

#ifdef ESP32
  randomSeed((uint32_t)esp_random());
#else
  randomSeed(analogRead(PIN_POT_X));
#endif

  clearGrid(gridA);
  clearGrid(gridB);

  enterSetup();
}

void loop() {
  buttonsUpdateSimple();

  // ---- LONG presses first ----

  // T long : overlay on/off
  if (btnConsumeLong(BTN_TOGGLE)) {
    showGenOverlay = !showGenOverlay;
    if (state == ST_RUNNING) drawWorld(true);
    else drawSetupScreen();
    return;
  }

  // M long : reset monde
  if (btnConsumeLong(BTN_MODE)) {
    if (state == ST_RUNNING) resetWorld();
    else { clearGrid(gridA); genCount = 0; drawSetupScreen(); }
    return;
  }

  // P long : aller en setup
  if (btnConsumeLong(BTN_PLAY)) {
    enterSetup();
    return;
  }

  // ---- SHORT presses ----

  // M short : manual/auto
  if (btnConsumeShort(BTN_MODE)) {
    modeSel = (modeSel == MODE_AUTO) ? MODE_MANUAL : MODE_AUTO;

    if (state == ST_RUNNING) {
      autoRunning = (modeSel == MODE_AUTO);
      drawWorld(true);
    } else {
      drawSetupScreen();
    }
  }

  // P short :
  // - en SETUP : change de règle
  // - en RUNNING : play/pause (AUTO) ou step (MANUAL)
  if (btnConsumeShort(BTN_PLAY)) {
    if (state == ST_SETUP) {
      nextRule();
      drawSetupScreen();
      return;
    }

    if (state == ST_RUNNING) {
      if (modeSel == MODE_AUTO) {
        autoRunning = !autoRunning;
        drawWorld(true);
      } else {
        stepLife();
        drawWorld(true);
      }
    }
  }

  // T short : start / toggle cellule
  if (btnConsumeShort(BTN_TOGGLE)) {
    if (state == ST_SETUP) {
      startFromSetup();
      return;
    } else {
      if (modeSel == MODE_MANUAL) {
        updateCursorFromPots();
        toggleCell(gridA, cursorX, cursorY);
        drawWorld(true);
      }
    }
  }

  // ---- State logic ----

  if (state == ST_SETUP) {
    int ax = readADCavg(PIN_POT_X);
    int ay = readADCavg(PIN_POT_Y);

    delayMs     = mapClamp(ax, POTX_MIN, POTX_MAX, 15, 5000);
    injectCount = mapClamp(ay, POTY_MIN, POTY_MAX, 100, 0);

    static unsigned long lastUI = 0;
    if (millis() - lastUI > 120) {
      lastUI = millis();
      drawSetupScreen();
    }
    return;
  }

  // RUNNING
  if (modeSel == MODE_MANUAL) {
    updateCursorFromPots();
    static unsigned long lastDraw = 0;
    if (millis() - lastDraw > 80) {
      lastDraw = millis();
      drawWorld(true);
    }
  } else {
    if (autoRunning && (millis() - lastStepMs >= (unsigned long)delayMs)) {
      lastStepMs = millis();
      stepLife();
      drawWorld(true);
    } else {
      static unsigned long lastDraw = 0;
      if (millis() - lastDraw > 250) {
        lastDraw = millis();
        drawWorld(true);
      }
    }
  }
}
