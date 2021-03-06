#ifndef __config_H
#define __config_H


#define Baudrate 115200  //for 8mhz use 38400


// Radio pipe addresses. You can save here up to 255 different addresses 

#define debug_mode // debugging replies

#define power_led 3
#define pot_sensor A3





//
//#define cmd_request_firmware_edition 'F' //128
//#define cmd_change_color             'C' //008 + unit select
//#define cmd_analog_mode_start    'A'//144
//#define cmd_analog_read_stop     'S'//152
//#define cmd_maxValue_mode_start     'M'//152



//commands
//responds



#endif //__config_H


/*


 +-------+-------++-------+-------+---------------+             +-------+-------+---------------++---------------+
 |command||  node||   seperrator  |   number      |             |   seperrator  |   number      ||   end of      |
 | type  ||   ID ||               |               |  up to      |               |               ||   packet      |
 +---------------++---------------+---------------+  5 numbers  +---------------+---------------++---------------+
 |1ascii ||1ascii||    1ascii     | up to 6 digits|  . . .      |    1ascii     | up to 6 digits|| one ascc      |
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+             +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 |a to Z ||  1-5 ||     "/"       |number ascii   |             |     "/"       |number ascii   ||  "/n"         |
 +-------+-------++-------+-------+---------------+             +-------+-------+---------------++---------------+

example
 "F1/100/101/102/103/104/n"
 this string will send to node 1 the command assigned to F with 4 different number fields.

C\n search for devices replies with the number of devices 
B\n











#define cmd_request_firmware_edition 'F' //128
#define cmd_change_color             'C' //008 + unit select
#define cmd_analog_mode_start    'A'//144
#define cmd_analog_read_stop     'S'//152
#define cmd_maxValue_mode_start     'M'//152

 
 
 
 +-------+-------++-------+-------+---------------+----------------+
 |command|command||  unit |       |   ignore      |                
 | type  |  ID   ||   ID  | ignore|               |                |     P random random
 +---------------++---------------+---------------+    firmware    |
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|    edition     |   send     80 16 rnd
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |   respond  96 16 3 
 |0 1 0 1|0 0 0 0|| 0-15  |   x   |     0-255     |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+---------------+----------------+
 |command|command||  unit | ignore|   ignore      |                
 | type  |  ID   ||   ID  |       |               |                |   Q random random (id=1)  the device corresponds with unit ID response
 +---------------++---------------+---------------+    set device  |
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|      ID        |   send     81 16 rnd
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |   respond  97 16 0 
 |0 1 0 1|0 0 0 1|| 0-15  |   x   |    ignore     |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |mode   | threshold     |                |R      MODE ON
 | type  |  ID   ||   ID  |On/Off |               |                |       send     82 17 0  threshold=0
 +-------+-------++-------+-------+-------+-------+  key up/down   |       respond  98 17 val  when high
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|                |       respond  98 16 0    when low
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |       MODE OFF
 |0 1 0 1|0 0 1 0|| 0-15  |x x x x|     0-255     |                |       send     82 16 0
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |  red  | green | blue  |                |
 | type  |  ID   ||   ID  | ratio | ratio | ratio |                |Sww
 +-------+-------++-------+-------+-------+-------+ Set RGB ratio 1|     send     83 24 128 R=8 g=8 b=0
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|                |     respond  NO
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |
 |0 1 0 1|0 0 1 1|| 0-15  | 0-15  | 0-15  | 0-15  |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+---------------+----------------+     led ON
 |command|command||  unit | led   |      led      |                |     send     84 16 128     light intensity=128 50%
 | type  |  ID   ||   ID  |       |   intensity   |                |T    respond  NO
 +-------+-------++-------+-------+---------------+   Led On/Off   |     led OFF      
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|                |     send     84 16 0     
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |     respond  NO
 |0 1 0 1|0 1 0 0|| 0-15  |ignore |     0-255     |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+---------------+----------------+
 |command|command||  unit |logic  |    led        |                |              logic 0 means the light is on when there is force
 | type  |  ID   ||   ID  |  state|  intensity    |                |              logic 1 means the light is off when there is force
 +-------+-------++-------+-------+---------------+   Led mode     |U             state 0 means the mode is off
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0| follow  sensor |              state 1 means the mode is on
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   density      |              led intensity>0 means that led lights up at this brightness 
 |0 1 0 1|0 1 0 1|| 0-15  |  0/1  |               |                |              85 21 0
 +-------+-------++-------+-------+---------------+----------------+              85 17 0
 +-------+-------++-------+-------+-------+-------+----------------+              85 21 125
 |command|command||  unit |  red  | green | blue  |                |              85 17 125
 | type  |  ID   ||   ID  | ratio | ratio | ratio |                |              85 16 0
 +-------+-------++-------+-------+-------+-------+ request        |  
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|   unit id      |V     86  16  0
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |
 |0 1 0 1|0 1 1 0|| 0-15  | 0-15  | 0-15  | 0-15  |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |mode   |    ignore     |                |
 | type  |  ID   ||   ID  |On/Off |               |                |W     KEY UP DOWN MODE NEEDS TO BE ON
 +-------+-------++-------+-------+-------+-------+  aftertouch    |
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|                |         87 17 0
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |         87 16 0
 |0 1 0 1|0 1 1 1|| 0-15  |x x x x|     0-255     |                |
 +-------+-------++-------+-------+---------------+----------------+
  +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |mode   | ignore        |                |
 | type  |  ID   ||   ID  |On/Off |               |                |     88 16 0 
 +-------+-------++-------+-------+-------+-------+  battery       |
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|     level      |          
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |         
 |0 1 0 1|1 0 0 0|| 0-15  |x x x x|     0-255     |                |
 +-------+-------++-------+-------+---------------+----------------+
 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |time   |  brightness   |                |
 | type  |  ID   ||   ID  |  logic|               |                | 89   logic  0 fade out          time 0=0
 +-------+-------++-------+-------+-------+-------+  fade          |      logic  1 fade in           time 1=250ms
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|     in/out     |      logic  2 fade out/in       time 2=500ms
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |      logic  3 fade in/out       time 3=750ms
 |0 1 0 1|1 0 1 0|| 0-15  |0-3 0-3|     0-128     |                |
 +-------+-------++-------+-------+---------------+----------------+

 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |logic  | brightness    |                |
 | type  |  ID   ||   ID  |state  |               |                | 90
 +-------+-------++-------+-------+-------+-------+  default       |
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|     brightness |          
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |         
 |0 1 0 1|1 0 1 0|| 0-15  |x x x x|     0-255     |                |
 +-------+-------++-------+-------+---------------+----------------+

 +-------+-------++-------+-------+-------+-------+----------------+
 |command|command||  unit |time   | brightness    |                |
 | type  |  ID   ||   ID  |  logic|               |                | 91   logic  0 fade out          time 0=0
 +-------+-------++-------+-------+-------+-------+  fade  mode    |      logic  1 fade in           time 1=250ms
 |7 6 5 4 3 2 1 0||7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|     in/out     |      logic  2 fade out/in       time 2=500ms
 +-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                |      logic  3 fade in/out       time 3=750ms
 |0 1 0 1|1 0 1 0|| 0-15  |0-3 0-3|     0-128     |                |
 +-------+-------++-------+-------+---------------+----------------+


  */
