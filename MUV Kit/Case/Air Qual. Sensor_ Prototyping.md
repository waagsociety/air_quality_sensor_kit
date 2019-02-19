Air Qual. Sensor: Prototyping 1


## MUV Air quality sensor case:

**Parts and Sizing:**

25 x: m3 washers
50 x: m3 nuts
2 x: m6 x 25 hook bolt
4 x: m6 nut
4 x: m6 washer
4 x: m5 nut
4 x: m5 washer
2 x: m4.5 wing nut (M4 also fits looser)
2 x: m4.5 wing bolts (m4 also fits looser)
4 x: m4.5 or m4 washers
7 x: m3 x 30mm bolts    
12 x: m3 x 12mm bolts
4 x: m3 x 25mm bolts
4 x: m3 x 15mm bolts


1 x 19.05mm blind grommet
1 x 22.23mm blind grommet
1 x 14.29mm OD 7.94mm ID grommet





**Enclosure internal Dimensions:**
L: 245, W: 190 H: 68mm(without lid)

**Dust sensor, SDS011:**

Has a 190MM cable as standard


Air barb diameter: 7.05
Air barb length: 11

Air hose: 8mm OD 6mm ID
Air hose length: 75mm

Screws: 3 x m3 x 30mm


**Adafruit Feather M0 Basic Proto + PCB + noise sensor:**

Cable: 200mm

Screws: 4 x m3 x 30mm





**Sensors PCB:**

Cable: 200mm

Screws: m3 x 12mm


**Gas sensor w pcb:**

Cable: 200mm




Screws: m3 x 25mm

**Temp/Hum/Pressure sensor:**

Cable: 200mm

Min screw length: m3 x 15mm

**Inner Tube pieces for grip:**


Hose clamp strain relief:
71 x 42mm (x2)

Drain pipe grip pad: 
180 x 42mm

Balcony grip pad:
115 x 42mm(x2)

L section long pad:
135 x 40mm (x2)
w
L section short pads:
83 x 40mm (x2)


## The Brief:

Design a case for the MUV air quality sensor.

**The case should:**

-    Be able to hold, securely, all of the sensors of the kit (dust,temp/press/humid,noise, 2 x N02)
- Allow the proper functioning of all the sensors 

-   Be waterproof and durable enough to withstand wet weather
- Be able to be mounted securely outdoors (e.g. on lamp posts)
- Be easy to make
- Be easy to service
- Be easy to assemble
- Use redily available parts
 
**Challenges:**

I think allowing the sensors acess to the outside with the sensors without allowing moisture in is a big challenge.

The 2 gas sensors, the dust sensor and the temp/ humidity sensor will have to have acess to the outside air to collect data properly. The usb cable for power must also be able to plug in to the main board. The microphone must also be able to record the audio from outside.

I think the best option is to have all of the holes on the bottom to avoid rain gathering in them from above. I will mount a "baseplate" to the screw holes in the bottom of the box. These holes are raised 11.1mm from the bottom of the box, so I will have an 11.1mm gap between the baseplate and the floor of the box where the air holes are. I will then mount the sensors to holes in the baseplate so they can poke through the baseplate to access the airflow. I think that by keeping this 11.1mm gap between the air holes and the sensors, I can stop rain from being able to reach the sensors. I will have to test it and see.

I will have to come up with a way to seal the components to the holes, I think glue or silicone would work but I would like to try to make some gaskets or O-rings myself and see if they stop the moisture. Using these, the sensors could be more easily removable for maintainance.

INSERT IMAGE "Process27"
INSERT IMAGE "Process"

Another potetial problem will be allowing the status of the LED to be visible from the outside. To carry the light from the led to the wall of the case, I will try to use "light piping" with a piece of frosted acrylic. The acryic will will be sealed to a hole in the case wall.

INSERT IMAGE "Process31"

http://blog.stratasys.com/wp-content/uploads/2015/10/3d-printed-light-pipes-e1443786870121.jpg 

If that doesnt work, or if I dont have the time, I will replace the light pipe with a simple Acrylic window.

**Designing & Testing:**

For the outer enclosure of the sensors I will use an off the shelf waterproof enclosure. I will then modify this enclosure to hold the sensors in a way which allows good collection of the data (good airflow). I will make my design for holding the sensors in the enclosure easily resizable for those who can only source other size enclosures.

I chose this waterproof enclosure for this first version of the air quality kit:
<https://www.amazon.de/dp/B0151KNMB2/ref=pe_3044161_185740101_TE_item>

INSERT IMAGE "Process30"

This case is larger than I need but there are many others like it available in different sizes from online stores worldwide. This case had a short delivery time to me so I went with it.

So by modifying my baseplate design (changing the screw hole locations, scaling, changing sensor locations) it can be adapted to fit many of these outdoor enclosures. 

I can mount the sensors to the baseplate simply with screws and nuts, as the PCBs have screw holes in the corners already. 

I decided to cut out the bottom of the enclosure and replace it with a laser cut acrylic plate with holes cut in it for airflow to the sensors. This plate can be modified in response to testing to increase or decrease the airflow by using more or less air flow holes.

INSERT IMAGE "Process14"
INSERT IMAGE "Process11"
INSERT IMAGE "Process15"
INSERT IMAGE "Process12"

The kit must operate in the outdoor setting of a balcony. To secure the kit in this setting. I chose to design another plate from acrylic which can attach to the enclosure. The mounting plate will allow hose clamps to attach for mounting to drain pipes and clamps to attach for mounting to railings.

INSERT IMAGE "Process13"


**Making**

During the process of making my design, I made small tests to see if my measurements were correct in less costly materials such as cardboard and poplar plywood. This was important because I only had oneenough acrylic to cut the final parts once

INSERT IMAGE "Process2"
INSERT IMAGE "Process4"
INSERT IMAGE "Process5"

INSERT IMAGE "Process6"
INSERT IMAGE "Process10"
INSERT IMAGE "Process23"
















