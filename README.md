# U-Plane
A twist on a U-boat but with wings. An underwater glider utilising high efficacy air foils and buoyancy control to glide through the water using as little power as possible.

## Why I'm building the U-Plane
I have always wanted to built an underwater UAV, being inspired by the likes of RCtestflight, CPS drone, and local companies such as OCIUS. I am also fascinated with planes and airfoil. I Sail WAZSP's so I am familiar with hydrofoils, and my thinking was what if I combine the two, planes and submarines, into an ultra efficient underwater vehicle. My research showed that Since the 2000's very few blended wing designs have been produced, I wanted to utilise modern CFD to built my own scale flying wing design, combining my interests in planes and baots into one project.

# The Design
XLR5 model:

<img width="892" height="716" alt="image" src="https://github.com/user-attachments/assets/919588bb-8960-4e57-ac8f-1c57785fb54d" />

PCB & Schematic:

<img width="2012" height="1424" alt="image" src="https://github.com/user-attachments/assets/bcfa57e6-50cc-4e63-98c3-b28db183a992" />
<img width="1748" height="1204" alt="image" src="https://github.com/user-attachments/assets/4d77500f-c942-48a8-9af6-c55941e5702b" />
<img width="2076" height="1158" alt="image" src="https://github.com/user-attachments/assets/41e3c2f7-4e9b-4d1b-b88a-a568013341e4" />


# (Not so frequently) asked questions
## Why is the wing hollow
The wing is hollow to reduce the buoyancy differential by allowing the wing to fill mostly with water, this mean I need less ballast weight on the sub. This does laso make the wings significantly weaker, however the forces on them are minimal enough that it will not cause breakages.

## Why are the horizontal stabilisers so big
Flying wings are inherently unstable in the roll direction. There is an aerodynamic phenomena, the dutch roll, where yawing moments couple into a rolling movement which can easily cause the wing to stall or spin out. To counter this massive vertical stabilisers are needed to stabilise the wing, and while this does increase drag; the efficancys of a flying wing design makes up for this other loss in efficiency. 

## Images
<img width="741" height="401" alt="image" src="https://github.com/user-attachments/assets/c817682a-060e-44f3-957c-2fac66764d92" />
<img width="956" height="668" alt="image" src="https://github.com/user-attachments/assets/b8133bdd-3839-46fd-8dab-0aace9da530d" />
<img width="1195" height="711" alt="image" src="https://github.com/user-attachments/assets/72eab8cb-61cb-4788-98c6-c62b6f6c8e48" />

# BOM

| Part | Quantity | Unit Price | Total Price incl. shipping | Link |
| --- | --- | --- | --- | --- |
| NEMA 8 Non-Captive Acme Linear Stepper Motor 0.5A 28.2mm Stack Screw Lead 1mm (0.03937") Lead Length 150mm | 1 | $58.98 | $79.86 | [Link](https://www.omc-stepperonline.com/nema-8-non-captive-acme-linear-stepper-motor-0-5a-28-2mm-stack-screw-lead-1mm-0-03937-lead-length-150mm-8n11s0504sc5-150rs) |
| NEMA 8 External 28.2mm Stack 0.24A Lead 2mm (0.07874") Length 150mm | 1 | $59.90 | $80.78 | [Link](https://www.omc-stepperonline.com/nema-8-external-28-2mm-stack-0-24a-lead-2mm-0-07874-length-150mm-8e11s0244ac5-150rs) |
| Adafruit TMC2209 Stepper Motor Driver Breakout Board | 2 | $17.05 | $40.00 | [Link](https://core-electronics.com.au/adafruit-tmc2209-stepper-motor-driver-breakout-board.html) |
| Arduino Uno | 1 |  |  |  |
| 2S Battery | 1 |  |  |  |
| Injector Syringe 4.5x2.5cm Clear | 12 | $3.25 | $39.00 | [Link](https://www.bunnings.com.au/12pk-topchef-disposable-flavour-marinade-injector-syringe-4-5x2-5cm-clear_p0965116) |
| 100mm x 1m PVC DWV Pipe | 1 | $22.31 | $22.31 | [Link](https://www.bunnings.com.au/holman-100mm-x-1m-pvc-dwv-pipe_p4770090) |
| 100mm PVC DWV Push On Cap | 2 | $3.95 | $7.90 | [Link](https://www.bunnings.com.au/holman-100mm-pvc-dwv-push-on-cap_p4770360) |
| IP68 6 Pin Cable Connector | 1 | $15.00 | $20.00 | [Link](https://au.rs-online.com/web/b/rs-pro/?intcmp=AU-WEB-_-CP-HB-_-MCC_124_0126_APAC-_-rs-pro-accumulate-2026&pn=579) |
| Cable | 15 | $0.86 | $18.90 | [Link](https://www.altronics.com.au/product/w2313-6-core-flat-modular-telephone-cable/) |
| PCB | 5 |  | $20.27 | JLCPCB |
|  |  | **Total (AUD)** | **$329.02** |  |
|  |  | **Total (USD)** | **$231** |  |


