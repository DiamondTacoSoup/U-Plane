# Dual  stepper controller (ESP32-S2)

## Files

| File | Purpose |
| --- | --- |
| `dual_stepper_tmc2209.ino` | The sketch for the esp-32 |


## Wiring summary (AI generated formatting disclosure)

```
ESP32-S2                 TMC2209 #1            TMC2209 #2
 GPIO18  ---------------> STEP
 GPIO17  ---------------> DIR
 GPIO16  --------------------------------------> STEP
 GPIO15  --------------------------------------> DIR
 GPIO14  ---------------> EN     (shared) -----> EN
 3V3     ---------------> VIO                   VIO
 GND     ---------------> GND                   GND
 24V PSU ---------------> VM  (+100uF)          VM  (+100uF)
                          M1A/M1B/M2A/M2B       M2_1A/M2_1B/M2_2A/M2_2B -> motor coils
 GPIO1   <--- 10k to GND, 92k up to 24V (Batt_Monitor tap)
```

Power ground (`BAT_GND`) and the ESP32 ground must be common — the step/dir
signals have no return path otherwise.

## Command set

| Command | Effect |
| --- | --- |
| `?` | help |
| `STATUS` | position, % of travel, target, battery, driver state |
| `CFG` | travel, resolution, speed, accel, pins |
| `ZERO [1\|2\|ALL]` | declare the current position as 0 % (fully retracted) |
| `MOVE <1\|2\|ALL> <steps>` | go to an absolute step position (0 … `TOTAL_STEPS`) |
| `JOG <1\|2\|ALL> <steps>` | move relative to the current position (`-` retracts) |
| `PCT <1\|2\|ALL> <pct>` | go to a percentage of total extendable travel |
| `PCTREL <1\|2\|ALL> <pct>` | move by a percentage of total extendable travel |
| `SPEED <steps/s>` | cruise speed, 10 … 20000 |
| `ACCEL <steps/s²>` | acceleration, 10 … 200000 |
| `ENABLE` / `DISABLE` | drive the shared `Enable` net |
| `STOP` | decelerate both axes to a halt |
| `BAT` | battery voltage now |

Examples:

```
PCT 1 45.5          motor 1 to 45.5 % of its travel
PCT 2 0             motor 2 fully retracted
JOG 2 -500          motor 2 in 500 steps
PCT ALL 100         both axes to full extension, arriving together
PCTREL 1 -10        motor 1 retracts by 10 % of full travel
```

`PCT ALL` / `MOVE ALL` / `JOG ALL` are coordinated: the axis with the longer
distance gets the higher step rate, so both finish at the same instant.

## notes

- No Homing, Must be manually tuned
- Moves are non-blocking, so `STOP` and `STATUS` respond while the motors run.