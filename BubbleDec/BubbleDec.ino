#include <TimerOne.h>


#include <SPI.h>
#include <Ethernet.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
typedef struct
{
  unsigned int b_id;//鐢电溂1搴忓彿
  boolean b_isOk;//ok true    ng  false
  boolean b_is2Plate;//鏄惁杩炴澘
  unsigned long b_time;//galileo鏉垮崱璁板綍鏃堕棿
  boolean b_dataStatus;//鏁版嵁瀛樺叆鐘舵�锛宼rue涓哄伐鎺ф満鏁版嵁瀛樺叆锛汧ALSE涓烘暟鎹湭瀛樺叆
} inforResult; //璁板綍鐢电溂1鐨勮繑鍥炵粨鏋滈泦
byte mac[] = { 0x98, 0x4F, 0xEE, 0x05, 0x80, 0x7C};
IPAddress ip(192, 168, 2, 2);
EthernetServer server(5099); //
boolean alreadyConnected = false; // whether or not the client was connected previously
int sensor1 = 2;//浼犳劅鍣�
int sensor2 = 3;//浼犳劅鍣�
int sensor3 = 4;//浼犳劅鍣�
int kick = 6; //韪㈠簾
int stopMotor = 7; //鍋滄鐢垫満
int photograph = 5; //鎷嶇収
//volatile int state = LOW;
unsigned int sensor1Num = 0;//鐢电溂1璁℃暟
unsigned int sensor2Num = 0;//鐢电溂2璁℃暟
unsigned int sensor3Num = 0;//鐢电溂3璁℃暟
unsigned int photographNum = 0;//鎷嶇収璁℃暟
unsigned int kickNum = 0;//韪㈠簾璁℃暟
unsigned int stopMotorNum = 0;//鍋滄満璁℃暟
unsigned long sensor2Time[6] = {0}; //鐢电溂2鏃堕棿璁板綍
int sensor2Index = 0;
int fiber_1_2_Time = 0; //鐢电溂1,2鏃堕棿
int fiber_2_3_Time = 0; //鐢电溂1,2鏃堕棿
int timeDiff = 0; //鏉垮崱鍜屽伐鎺ф満鏃堕棿宸�
inforResult resultSensor1[6];//鐢电溂1妫�祴淇℃伅淇濆瓨
//boolean isWriteLog=false;
//fiber_kick_infor
void setup() {
  Serial.begin(9600);
  delay(1000);
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  //if (Ethernet.begin(mac) == 0) {
  //    Serial.println("Failed to configure Ethernet using DHCP");
  //    // DHCP failed, so use a fixed IP address:
  //    Ethernet.begin(mac, ip);
  //  }
  //  else
  //  {
  //        Serial.println("lalalalalalal");
  //  }
  Serial.println("start");
  // start listening for clients
  server.begin();
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  //  pinMode(photograph, OUTPUT);
  pinMode(kick, OUTPUT);
  pinMode(stopMotor, OUTPUT);
  attachInterrupt(sensor1, phoGraph, FALLING);//褰搒ensor1鐢靛钩淇″彿鍑虹幇涓嬮檷娌挎椂,瑙﹀彂涓柇鍑芥暟phoGraph
  attachInterrupt(sensor2, fber2, FALLING);//褰搒ensor2鐢靛钩淇″彿鍑虹幇涓嬮檷娌挎椂,瑙﹀彂涓柇鍑芥暟fber2
  attachInterrupt(sensor3, fber3, FALLING);//褰搒ensor3鐢靛钩淇″彿鍑虹幇涓嬮檷娌挎椂,瑙﹀彂涓柇鍑芥暟fber3
  for (int i = 0; i < 6; i++)
  {
    resultSensor1[i].b_dataStatus = false;
    sensor2Time[i] = 0;
  }
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    //    if (!alreadyConnected) {
    //      // clead out the input buffer:
    //      client.flush();
    //      client.println("Hello, client!");
    //      alreadyConnected = true;
    //    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      Serial.println("start client");
      char thisChar = client.read();
      if (thisChar == 'a')
      {
        unsigned long timeTmp = millis();
        Serial.print("client say:");
        Serial.println(thisChar);
        char timeStr[100];
        String test1 = "aa_";
        test1.concat(timeTmp);
        test1.toCharArray(timeStr, 100);
        server.write(timeStr);
        Serial.print("server say:");
        Serial.println(timeStr);
      }
      if (thisChar == 'c')
      {
        kickNum++;
        unsigned long timeTmp = millis();
        digitalWrite(kick, HIGH);   // sets the LED on
        Serial.print("Galileo kick :");
        Serial.print(kickNum);
        Serial.print(" :start:");
        Serial.println(timeTmp);

        Timer1.initialize(50000);
        Timer1.attachInterrupt( timerIsr );

        //        delayMicroseconds(50000);                  // waits for a second
        //        digitalWrite(kick, LOW);
        //        timeTmp = millis();
        //        Serial.print("Galileo kick :");
        //        Serial.print(kickNum);
        //        Serial.print(" :stop:");
        //        Serial.println(timeTmp);

      }
      if (thisChar == 'd')
      {
        stopMotorNum++;
        digitalWrite(stopMotorNum, HIGH);   // sets the LED on
        unsigned long timeTmp = millis();
        Serial.print("Galileo stop:");
        Serial.print(stopMotorNum);
        Serial.print(":");
        Serial.println(timeTmp);
      }
      if (thisChar == 'e')
      {
        digitalWrite(stopMotorNum, LOW);   // sets the LED on
        unsigned long timeTmp = millis();
        Serial.print("Galileo start run:");
        Serial.print(stopMotorNum);
        Serial.print(":");
        Serial.println(timeTmp);
      }
      if (thisChar == 'f')
      {
        sensor1Num = 0;//鐢电溂1璁℃暟
        sensor2Num = 0;//鐢电溂2璁℃暟
        sensor3Num = 0;//鐢电溂3璁℃暟
        photographNum = 0;//鎷嶇収璁℃暟
        kickNum = 0;//韪㈠簾璁℃暟
        stopMotorNum = 0;//鍋滄満璁℃暟
      }
    }
  }
}
void timerIsr()
{
  timeTmp = millis();
  digitalWrite(kick, LOW);
  Serial.print("Galileo kick :");
  Serial.print(kickNum);
  Serial.print(" :stop:");
  Serial.println(timeTmp);
}
void phoGraph()//涓柇鍑芥暟
{
  unsigned long timeTmp = millis();
  int num = 0;
  do
  {
    if (digitalRead(sensor1) != 0)
    {
      return;
    }
    delayMicroseconds(300);
    num++;
  } while (num < 5);
  if (num >= 5)
  {
    digitalWrite(photograph, HIGH);   // sets the LED on
    sensor1Num++;
    EthernetClient client = server.available();
    if (client) {
      char timeStr[100];
      String test1 = "BB_";
      test1.concat(sensor1Num);
      test1.concat("_");
      test1.concat(timeTmp);
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
    }
    //    int index = (sensor1Num-1)  % 6;
    //    resultSensor1[index].b_time = millis();
    //    resultSensor1[index].b_id = sensor1Num;
    //    resultSensor1[index].b_dataStatus = false;
    delayMicroseconds(3000);                  // waits for a second
    digitalWrite(photograph, LOW);
    Serial.println("camera");

  }
}

