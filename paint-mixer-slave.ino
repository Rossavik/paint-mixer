#include <HX711_ADC.h>
#include <EEPROM.h>

const int densityPaint= 1; //g/ml
const int densityHardner= 1; //g/ml
const int densityThinner= 1; //g/ml

//pins
const int Paint=10;
const int Hardner=11;
const int Thinner=12;
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;

//global variables
float lastWeight;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("Starting...");
  loadcellStartup();
  pumpStartup();
}

void loop() {
  // put your main code here, to run repeatedly:

  
  if (Serial.available() > 0) {
    char rChar = Serial.read(); // Read the incoming byte (character)
    Serial.print(rChar); // Echo the received character to the Serial Monitor
    // Determine action based on received character
    switch(rChar) {
      case 'M':
       // Handle manual control based on further serial input
       manual();
        break;
      case 'F':
        // Handle fill operation
        readFill();
        break;
      // Add cases for other operations as needed
    }
  }


//Fill(100,25,25,50);

printLoad(1);

}

float readLoad(){

 static boolean newDataReady = 0;
 
  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get value from the loadcell:
  if (newDataReady) {
      float i = LoadCell.getData();
      newDataReady = 0;
      lastWeight=i;
    }
    return lastWeight;
}

void printLoad(int j){
  for (int i = 0;i < j; i++) {
      float w=readLoad();
      Serial.print("Load_cell output val: ");
      Serial.println(w);
    }

}

void pumpStartup(){
  pinMode(Paint, OUTPUT);
  pinMode(Hardner, OUTPUT);
  pinMode(Thinner, OUTPUT);

  digitalWrite(Paint, HIGH);
  digitalWrite(Hardner, HIGH);
  digitalWrite(Thinner, HIGH);
}

void loadcellStartup(){
  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
  
}

void pumpsOff(){
  digitalWrite(Paint, HIGH);
  digitalWrite(Hardner, HIGH);
  digitalWrite(Thinner, HIGH);
}


void manual(){
  delay(15);
  while (Serial.available() > 0) {
    delay(20);
    char mChar = Serial.read(); // Read the incoming byte (character)
    Serial.print(mChar); // Echo the received character to the Serial Monitor

    // Determine action based on received character
    if (mChar=='1'){digitalWrite(Paint, LOW);}
    else if (mChar=='2'){digitalWrite(Hardner, LOW);}
    else if (mChar=='3'){digitalWrite(Thinner, LOW);}
    else if (mChar=='4'){
      digitalWrite(Paint, LOW);
      digitalWrite(Hardner, LOW);
      digitalWrite(Thinner, LOW);
      }
  }
  pumpsOff();
}

void Fillml(int comp, int ml){ //fills a set amount of ml on top of existing liquid
 int density;
 if(comp==Paint){density=densityPaint;}
 else if(comp==Hardner){density=densityHardner;}
 else if(comp==Thinner){density=densityThinner;}

  float local_w=0;
  float old_w=readLoad();
  while (local_w<ml*density){
  digitalWrite(comp, LOW);
  local_w=readLoad()-old_w;
  Serial.print("Load_cell output val: ");
  Serial.println(local_w);
  }
  digitalWrite(comp, HIGH);
}

void Fillml_accurate(int comp, int ml){ //accuratly fill liquid
  int density=0;
  if(comp==Paint){density=densityPaint;}
  else if(comp==Hardner){density=densityHardner;}
  else if(comp==Thinner){density=densityThinner;}


  float alocal_w=0;
  printLoad(300);
  float aold_w=readLoad();
  Fillml(comp,ml-10); //coarse

  for (int i = 0;i < 300; i++) {
    alocal_w=readLoad()-aold_w;
    Serial.print("Load_cell output val: ");
    Serial.println(alocal_w);
  }

  alocal_w=readLoad()-aold_w;

  if(comp!=Paint){
    while (alocal_w<(ml*density)-1){ //medium
      digitalWrite(comp, LOW);
      delay(500);
      digitalWrite(comp, HIGH);
     
      for (int i = 0;i < 300; i++) {
        alocal_w=readLoad()-aold_w;
        Serial.print("Load_cell output val: ");
        Serial.println(alocal_w);
      }
    }
  }

  while (alocal_w<(ml*density)){ //fine
     digitalWrite(comp, LOW);
     delay(175);
     digitalWrite(comp, HIGH);
     
    for (int i = 0;i < 300; i++) {
    alocal_w=readLoad()-aold_w;
    Serial.print("Load_cell output val: ");
    Serial.println(alocal_w);
    }
    
  }
}

void Fill(float totalml,float PaintPercent, float HardnerPercent,float ThinnerPercent){ //takes in the total amount of wanted paint and the percentages of the different components
  totalml-=0.5;
  float Paintml=(totalml*(PaintPercent/100));
  float Hardnerml=(totalml*(HardnerPercent/100));
  float Thinnerml=(totalml*(ThinnerPercent/100));

  Fillml_accurate(Paint,Paintml);
  delay(500);
  Fillml_accurate(Hardner,Hardnerml);
  delay(500);
  Fillml_accurate(Thinner,Thinnerml);
  delay(500);
}

void readFill(){
  delay(50);
    if (Serial.available() > 0) {
    // Read the incoming string until newline character
    String rString = Serial.readStringUntil('\n');
    
    // Split the string by commas and convert each part to an integer
    int firstCommaIndex = rString.indexOf(',');
    int secondCommaIndex = rString.indexOf(',', firstCommaIndex + 1);
    int thirdCommaIndex = rString.indexOf(',', secondCommaIndex + 1);

    // Convert each substring to an integer
    int Tot = rString.substring(0, firstCommaIndex).toInt();
    int P = rString.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
    int H = rString.substring(secondCommaIndex + 1, thirdCommaIndex).toInt();
    int T = rString.substring(thirdCommaIndex + 1).toInt(); // To the end of the string

    // Print the numbers to confirm
    
   Fill(Tot,P,H,T);
  }
  else{Serial.println("ERROR: Data not recieved.");}
  delay(2000);
}
