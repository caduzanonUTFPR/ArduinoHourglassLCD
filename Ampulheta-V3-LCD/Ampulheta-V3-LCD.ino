#include "Arduino.h"
#include "LedControl.h" //controle das matrizes
#include "Delay.h"
#include <MPU6050_tockn.h> //acelerômetro mpu6050
#include <LiquidCrystal.h>
#define MATRIX_A  0
#define MATRIX_B  1
MPU6050 mpu6050(Wire);
#define ACC_THRESHOLD_LOW -25
#define ACC_THRESHOLD_HIGH 25
// matrizes
#define PIN_DATAIN 6
#define PIN_CLK 4
#define PIN_LOAD 5
// acelerômetro
#define PIN_X  mpu6050.getAngleX()
#define PIN_Y  mpu6050.getAngleY()
// pinos dos botões
#define PIN_BUTTON 2 //Adiciona
#define PIN_BUTTON2 3 //Subtrai/Troca de Modo
#define PIN_BUZZER 17
// leva em consideração como as matrizes estão montadas
#define ROTATION_OFFSET 90
// em millisegundos
#define DEBOUNCE_THRESHOLD 500
#define DELAY_FRAME 50
#define MODE_HOURGLASS 0
#define MODE_SETMINUTES 1
#define MODE_SETSECONDS 2

LiquidCrystal lcd(8, 7, 12, 11, 10, 9);  //RS, E, D4, D5, D6, D7
float delayMinutes = 0;
float delaySeconds = 20;
float dura = 0;
float delayMilisecs = (dura/60)*1000;
int mode = MODE_HOURGLASS; //ampulheta é o modo inicial
int gravity;
LedControl lc = LedControl(PIN_DATAIN, PIN_CLK, PIN_LOAD, 2);
NonBlockDelay d;
int resetCounter = 0;
bool alarmWentOff = false;
unsigned long pulsa;
//função para mostrar o tempo que está sendo contado lá no LCD
void displayTime(int seconds, int minutes) {
  lcd.setCursor(13, 2);
  lcd.print(minutes / 10 % 10);
  lcd.print(minutes % 10);
  lcd.print(":");
  lcd.print(seconds / 10 % 10);
  lcd.print(seconds % 10);
}
//função para soar o alarme e indicar no LCD que a ampulheta terminou a contagem
void alarm() {
  lcd.setCursor(0,3);
  lcd.print("======Terminei======");
  for (int i=0; i<5; i++) {
    tone(PIN_BUZZER, 440, 100);
    delay(500);
  }
}
//Detectar orientação usando o acelerômetro:
int getGravity() {
  int x = mpu6050.getAngleX();
  int y = mpu6050.getAngleY();
  if (y < ACC_THRESHOLD_LOW)  { return 90;   }
  if (x > ACC_THRESHOLD_HIGH) { return 0;  }
  if (y > ACC_THRESHOLD_HIGH) { return 270; }
  if (x < ACC_THRESHOLD_LOW)  { return 180; }
}