void fber2()//涓柇鍑芥暟
{
  unsigned long timeTmp = millis();
  int num = 0;
  do
  {
    if (digitalRead(sensor2) != 0)
    {
      return;
    }
    delayMicroseconds(300);
    num++;
  } while (num < 5);
  if (num >= 5)
  {
    sensor2Num++;
    EthernetClient client = server.available();
    if (client) {
      char timeStr[100];
      String test1 = "CC_";
      test1.concat(sensor2Num);
      test1.concat("_");
      test1.concat(timeTmp);
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
    }
    /*sensor2Time[(sensor2Num-1)%6]=tmpTime;*/

    //    unsigned long tmpTime = millis();



    //    int index = 0;
    //    boolean isFind = false;
    //    for (index = 0; index < 6; index++)
    //    {
    //      if (resultSensor1[index].b_dataStatus && tmpTime > resultSensor1[index].b_time)
    //      {
    //        int temp = abs(tmpTime + timeDiff - fiber_1_2_Time - resultSensor1[index].b_time );
    //        if ( temp <= 50)
    //        {
    //          isFind = true;
    //          if (sensor2Num != resultSensor1[index].b_id )
    //          {
    //            int tmp_ofs_num = resultSensor1[index].b_id ; //鏃堕棿鍖归厤鏃讹紝瀵硅鏁扮粨鏋滃己琛屽尮閰�    //            sensor2Num += resultSensor1[index].b_id - tmp_ofs_num;
    //            //count_infor1=( tmp_ofs_num - 1 )%MAX_BUFFER_NUM;
    //          }
    //          //鍙鎵惧埌鏃堕棿涓婄殑鍖归厤璇佹槑鐢电溂1鍦ㄨ鏁帮紝涓旂數鐪�璁℃暟姝ｅ父锛屽惁鍒欐病鏈夎鏁�    //          break;
    //        }
    //      }
    //    }
    //
    //    if ( !isFind)  //娌℃湁鎵惧埌涓庣數鐪�鐨勫尮閰嶆椂闂村
    //    {
    //      //if (isWriteLog)
    //      //{
    //      //	CString medium_kick("") ;
    //      //	medium_kick.Format( _T("\nERROR1\t%d\t%d\t%d"), /*pDlg->*/m_ofs_num2_Front,m_counkick, m_fiber_infor2_Front[m_counkick].now_Last) ;
    //      //	fiber_kick_infor += medium_kick ;
    //      //}
    //      kickNum++;
    //      digitalWrite(kick, HIGH);   // sets the LED on
    //
    //      delayMicroseconds(50000);                  // waits for a second
    //      digitalWrite(kick, LOW);
    //    }
    //    else if (isFind && !resultSensor1[index].b_isOk)
    //    {
    //      kickNum++;
    //      digitalWrite(kick, HIGH);   // sets the LED on
    //
    //      delayMicroseconds(50000);                  // waits for a second
    //      digitalWrite(kick, LOW);
    //    }
    //    else
    //    {
    //      sensor2Index = (sensor2Index++) % 6;
    //      sensor2Time[sensor2Index] = tmpTime;
    //    }
  }
}

