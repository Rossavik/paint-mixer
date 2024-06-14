#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool up = LOW;
bool down = LOW;
bool select = LOW;
bool left = LOW;
bool right = LOW;
int menu = 1;
int manualMenu = 1;
int fill = 1;

int totalMl=100;
int paintP=0;
int hardenerP=0;
int thinnerP=0;



const int btnPin = A0; // Button pin
bool inManual = false; // Are we viewing a menu item page
bool inFill=false;

void setup() {
  Serial.begin(57600);
  lcd.begin(16, 2); // set up the LCD's number of columns and rows.
  pinMode(btnPin, INPUT); // Button pin as input
 
  HelloWorld();
  updateMenu();
}

void loop() {
menuChoise();


}

void HelloWorld(){
 lcd.setCursor(0,0);
  lcd.print("Hello TrollLabs");
  delay(2000);
 /* lcd.clear();
  //lcd.setCursor(0,0)
  lcd.print("This paintmixer");
  lcd.setCursor(0,1);
  lcd.print("is part of a");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("is part of a");
  lcd.setCursor(0,1);
  lcd.print("master project");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("master project");
  lcd.setCursor(0,1);
  lcd.print("from 2024.");
  delay(4000);
  lcd.clear();*/
}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">1. Fill");
      lcd.setCursor(0, 1);
      lcd.print(" 2. Manual");
      break;
    case 2:
      lcd.clear();
      lcd.print(" 1. Fill");
      lcd.setCursor(0, 1);
      lcd.print(">2. Manual");
      break;
    case 3:
      menu = 2;
      break;
  }
}

void updateManualMenu() {
  switch (manualMenu) {
    case 0:
      manualMenu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">1. Paint");
      lcd.setCursor(0, 1);
      lcd.print(" 2. Hardener");
      break;
    case 2:
      lcd.clear();
      lcd.print(" 1. Paint");
      lcd.setCursor(0, 1);
      lcd.print(">2. Hardener");
      break;
    case 3:
      lcd.clear();
      lcd.print(">3. Thinner");
      lcd.setCursor(0, 1);
      lcd.print(" 4. All");
      break;
      case 4:
      lcd.clear();
      lcd.print(" 3. Thinner");
      lcd.setCursor(0, 1);
      lcd.print(">4. All");
      break;
    case 5:
      manualMenu = 4;
      break;
  }
}

void getKeyinput() {
  int key = analogRead(A0);
  if (key > 1000) 
  {
    up=LOW;
    down=LOW;
    select =LOW;
    left=LOW;
    right=LOW;
  }
  else{
    delay(50);
    key = analogRead(A0);
    }
 if (key < 125){right=HIGH;}// Right Key
 else if (key < 200){up=HIGH;} // Up Key
 else if (key < 400){down=HIGH;} // Down Key
 else if (key < 600){left=HIGH;} //Left Key
 else if (key < 800){select=HIGH;} //Select key
}

void menuChoise(){
  getKeyinput();
   if (left){
    menu=1;
       while (left)
      {
      getKeyinput();
       }
       updateMenu();
  }
  if (down){
    menu++;
    updateMenu();
    delay(100);
    while (down)
    {
      getKeyinput();
       }
  }
  if (up){
    menu--;
    updateMenu();
    delay(100);
    while(up)
    {
      getKeyinput();
      
    }
  }
  if (right){
    updateMenu();
    delay(100);
    while (right)
    {
      getKeyinput();
    }
    executeMenu();
  }
}

void executeMenu() {
  switch (menu) {
    case 1:
      Fill();
      break;
    case 2:
      manualMenuChoise();
      break;
  }
}