int getTopMatrix() {
  return (getGravity() == 90) ? MATRIX_A : MATRIX_B;
}
int getBottomMatrix() {
  return (getGravity() != 90) ? MATRIX_A : MATRIX_B;
}
// obtém o delay entre cada 'queda' de partícula (em segundos)
float getDelayDrop() {
  // como são exatamente 60:
  return delayMinutes + delaySeconds/60;
}
bool moveParticle(int addr, int x, int y) {
  if (!lc.getXY(addr, x, y)) {
    return false;
  }

  bool can_GoLeft = canGoLeft(addr, x, y);
  bool can_GoRight = canGoRight(addr, x, y);

  if (!can_GoLeft && !can_GoRight) {
    return false; // está presa
  }

  bool can_GoDown = canGoDown(addr, x, y);

  if (can_GoDown) {
    goDown(addr, x, y);
  } else if (can_GoLeft && !can_GoRight) {
    goLeft(addr, x, y);
  } else if (can_GoRight && !can_GoLeft) {
    goRight(addr, x, y);
  } else if (random(2) == 1) { // pode mover para a esquerda e para a direita, mas não para baixo
    goLeft(addr, x, y);
  } else {
    goRight(addr, x, y);
  }
  return true;
}
void fill(int addr, int maxcount) {
  int n = 8;
  byte x, y;
  int count = 0;
  for (byte slice = 0; slice < 2*n-1; ++slice) {
    byte z = slice<n ? 0 : slice-n + 1;
    for (byte j = z; j <= slice-z; ++j) {
      y = 7-j;
      x = (slice-j);
      lc.setXY(addr, x, y, (++count <= maxcount));
    }
  }
}
coord getDown(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y+1;
  return xy;
}
coord getLeft(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y;
  return xy;
}
coord getRight(int x, int y) {
  coord xy;
  xy.x = x;
  xy.y = y+1;
  return xy;
}
void resetTime() {
  for (byte i = 0; i < 2; i++) {
    lc.clearDisplay(i);
  }
  fill(getTopMatrix(), 60);
  d.Delay(getDelayDrop()*1000);
}
bool canGoLeft(int addr, int x, int y) {
  if (x == 0) return false; // indisponível
  return !lc.getXY(addr, getLeft(x, y)); // pode ir para a posição se estiver vazia
}
bool canGoRight(int addr, int x, int y) {
  if (y == 7) return false; // indisponível
  return !lc.getXY(addr, getRight(x, y)); // pode ir para a posição se estiver vazia
}
bool canGoDown(int addr, int x, int y) {
  if (y == 7) return false; // indisponível
  if (x == 0) return false; // indisponível
  if (!canGoLeft(addr, x, y)) return false;
  if (!canGoRight(addr, x, y)) return false;
  return !lc.getXY(addr, getDown(x, y)); // pode ir para a posição se estiver vazia
}
void goDown(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getDown(x, y), true);
}
void goLeft(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getLeft(x, y), true);
}
void goRight(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getRight(x, y), true);
}
int countParticles(int addr) {
  int c = 0;
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      if (lc.getXY(addr, x, y)) {
        c++;
      }
    }
  }
  return c;
}
//faz com que uma partícula vá de uma matriz para a outra
boolean dropParticle() {
  if (d.Timeout()) {
    d.Delay(getDelayDrop()*1000);
    if (gravity == 0 || gravity == 180) {
      if ((lc.getRawXY(MATRIX_A, 0, 0) && !lc.getRawXY(MATRIX_B, 7, 7)) ||
          (!lc.getRawXY(MATRIX_A, 0, 0) && lc.getRawXY(MATRIX_B, 7, 7))
         ) {
        lc.invertRawXY(MATRIX_A, 0, 0);
        lc.invertRawXY(MATRIX_B, 7, 7);
        tone(PIN_BUZZER, 440, 5);
        return true;
      }
    }
  }
  return false;
}
//faz a transversão da matriz e confere se partículas precisam ser movidas
bool updateMatrix() {
  int n = 8;
  bool somethingMoved = false;
  byte x, y;
  bool direction;
  for (byte slice = 0; slice < 2*n-1; ++slice) {
    direction = (random(2) == 1); // aleatoriza se será escaneado da esquerda pra direita ou da direita pra esquerda, a partícula não cai sempre na mesma direção
    byte z = slice<n ? 0 : slice-n + 1;
    for (byte j = z; j <= slice-z; ++j) {
      y = direction ? (7-j) : (7-(slice-j));
      x = direction ? (slice-j) : j;
      if (moveParticle(MATRIX_B, x, y)) {
        somethingMoved = true;
      };
      if (moveParticle(MATRIX_A, x, y)) {
        somethingMoved = true;
      }
    }
  }
  return somethingMoved;
}
void resetCheck() {
  int z = analogRead(A3);
  if (z > ACC_THRESHOLD_HIGH || z < ACC_THRESHOLD_LOW) {
    resetCounter++;
    Serial.println(resetCounter);
  } else {
    resetCounter = 0;
  }
  if (resetCounter > 20) {
    Serial.println("RESET!");
    resetTime();
    resetCounter = 0;
  }
}
void displayLetter(char letter, int matrix) {
  lc.clearDisplay(matrix);
  if (letter == 'M') {
    lc.setXY(matrix, 1, 4, true);
    lc.setXY(matrix, 2, 3, true);
    lc.setXY(matrix, 3, 2, true);
    lc.setXY(matrix, 4, 1, true);
  
    lc.setXY(matrix, 3, 6, true);
    lc.setXY(matrix, 4, 5, true);
    lc.setXY(matrix, 5, 4, true);
    lc.setXY(matrix, 6, 3, true);
  
    lc.setXY(matrix, 4, 2, true);
    lc.setXY(matrix, 4, 3, true);
    lc.setXY(matrix, 5, 3, true);
  }
  if (letter == 'S') {
    lc.setXY(matrix, 1, 5, true);
    lc.setXY(matrix, 2, 6, true);  
      
    lc.setXY(matrix, 3, 6, true);
    lc.setXY(matrix, 4, 5, true);

    lc.setXY(matrix, 3, 3, true);
    lc.setXY(matrix, 4, 4, true);    
    
    lc.setXY(matrix, 3, 2, true);
    lc.setXY(matrix, 4, 1, true);
  
    lc.setXY(matrix, 5, 1, true);
    lc.setXY(matrix, 6, 2, true); 
  }
}
void renderSetMinutes() {
  fill(getTopMatrix(), delayMinutes);
  displayTime(delaySeconds,delayMinutes);
  displayLetter('M', getBottomMatrix());
  lcd.setCursor(0,1);
  lcd.print("Config.  ");
  lcd.setCursor(0,2);
  lcd.print("Minutos ");
}
void renderSetSeconds() {
  fill(getTopMatrix(), delaySeconds);
  displayTime(delaySeconds,delayMinutes);
  displayLetter('S', getBottomMatrix());
  lcd.setCursor(0,1);
  lcd.print("Config.  ");
  lcd.setCursor(0,2);
  lcd.print("Segundos");
}
//função do botão usado para aumento
void btnAdd() {
  Serial.println("Aumento");
  if (mode == MODE_SETSECONDS) {
    delaySeconds = constrain(delaySeconds + 1, 0, 59);
    renderSetSeconds();
  } else if (mode == MODE_SETMINUTES) {
    delayMinutes = constrain(delayMinutes + 1, 0, 60);
    renderSetMinutes();
  }
  //Serial.print("Delay: ");
  //Serial.println(getDelayDrop());
}
//função do botão usado para diminuição
void btnSubtract() {
  Serial.println("Troca/Dimunuição  ");
  if (mode == MODE_SETSECONDS) {
    delaySeconds = constrain(delaySeconds - 1, 0, 59);
    renderSetSeconds();
  } else if (mode == MODE_SETMINUTES) {
    delayMinutes = constrain(delayMinutes - 1, 0, 60);
    renderSetMinutes();
  }
}
//função para a troca de modos
void botao1(unsigned long pulsa){
  if(pulsa>50000){
      mode = (mode + 1) % 3;
      Serial.print("Modo foi trocado para: "); 
      if (mode == MODE_SETMINUTES) {
        Serial.println("Config. Minutos");
        lc.backup(); // só precisamos voltar quando formos de MODE_HOURGLASS para MODE_SETMINUTES
        renderSetMinutes();
      }
      if (mode == MODE_SETSECONDS) {
        Serial.println("Config. Segundos");
        renderSetSeconds();
      }
      if (mode == MODE_HOURGLASS) {
        Serial.println("Ampulheta");
        lc.clearDisplay(0);
        lc.clearDisplay(1);
        lc.restore();
        resetTime();
        lcd.setCursor(0,1);
        lcd.print("Ampulheta ");
        lcd.setCursor(0,2);
        lcd.print("Digital   ");
      }
    }else{
      if (pulsa>=500){
        if (mode != MODE_HOURGLASS){
          btnSubtract();
        }
      }
    }    
  //}
}
void tempo1(){
    pulsa = 0;    
    while(!digitalRead(PIN_BUTTON)){
      pulsa++;
    }
    botao1(pulsa);    
}
//callback do botão, com um "debouncer" - troca entre os modos
void buttonPush() {
    tempo1();    
}
volatile unsigned long lastButton2PushMillis;
void button2Push() {
  if ((long)(millis() - lastButton2PushMillis) >= DEBOUNCE_THRESHOLD) {
    if (mode != MODE_HOURGLASS){
      btnAdd();
    }
  }
}
//Setup
void setup() {
  Serial.begin(9600);
  //inicia LCD
  lcd.begin(20, 4);
  //printa "Calibrando Acelerometro" enquanto o sensor não é inicializado
  lcd.setCursor(0,0);  
  lcd.print("Calibrando");
  lcd.setCursor(0,1);
  lcd.print("Acelerometro");
  //cálculo da posição inicial do sensor
  mpu6050.calcGyroOffsets(true);
  randomSeed(analogRead(A0));
  //inicia o sensor
  mpu6050.begin();
  //printamos as mensagens no LCD
  lcd.setCursor(0,0);
  lcd.print("   (+)    | Modo/(-)");
  lcd.setCursor(0,1);
  lcd.print("Ampulheta |  Tempo ");
  lcd.setCursor(0,2);
  lcd.print("Digital   |  XX:XX ");
  lcd.setCursor(0,3);
  lcd.print("          |         ");
  // incia botões
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonPush, LOW);  
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON2), button2Push, RISING);
  displayTime(delaySeconds,delayMinutes);
  // inicia matrizes
  for (byte i = 0; i < 2; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
  }
  resetTime();
}
//loop principal
void loop() {
  delay(DELAY_FRAME);
  mpu6050.update();
  // atualiza a configuração de rotação
  gravity = getGravity();
  lc.setRotation((ROTATION_OFFSET + gravity) % 360);
  bool moved = updateMatrix();
  bool dropped = dropParticle();
  // cuida dos modos especiais
  if (mode == MODE_SETMINUTES) {
    renderSetMinutes(); return;
  } else if (mode == MODE_SETSECONDS) {
    renderSetSeconds(); return;
  }
  // soa um alarme quando tudo está na parte de baixo
  if (!moved && !dropped && !alarmWentOff && (countParticles(getTopMatrix()) == 0)) {
    alarmWentOff = true;
    alarm();
  }
  // reseta o alarme quando uma partícula cai 
  if (dropped) {
    alarmWentOff = false;
    lcd.setCursor(0,3);
    lcd.print("          |         ");
  }
}