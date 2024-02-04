void setup() {
  Serial.begin(9600);       
}
 
void loop() { 
  float sensor_volt;           
  float RS_air;               
  float R0;                   
  float sensorValue=0;

  for(int x = 0 ; x < 500 ; x++){
    sensorValue = sensorValue + analogRead(A0);  
  }

  sensorValue = sensorValue/500.0;          //Hacemos la media tras el bucle
  sensor_volt = sensorValue*(5.0/1023.0);   //Lo convertinos a voltaje
  RS_air = ((5.0*10.0)/sensor_volt)-10.0;   //Dada en el datasheet
  R0 = RS_air/3.6;                          //Calculamos el valor de R0 
  sensorValue=0;
  Serial.print("R0 = "); 
  Serial.println(R0);  
  delay(1000); 
}