void fber3()//涓柇鍑芥暟
{
  sensor3Num++;
  unsigned long timeTmp = millis();
  int num = 0;
  do
  {
    if (digitalRead(sensor3) != 0)
    {
      return;
    }
    delayMicroseconds(300);
    num++;
  } while (num < 5);
  if (num >= 5)
  {
    EthernetClient client = server.available();
    if (client) {
      char timeStr[100];
      String test1 = "DD_";
      test1.concat(sensor3Num);
      test1.concat("_");
      test1.concat(timeTmp);
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
    }


    //    unsigned long tmpTime = millis();
    //    int index = 0;
    //    boolean isFind = false;
    //    for (index = 0; index < 6; index++)
    //    {
    //      if (tmpTime > sensor2Time[index])
    //      {
    //        int temp = abs(tmpTime + timeDiff - fiber_2_3_Time - sensor2Time[index] );
    //        if ( temp <= 50)
    //        {
    //          isFind = true;
    //          break;
    //        }
    //      }
    //    }
    //
    //    if ( !isFind)  //娌℃湁鎵惧埌涓庣數鐪�鐨勫尮閰嶆椂闂村
    //    {
    //      //if (isWriteLog)
    //      //{
    //      //	CString medium_kick("") ;
    //      //	medium_kick.Format( _T("\nERROR1\t%d\t%d\t%d"), /*pDlg->*/m_ofs_num2_Front,m_counkick, m_fiber_infor2_Front[m_counkick].now_Last) ;
    //      //	fiber_kick_infor += medium_kick ;
    //      //}
    //      stopMotorNum++;
    //      digitalWrite(stopMotorNum, HIGH);   // sets the LED on
    //      EthernetClient client = server.available();
    //      if (client) {
    //        char thisChar[] = "E";
    //        server.write(thisChar);//鍙戦�鍋滄満鐘舵�
    //      }
    //    }
  }
}

