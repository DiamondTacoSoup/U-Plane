//**AI disclosure** I am bad at writing pretty code, I initially wrote this code myself and then ran it through AI for nice formatting. The logic is all mine, the only things the AI changed was the indentation and variable names. Most of the comments are handwritten as well, you can probably tell which are and which aren't but it is a 90/10 split (Human:Ai).


#include <AccelStepper.h>

// Pin map
#define PIN_M1_STEP   18   // U1 STEP  (motor 1)
#define PIN_M1_DIR    17   // U1 DIR
#define PIN_M2_STEP   16   // U2 STEP  (motor 2)
#define PIN_M2_DIR    15   // U2 DIR
#define PIN_EN        14   // shared EN net for U1 + U2 ("Enable" label)
#define EN_ACTIVE_LOW 1    // TMC2209 ENN: LOW = driver enabled
#define PIN_BATT       1

// Physical dimentions
static const float TOTAL_MM     = 300.0f;   // full travel, retracted -> extended
static const float STEPS_PER_MM = 40.0f;    // your resolution incl. 1/8 microsteps
static const long  TOTAL_STEPS  = (long)(TOTAL_MM * STEPS_PER_MM + 0.5f);

static const float DEFAULT_SPEED = 1600.0f; // steps/s
static const float DEFAULT_ACCEL = 3200.0f; // steps/s^2

// battery divider values
static const float BATT_R_ABOVE = 92000.0f;
static const float BATT_R_BELOW = 10000.0f;
static const float BATT_DIVIDER = (BATT_R_ABOVE + BATT_R_BELOW) / BATT_R_BELOW;
static const float BATT_LOW_V   = 20.0f;


static float g_speed = DEFAULT_SPEED;
static float g_accel = DEFAULT_ACCEL;
static bool  g_enabled = false;

AccelStepper m1(AccelStepper::DRIVER, PIN_M1_STEP, PIN_M1_DIR);
AccelStepper m2(AccelStepper::DRIVER, PIN_M2_STEP, PIN_M2_DIR);

static char    rxLine[96];
static uint8_t rxLen = 0;

// Definitions
static long clampSteps(long s)
{
  if (s < 0) return 0;
  if (s > TOTAL_STEPS) return TOTAL_STEPS;
  return s;
}

static long pctToSteps(float pct)
{
  if (pct < 0.0f)   pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;
  return (long)(pct * (float)TOTAL_STEPS / 100.0f + 0.5f);
}

static float stepsToPct(long steps)
{
  return 100.0f * (float)steps / (float)TOTAL_STEPS;
}

static void setEnable(bool on)
{
#if EN_ACTIVE_LOW
  digitalWrite(PIN_EN, on ? LOW : HIGH);
#else
  digitalWrite(PIN_EN, on ? HIGH : LOW);
#endif
  g_enabled = on;
}

static float readBattery(void)
{
  const int N = 16;
  uint32_t s[N];

  for (int i = 0; i < N; i++) {
    s[i] = (uint32_t)analogReadMilliVolts(PIN_BATT);
  }
  for (int i = 1; i < N; i++) {          // insertion sort, then median
    uint32_t key = s[i];
    int j = i - 1;
    while (j >= 0 && s[j] > key) { s[j + 1] = s[j]; j--; }
    s[j + 1] = key;
  }
  float mv = 0.5f * (float)(s[N / 2 - 1] + s[N / 2]);
  return (mv / 1000.0f) * BATT_DIVIDER;
}


static void startMoveSingle(uint8_t axis, long target)
{
  AccelStepper &m = (axis == 1) ? m1 : m2;
  target = clampSteps(target);
  m.setMaxSpeed(g_speed);
  m.setAcceleration(g_accel);
  m.moveTo(target);                     // returns immediately, run() does the work
}

