## Arduino Based PWM Control for 3 Wire PWM based IACV Idle Air Control Valve
 Useful for triggering and testing 3 wire IACVs where 2 coils are brought to ground in an either static ground + PWM or PWM + inverse PWM system. 


    WIRING FOR 3 Wire IACV Honda / Acura 2001 CL / TL 2001 J32 and J35 possibly others. 
       For inverse PWM hook up another mosfet on P11 and enable IACVInverseOutputEnabled in the configuration replacing the ground connection of the IACV
           with the inverse PWM signal.

                               MOSFET                   IACV
                           Gate  Drain Source       P1   P2   P3
                            |      |      |         |     |    |
    Arduino                 |      |      |         |     |    |
        ->P10 - - - - - - - |      |- - -^|^- - - -^|^- -^|^- -|
                                          |         |     |
        ->GND - - - - - - - - - - - - - - |- - - - -|     |
                                          |               |
    12V Source                            |               |
                                          ^               |
       ->12V - - - - - - - - - - - - - - - - - - - - - - - 
                                          ^          
       ->GND - - - - - - - - - - - - - - -|

    Notes: 
            |       
      A - -^|^- -   Indicates that the wire from L to R does *not* connect. EG Mosfet DRAIN directly connects to IACV P3 and does not touch the other wires. 
            |

      If using a benchtop power supply ensure that the grounds are all equal between the Arduino power and the 12V source. Using a battery would work as well. 

      During my testing I was able to get smooth articulation of the valve down to ~100 HZ although jitter was noticable. Below 75 HZ was unusable as it caused the valve
          to retract between pulses. 200+ HZ is optomial and very smooth running was found around 500 - 1000HZ. 
      
      This driver is designed to control coils that have an internal layout as follows:  
  
                     
                 P2 ( VCC )
                     |
                - - - - - - - 
                /           /
               COIL       COIL
                /           /
                |           | 
                |           |
                P1          P3
