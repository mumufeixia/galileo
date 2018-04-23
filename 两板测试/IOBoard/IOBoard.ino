#include <Event.h>
#include <Timer.h>
#include <SPI.h>

int sensor1 = 2;//
int sensor2 = 3;//
int sensor3 = 4;//�
int kick = 6; //
int stopMotor = 7; //
int photograph = 5; //
unsigned int sensor1Num = 0;//
unsigned int sensor2Num = 0;//�
unsigned int matchSensor2Num = 0;
unsigned int sensor3Num = 0;//
unsigned int photographNum = 0;//
unsigned int kickNum = 0;//
unsigned int stopMotorNum = 0;//
unsigned long sensor2Time[6] = {0}; //
int sensor2Index = 0;
int fiber_1_2_Time = 0; //
int fiber_2_3_Time = 0; //
int kickTime = 50; //
int timeDiff = 0; //�


Timer timerKick;



void setup() {
  Serial.begin(9600);
  delay(1000);


  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(photograph, OUTPUT);
  pinMode(kick, OUTPUT);
  pinMode(stopMotor, OUTPUT);
  attachInterrupt(sensor1, phoGraph, FALLING);//
  attachInterrupt(sensor2, fber2, FALLING);//
  attachInterrupt(sensor3, fber3, FALLING);//

}

void loop() {
  timerKick.update();
}

void timerIsr()
{
  unsigned long timeTmp = millis();
  digitalWrite(kick, LOW);
  Serial.print("kick :");
  Serial.print(sensor2Num);
  Serial.print(" :stop:");
  Serial.println(timeTmp);
}
void phoGraph()
{
  unsigned long timeTmp = millis();
  Serial.print("phoGraph :");
  Serial.print(++sensor1Num);
  Serial.print(" :");
  Serial.println(timeTmp);
}

void fber2()
{
  unsigned long timeTmp = millis();

  digitalWrite(kick, HIGH);   // sets the LED on
  timerKick.after(kickTime, timerIsr);
  Serial.print("kick :");
  Serial.print(++sensor2Num);
  Serial.print("Strart :");
  Serial.println(timeTmp);
}

void fber3()
{

}