void manualMenuChoise(){
  manualMenu=1;
  updateManualMenu();
  inManual=true;
  while (inManual){
    getKeyinput();
    if (left){
      inManual=false;
      menu=2;
        while (left)
        {
        getKeyinput();
        }
        
        updateMenu();
    }
    if (down){
      manualMenu++;
      updateManualMenu();
      delay(100);
      while (down)
      {
        getKeyinput();
        }
    }
    if (up){
      manualMenu--;
      updateManualMenu();
      delay(100);
      while(up)
      {
        getKeyinput();
      
      }
    }
    if (right){
      updateManualMenu();
      delay(100);
      while (right)
      {
        getKeyinput();
      }
    }  
      if (select){
      updateManualMenu();
      delay(100);
      while (select)
      {
        Serial.print("M");
        Serial.print(manualMenu);
        Serial.println("");

        lcd.clear();
        if(manualMenu==1){lcd.print("Paint");}
        else if(manualMenu==2){lcd.print("Hardener");}
        else if(manualMenu==3){lcd.print("Thinner");}
        else{lcd.print("All");}
        lcd.setCursor(0,1);
        lcd.print("Running...");
        getKeyinput();
      }
      updateManualMenu();
    }
  }
}

void Fill(){
  fill=1;
  updateFill();
  inFill=true;
  while (inFill){
    thinnerP=100-paintP-hardenerP;
    getKeyinput();
    if (left){
      fill--;
        while (left)
        {
        getKeyinput();
        }
        updateFill();
    }
    if (up){
        if (fill==1){totalMl+=50;}
        else if (fill==2){paintP+=5;}
        else if (fill==3){hardenerP+=5;}

      while (up)
        {
        getKeyinput();
        }
        updateFill();
    }
    if (down){
        if (fill==1){totalMl-=50;}
        else if (fill==2){paintP-=5;}
        else if (fill==3){hardenerP-=5;}

      while (down){
        getKeyinput();
        }
        updateFill();
    }
    if (right){
      fill++;
       while(right) {
        getKeyinput();
        }
        updateFill();
    }
     if (select){
       if(fill==5){
        Serial.print("F");
        Serial.print(totalMl);
        Serial.print(", ");
        Serial.print(paintP);
        Serial.print(", ");
        Serial.print(hardenerP);
        Serial.print(", ");
        Serial.print(thinnerP);
        Serial.println("");
        fill=7;
       }
       while(select) {
        getKeyinput();
        }
        updateFill();
    }
  }
}

void updateFill(){
  switch(fill){
    case 0:
    inFill=false;
    fill=1;
    menu=1;
    updateMenu();
    break;
    case 1:
      lcd.clear();
      lcd.print("Set Total ml:");
      lcd.setCursor(0, 1);
      lcd.print(totalMl);
      lcd.print("ml");
    break;
    case 2:
      lcd.clear();
      lcd.print("Set % Paint:");
      lcd.setCursor(0, 1);
      lcd.print(paintP);
      lcd.print("%");
    break;
    case 3:
      lcd.clear();
      lcd.print("Set % Hardener:");
      lcd.setCursor(0, 1);
      lcd.print(hardenerP);
      lcd.print("%");
    break;
    case 4:
      lcd.clear();
      lcd.print("Thinner:");
      lcd.setCursor(0, 1);
      lcd.print(thinnerP);
      lcd.print("%");
    break;
    case 5:
    lcd.clear();
    lcd.print("Tot   P   H   T");
    lcd.setCursor(0, 1);
    lcd.print(totalMl);
    lcd.print("  ");
    lcd.print(paintP);
    lcd.print("% ");
    lcd.print(hardenerP);
    lcd.print("% ");
    lcd.print(thinnerP);
    lcd.print("%");
    break;
    case 6:
    fill=5;
    break;
    case 7:
  

  for (int i = 0;i < 10; i++) {
    lcd.clear();
    lcd.print(totalMl);
    lcd.print("ml");
    lcd.setCursor(0,1);
    lcd.print("Filling");
    delay(500);
    lcd.clear();
    lcd.print(totalMl);
    lcd.print("ml");
    lcd.setCursor(0,1);
    lcd.print("Filling.");
    delay(500);
    lcd.clear();
    lcd.print(totalMl);
    lcd.print("ml");
    lcd.setCursor(0,1);
    lcd.print("Filling..");
    delay(500);
    lcd.clear();
    lcd.print(totalMl);
    lcd.print("ml");
    lcd.setCursor(0,1);
    lcd.print("Filling...");
    delay(500);
  }
  fill=1;
  inFill=false;
  menu=1;
  updateMenu();
  }
}
