# Programming Embedded Project..
### This project aims in monitoring environmental conditions in a prototyped environment  
Sensor Pico which is the target device reads soil moisture sensor(SEN0114), air quality sensor and BME680  
It also send an interrupt to master when soil moisture is measured dry.  
  
The master Pico(Base station) continuously reads the sensor pico in every 3s  
whenever an interrupt occurs the dry indicator led blinks informing to water plant. After the interrupt is serviced  
the led is then turned off when the recieved value goes back to normal.  