// Both targets are set at once; speeds are scaled by travel so that the axis with the longer move finishes at the same time as the shorter one.
static void startMoveBoth(long target1, long target2)
{
  target1 = clampSteps(target1);
  target2 = clampSteps(target2);

  long d1 = labs(target1 - m1.currentPosition());
  long d2 = labs(target2 - m2.currentPosition());
  long dmax = (d1 > d2) ? d1 : d2;

  float s1 = g_speed;
  float s2 = g_speed;
  if (dmax > 0) {
    s1 = g_speed * (float)d1 / (float)dmax;
    s2 = g_speed * (float)d2 / (float)dmax;
    float floor_s = g_speed * 0.02f;     // don't crawl on tiny moves
    if (s1 < floor_s) s1 = floor_s;
    if (s2 < floor_s) s2 = floor_s;
  }

  m1.setMaxSpeed(s1);
  m2.setMaxSpeed(s2);
  m1.setAcceleration(g_accel);
  m2.setAcceleration(g_accel);
  m1.moveTo(target1);
  m2.moveTo(target2);
}

// debugging/serial :P
static void printHelp(void)
{
  Serial.println();
  Serial.println(F("Commands (case-insensitive):"));
  Serial.println(F("  ?                        this help"));
  Serial.println(F("  STATUS                   position, % travel, speed, battery"));
  Serial.println(F("  CFG                      show configuration"));
  Serial.println(F("  ZERO [1|2|ALL]           current position becomes 0 % (retracted)"));
  Serial.println(F("  MOVE <1|2|ALL> <steps>   go to an absolute step position"));
  Serial.println(F("  JOG  <1|2|ALL> <steps>   move relative to the current position"));
  Serial.println(F("  PCT  <1|2|ALL> <pct>     go to a percentage of total travel"));
  Serial.println(F("  PCTREL <1|2|ALL> <pct>   move by a percentage of total travel"));
  Serial.println(F("  SPEED <steps/s>          cruise speed (10..20000)"));
  Serial.println(F("  ACCEL <steps/s^2>        acceleration (10..200000)"));
  Serial.println(F("  ENABLE | DISABLE         driver enable pin"));
  Serial.println(F("  STOP                     decelerate both axes to a halt"));
  Serial.println(F("  BAT                      read battery voltage now"));
  Serial.println(F("Examples: PCT 1 45.5   JOG 2 -500   PCT ALL 100   PCTREL 1 -10"));
  Serial.println();
}

static void printConfig(void)
{
  Serial.println(F("--- configuration ---"));
  Serial.print(F("total travel      : ")); Serial.print(TOTAL_MM, 2);
  Serial.print(F(" mm = ")); Serial.print(TOTAL_STEPS);
  Serial.println(F(" steps"));
  Serial.print(F("step resolution   : ")); Serial.print(STEPS_PER_MM, 2);
  Serial.println(F(" steps/mm"));
  Serial.print(F("speed / accel     : ")); Serial.print(g_speed);
  Serial.print(F(" steps/s, ")); Serial.print(g_accel); Serial.println(F(" steps/s^2"));
  Serial.print(F("pins  M1 step/dir : ")); Serial.print(PIN_M1_STEP);
  Serial.print(F("/")); Serial.println(PIN_M1_DIR);
  Serial.print(F("pins  M2 step/dir : ")); Serial.print(PIN_M2_STEP);
  Serial.print(F("/")); Serial.println(PIN_M2_DIR);
  Serial.print(F("enable pin        : ")); Serial.print(PIN_EN);
  Serial.println(EN_ACTIVE_LOW ? F(" (active LOW)") : F(" (active HIGH)"));
  Serial.println(F("--------------------"));
}

