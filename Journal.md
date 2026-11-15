---
title: "U-Plane"
github: "https://github.com/DiamondTacoSoup/U-Plane"
description: "A twist on a U-boat but with wings. An underwater glider utilising high efficacy air foils and  buoyancy control to glide through the water using as little power as possible."
created_at: "2026-10-07"
total_time: "34h 6m"
---

# October 8, 2026: Setting up the github

Read the rules of the comp, set up the Github repo.
In coming days I will upload my cad files.
There is no PCB in this project as intend to use Perfboard instead to simplify the design and prototyping. Similarly there are few 3D printed parts, only the wings are 3d printed with the pressure hull being made of PVC pipe.

![](https://fabricate.hackclub-assets.com/4825a36c99fd928cfd3b3f08943723d7f3ddd5fb34f5e5daf84cb64affe8aaa5/image-png/image.png)

**Total time spent: 1h**

# October 12, 2026: Created the CFD model

Today I created a basic design outline of the U-plane's design in XFLR5.
This program allowed me to easily design an airframe and wing structure using CAD tools and stability/efficiency simulations.

To maximise efficiency I have opted for a flying wing design.
![](https://fabricate.hackclub-assets.com/b9ab91f4073dff3a010dc07c7002df3079c1642c352b0e9ad34e2f872389612a/image-png/image.png)
This design maximises wing area and minimises unnecessary drag; making it the standard for all underwater gliders.

However this design is highly unstable and as such careful simulation is required to ensure stability. In the end the major trade off I had to make in my design was large canted vertical stabilizers to ensure yaw stability.
![](https://fabricate.hackclub-assets.com/0ed3187ce14b95693804b7759f34150e2d2016a72c116078a089634a8106efe7/image-png/image.png)
Pictured here you can see that all dots are on the left of the vertical axis, this indicates that the design is dynamically stable. To reach this point I underwent many trial with slightly tweaked designs until I found the most efficient and stable option. The stability in this design is derived from both canted vertical stabilisers on the wing tips and high wing dihedral.

The wings utilize a MH45 Airfoil profile which is designed for the high lift - low speed - high efficiency scenarios such as this; giving final efficiency values of:
![](https://fabricate.hackclub-assets.com/039c1fa50974c5d5c1262498beebdc0bf5e5005822ca97f0abddf4a70d33664d/image-png/image.png)

**Total time spent: 8h 18m**

# October 15, 2026: Started the cad & created initial BOM

Today I started the CAD in my CAD software of choice, onshape. I imported my XFLR5 wing design using Profile Generator Featurescript. I chose a 100mm PVC pipe as the main pressure vessel as this was what was available at my local hardware store. Same story with the syringes, they are sized based of what was locally available to me. The stepper motors I decided to make NEMA 11 as it is the perfect balance of power and compactness. Attached below are images of the current design.
Things left to design:
1. Electronics mounts
2. Winglets

![](https://fabricate.hackclub-assets.com/bdc4759474fe8c046edcad821a34c4273123512842dbc683fbd37c3e891acba0/image-png/image.png)
![](https://fabricate.hackclub-assets.com/f022c9a36016e201d0903daa8c833020f572e99b4d03c8af842c92d0bbd53ff9/image-png/image.png)
![](https://fabricate.hackclub-assets.com/afc3f105b40e4279ec14192527bd2de860a45f53847e9c81b3adf1a848c02854/image-png/image.png)

I also created a preliminary BOM which is on the GitHub.

**Total time spent: 9h 42m**

# October 27, 2026: Created schematic and PCB (26/10/2026)

# Schematic
I historically have used Kicad for my other projects, however because of its extensive part library I have opted to use EasyEDA for this project. 

I started by creating my schematic, this was largely dictated by my choice of microcontroller (ESP32), and my choice of stepper motor controller (TMC2209). I chose this microcontroller as I have many of them just sitting around. I chose the TMC2209 as it has an operating voltage of 4.75V - 29V meaning I can drive it directly from my 2s battery which is not possible with other alternatives.

The main issue I face was figuring out how to create solder pads, I am embarrassed to admit this look me like and hour D:, However other than that it was a simple matter of placing and tracing the components. 
![](https://fabricate.hackclub-assets.com/ca7898c19d353aa500af768b6ffb909c97b5ef08d5c1c37fc9fcb9b706ab2d7e/image-png/image.png)

# PCB
A major consideration I had to factor in with my PCB design was the trace sizing. As many of these traces will be carrying Amps of current they needed to be sufficiently sized. I used Kicad's inbuilt trace sizing calculator to ensure that the traces were big enough to carry the current demanded of them, that is why the traces are so wide in some areas of the PCB.
![](https://fabricate.hackclub-assets.com/23004348dd0df13af23b55c3b485b7352911a21d61a3c93dd1c722298f1b8096/image-png/image.png)
![](https://fabricate.hackclub-assets.com/9b2bf7a3330dbae3d5e1b646457eb34f9038caa638928e25d5d76c1a0b67b241/image-png/image.png)

**Total time spent: 7h**

# October 27, 2026: Added to Repo, BOM, & journaling (27/10/2026)

Added (ns)FAQ section to github repo, Added design images to github repo, and updated BOM with shipping costs, PCB costs, and currency conversions. I also wrote my journal entry for yesterday as by the time I was done I was way too tired to write and entry.

![](https://fabricate.hackclub-assets.com/5c7e234d59fe051a52233da1af7d512ca9a7385b5a39135d597950c7df181925/image-png/image.png)

**Total time spent: 1h 30m**

# November 11, 2026: Wrote the firmware

**My trials and tribulations**
I have not Journaled in a while as my yearly exams just concluded, I spent today creating the firmware for U-plane. This was quite a rabbit hole as coding is not my forte and I had to learn about stepper motor control drivers. I originally wrote this program to use UART which took a long time before I realised that I was using step control for the Tmc2209 stepper drivers. I then had to completely rewrite the code for the correct wiring/

**The programs function**
Before I write a full autonomous control program I need to first build and test the hydrodynamics of my design, this version of the firmware allows for fine manual control via serial link which enables me to test and refine the dynamics of the craft, it is not the final version of the firmware however it is the best I can do without building and testing the physical craft. As such it provides basic and advanced stepper motor control as both percentages of extent and individual stepping.

**AI disclosure**
I am bad at writing pretty code, I initially wrote this code myself and then ran it through AI for nice formatting. The logic is all mine, the only things the AI changed was the indentation and variable names. Most of the comments are handwritten as well, you can probably tell which are and which aren't but it is a 90/10 split (Human:Ai).
Note: The readme is Handwritten 

![](https://fabricate.hackclub-assets.com/762bc3cc399aadc8a4243fde411360dae5a162190c318251bb5d8b5aabf36261/image-png/image.png)

**Total time spent: 6h 36m**