static void printStatus(void)
{
  char b1[40], b2[40];
  snprintf(b1, sizeof(b1), "%ld/%ld steps %.2f%%", m1.currentPosition(), TOTAL_STEPS,
           stepsToPct(m1.currentPosition()));
  snprintf(b2, sizeof(b2), "%ld/%ld steps %.2f%%", m2.currentPosition(), TOTAL_STEPS,
           stepsToPct(m2.currentPosition()));

  Serial.println();
  Serial.print(F("M1  ")); Serial.print(b1);
  Serial.print(F("  target ")); Serial.print(m1.targetPosition());
  Serial.println(m1.isRunning() ? F("  moving") : F("  idle"));
  Serial.print(F("M2  ")); Serial.print(b2);
  Serial.print(F("  target ")); Serial.print(m2.targetPosition());
  Serial.println(m2.isRunning() ? F("  moving") : F("  idle"));

  float v = readBattery();
  Serial.print(F("Battery  ")); Serial.print(v, 2); Serial.print(F(" V"));
  if (v < BATT_LOW_V) Serial.print(F("   <-- LOW"));
  Serial.print(F("   drivers ")); Serial.println(g_enabled ? F("enabled") : F("disabled"));
  Serial.println();
}

//command definitions

static void uppercase(char *s)
{
  for (; *s; s++) {
    if (*s >= 'a' && *s <= 'z') *s = (char)(*s - 'a' + 'A');
  }
}

/* 0 = both, 1 or 2 = one axis, -1 = bad argument */
static int parseMotor(char *s)
{
  if (s == NULL) return -1;
  if (s[0] == '1' && s[1] == 0) return 1;
  if (s[0] == '2' && s[1] == 0) return 2;
  uppercase(s);
  if (!strcmp(s, "ALL") || !strcmp(s, "BOTH") || !strcmp(s, "A")) return 0;
  return -1;
}

static void handleCommand(char *line)
{
  char *verb = strtok(line, " ,\t");
  if (verb == NULL) return;
  uppercase(verb);

  if (!strcmp(verb, "?") || !strcmp(verb, "HELP")) {
    printHelp();
    return;
  }

  if (!strcmp(verb, "STATUS") || !strcmp(verb, "S")) {
    printStatus();
    return;
  }

  if (!strcmp(verb, "CFG")) {
    printConfig();
    return;
  }

  if (!strcmp(verb, "BAT")) {
    float v = readBattery();
    Serial.print(F("Battery "));
    Serial.print(v, 2);
    Serial.println(F(" V"));
    return;
  }

  if (!strcmp(verb, "STOP")) {
    m1.stop();
    m2.stop();
    Serial.println(F("Stopping (decelerating)"));
    return;
  }

  if (!strcmp(verb, "ENABLE")) {
    setEnable(true);
    Serial.println(F("Drivers enabled"));
    return;
  }

  if (!strcmp(verb, "DISABLE")) {
    m1.stop();
    m2.stop();
    setEnable(false);
    Serial.println(F("Drivers disabled (motors free)"));
    return;
  }

  if (!strcmp(verb, "SPEED")) {
    char *val = strtok(NULL, " ,\t");
    if (val == NULL) { Serial.println(F("Usage: SPEED <steps/s>")); return; }
    float v = (float)fabs(atof(val));
    if (v < 10.0f)     v = 10.0f;
    if (v > 20000.0f)  v = 20000.0f;
    g_speed = v;
    m1.setMaxSpeed(v);
    m2.setMaxSpeed(v);
    Serial.print(F("Speed set to ")); Serial.print(g_speed); Serial.println(F(" steps/s"));
    return;
  }

  if (!strcmp(verb, "ACCEL")) {
    char *val = strtok(NULL, " ,\t");
    if (val == NULL) { Serial.println(F("Usage: ACCEL <steps/s^2>")); return; }
    float v = (float)fabs(atof(val));
    if (v < 10.0f)      v = 10.0f;
    if (v > 200000.0f)  v = 200000.0f;
    g_accel = v;
    m1.setAcceleration(v);
    m2.setAcceleration(v);
    Serial.print(F("Acceleration set to ")); Serial.print(g_accel);
    Serial.println(F(" steps/s^2"));
    return;
  }

  if (!strcmp(verb, "ZERO")) {
    int who = parseMotor(strtok(NULL, " ,\t"));
    if (who < 0) who = 0;                        // no argument -> both
    if (who == 0 || who == 1) m1.setCurrentPosition(0);
    if (who == 0 || who == 2) m2.setCurrentPosition(0);
    Serial.println(F("Position zeroed (0 % = fully retracted)"));
    return;
  }

  bool isMove   = !strcmp(verb, "MOVE");
  bool isJog    = !strcmp(verb, "JOG");
  bool isPct    = !strcmp(verb, "PCT");
  bool isPctRel = !strcmp(verb, "PCTREL");

  if (isMove || isJog || isPct || isPctRel) {
    char *whoArg = strtok(NULL, " ,\t");
    char *valArg = strtok(NULL, " ,\t");
    int who = parseMotor(whoArg);

    if (who < 0 || valArg == NULL) {
      Serial.println(F("Usage: <MOVE|JOG|PCT|PCTREL> <1|2|ALL> <value>"));
      return;
    }

    float value = atof(valArg);
    long t1 = m1.currentPosition();
    long t2 = m2.currentPosition();

    if (isMove) {                       // absolute steps
      long s = clampSteps((long)value);
      if (who == 0 || who == 1) t1 = s;
      if (who == 0 || who == 2) t2 = s;
    } else if (isJog) {                 // relative steps
      long d = (long)value;
      if (who == 0 || who == 1) t1 = clampSteps(t1 + d);
      if (who == 0 || who == 2) t2 = clampSteps(t2 + d);
    } else if (isPct) {                 // absolute percentage of full travel
      long s = pctToSteps(value);
      if (who == 0 || who == 1) t1 = s;
      if (who == 0 || who == 2) t2 = s;
    } else {                            // relative percentage of full travel
      long d = (long)(value * (float)TOTAL_STEPS / 100.0f);
      if (who == 0 || who == 1) t1 = clampSteps(t1 + d);
      if (who == 0 || who == 2) t2 = clampSteps(t2 + d);
    }

    if (who == 0) startMoveBoth(t1, t2);
    else if (who == 1) startMoveSingle(1, t1);
    else startMoveSingle(2, t2);

    Serial.print(F("M1 -> ")); Serial.print(m1.targetPosition());
    Serial.print(F(" (")); Serial.print(stepsToPct(m1.targetPosition()), 2);
    Serial.print(F("%)   M2 -> ")); Serial.print(m2.targetPosition());
    Serial.print(F(" (")); Serial.print(stepsToPct(m2.targetPosition()), 2);
    Serial.println(F("%)"));
    return;
  }

  Serial.print(F("Unknown command: "));
  Serial.println(verb);
  Serial.println(F("Type ? for the command list."));
}

static void pollSerial(void)
{
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      rxLine[rxLen] = '\0';
      if (rxLen > 0) handleCommand(rxLine);
      rxLen = 0;
    } else if (rxLen < sizeof(rxLine) - 1) {
      rxLine[rxLen++] = c;
    }
  }
}

/* ============================ ARDUINO =================================== */
void setup(void)
{
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 1500) delay(10);   // USB-CDC boards

  digitalWrite(PIN_EN, EN_ACTIVE_LOW ? HIGH : LOW);       // start disabled
  pinMode(PIN_EN, OUTPUT);
  setEnable(true);

  pinMode(PIN_BATT, INPUT);
  analogSetAttenuation(ADC_11db);                         // 0..~3.1 V range

  m1.setMaxSpeed(g_speed);
  m1.setAcceleration(g_accel);
  m2.setMaxSpeed(g_speed);
  m2.setAcceleration(g_accel);

  Serial.println();
  Serial.println(F("Dual TMC2209 controller ready."));
  printConfig();
  printStatus();
}

void loop(void)
{
  pollSerial();
  m1.run();      // non-blocking; generates one step when one is due
  m2.run();
}
