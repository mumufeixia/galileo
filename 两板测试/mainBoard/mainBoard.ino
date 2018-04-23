#include <Event.h>
#include <SPI.h>
#include <Ethernet.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
typedef struct
{
  unsigned int img_id;//computer image id
  unsigned long img_time;//computer image time
  int img_dataStatus;//0--ok 1-bad,kick    2-bad,double kick
} imageInfo; //
typedef struct
{
  unsigned int b_id;//
  unsigned long b_time;//galileo
  imageInfo imageInfoA;
} inforResult; //
byte mac[] = { 0x98, 0x4F, 0xEE, 0x05, 0x80, 0x7C};
IPAddress ip(192, 168, 2, 2);
EthernetServer server(5099); //
boolean alreadyConnected = false; // whether or not the client was connected previously
int sensor1 = 2;//
int sensor2 = 3;//
int sensor3 = 4;//�
int kick = 6; //
int stopMotor = 7; //
int photograph = 5; //
//volatile int state = LOW;
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
inforResult resultSensor1[6];//
boolean isStartDec = false; // startDec ==true   startDec==false
char recChar[100] = {0};
int index_Char = 0; //char index
EthernetClient client;

void setup() {
  Serial.begin(9600);
  delay(1000);
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  for (int x = 0; x < 100; x++)			//
  {
    recChar[x] = '\0';
  }
  Serial.println("start");
  // start listening for clients
  server.begin();
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(photograph, OUTPUT);
  pinMode(kick, OUTPUT);
  pinMode(stopMotor, OUTPUT);
  attachInterrupt(sensor1, phoGraph, FALLING);//
  attachInterrupt(sensor2, fber2, FALLING);//
  attachInterrupt(sensor3, fber3, FALLING);//
  for (int i = 0; i < 6; i++)
  {
    resultSensor1[i].imageInfoA.img_dataStatus = 1;
    resultSensor1[i].imageInfoA.img_id = 0;
    resultSensor1[i].imageInfoA.img_time = 0;
    resultSensor1[i].b_id = 0;
    resultSensor1[i].b_time = 0;
    sensor2Time[i] = 0;
  }
  client = server.available();

}

void loop() {
  if (client) {
    int numChar = client.available();
    if (numChar > 0) {
      unsigned long timeTmp = millis();
      if (numChar > 100)
      {
        numChar = 100;
      }
      //      Serial.print("client say:");
      char tmpChar;
      while (numChar--)
      {
        tmpChar = client.read();
        if (tmpChar != '\0')
        {
          recChar[index_Char++] = tmpChar;
          Serial.print("@@@:");
          Serial.println(tmpChar);
        }
        else
        {
          splitString(recChar, index_Char, timeTmp);
          //          Serial.println(recChar);
          index_Char = 0;
          for (int x = 0; x < 100; x++)
          {
            recChar[x] = '\0';
          }
        }
      }
    }
  }
  else
  {
    client = server.available();
    if (client) {
      int numChar = client.available();
      if (numChar > 0) {
        unsigned long timeTmp = millis();
        if (numChar > 100)
        {
          numChar = 100;
        }
        //      Serial.print("client say:");
        char tmpChar;
        while (numChar--)
        {
          tmpChar = client.read();
          if (tmpChar != '\0')
          {
            recChar[index_Char++] = tmpChar;
            Serial.print("@@@:");
            Serial.println(tmpChar);
          }
          else
          {
            splitString(recChar, index_Char, timeTmp);
            //          Serial.println(recChar);
            index_Char = 0;
            for (int x = 0; x < 100; x++)
            {
              recChar[x] = '\0';
            }
          }
        }
      }
    }
  }
}
void splitString(char *data, int num, unsigned long timeTmp)
{
  Serial.print("Data entered:");
  Serial.println(data);
  if (data[0] == '#' && data[1] == '#' && data[num - 2] == '&' && data[num - 1] == '&')
  {
    unsigned long dataInt = 0;
    int index = 4;
    switch (data[2])
    {
      case 'A':
        {
          dataInt = wordToInt(data, index);
          if (dataInt != -1)
          {
            timeDiff = timeTmp - dataInt;
          }
          Serial.print("@@AAA_");
          Serial.print("bankaTime:");
          Serial.print(timeTmp);
          Serial.print("_PCTime:");
          Serial.print(dataInt);
          Serial.print("_timeDiff:");
          Serial.println(timeDiff);
          break;
        }
      case 'B':
        {
          dataInt = wordToInt(data, index);
          if (dataInt == 1)
          {
            isStartDec = true;
          }
          else if (dataInt == 0)
          {
            isStartDec = false;
          }
          else if (dataInt != -1)
          {
            dataErrorDeal(data[2]);
          }
          Serial.print("@@BBB_");
          Serial.print("isStartDec:");
          Serial.println(isStartDec);
          break;
        }
      case 'C':
        {
          dataInt = wordToInt(data, index);
          if (dataInt != -1)
          {
            fiber_1_2_Time = dataInt;
            dataInt = wordToInt(data, ++index);
            if (dataInt != -1)
            {
              fiber_2_3_Time = dataInt;
              dataInt = wordToInt(data, ++index);
              if (dataInt != -1)
              {
                kickTime = dataInt;
              }

            }

          }

          Serial.print("@@CCCC_");
          Serial.print("fiber_1_2_Time:");
          Serial.print(fiber_1_2_Time);
          Serial.print("_fiber_2_3_Time:");
          Serial.print(fiber_2_3_Time);
          Serial.print("_kickTime:");
          Serial.println(kickTime);
          break;
        }
      case 'D':
        {
          if (isStartDec)
          {
            imageInfo tmpImage = {0};
            int i = -1;
            if (dataInt != -1)
            {
              dataInt = wordToInt(data, index);
              tmpImage.img_id = dataInt;
              dataInt = wordToInt(data, ++index);
              if (dataInt != -1)
              {
                tmpImage.img_time = dataInt;
                dataInt = wordToInt(data, ++index);
                if (dataInt != -1)
                {
                  tmpImage.img_dataStatus = dataInt;
                  for (i = 0; i < 6; i++)
                  {
                    if (resultSensor1[i].b_time < tmpImage.img_time && (abs(resultSensor1[i].b_time - timeDiff + 50 - tmpImage.img_time) < 35))
                    {
                      //              if(resultSensor1[i].b_id != tmpImage.img_id)
                      //              {
                      //              }
                      resultSensor1[i].imageInfoA = tmpImage;
                    }
                  }
                }
              }
            }

            Serial.print("@@DDDD_");
            if (i >= 6 || i == -1)
            {
              Serial.print("ERRER:");
              i = 5;
            }
            Serial.print("resultSensor1[");
            Serial.print(i);
            Serial.print("]b_id:");
            Serial.print(resultSensor1[i].b_id);
            Serial.print("_b_time:");
            Serial.print(resultSensor1[i].b_time);
            Serial.print("_img_id:");
            Serial.print(tmpImage.img_id);
            Serial.print("_img_time:");
            Serial.print(tmpImage.img_time);
            Serial.print("_image_img_dataStatus:");
            Serial.println(tmpImage.img_dataStatus);

          }
          break;
        }
      case 'E'://reset
        {
          digitalWrite(stopMotor, LOW);   // sets the LED on
          break;
        }
      case 'F':
        {
          sensor1Num = 0;
          sensor2Num = 0;
          matchSensor2Num = 0;
          sensor3Num = 0;
          photographNum = 0;
          kickNum = 0;
          stopMotorNum = 0;
          for (int i = 0; i < 6; i++)
          {
            resultSensor1[i].imageInfoA.img_dataStatus = 1;
            resultSensor1[i].imageInfoA.img_id = 0;
            resultSensor1[i].imageInfoA.img_time = 0;
            resultSensor1[i].b_id = 0;
            resultSensor1[i].b_time = 0;
            sensor2Time[i] = 0;
          }
          break;
        }

    }
    //     Serial.println("Lalalalalal");
    if (data[++index] == '&' && data[++index] == '&')
    {
      if (++index < num - 1)
      {
        //            Serial.println("aa");
        char*dataTmp = new char[num - index + 1];
        for (int i = index; i < num; i++)
        {
          Serial.print(data[i]);
          dataTmp[i - index] = data[i];
        }
        dataTmp[num - index] = '\0';
        Serial.println("");
        splitString(dataTmp, num - index, timeTmp);
        delete []dataTmp;

      }
      //          else
      //          {
      //            Serial.println("BB");
      //          }
    }
    else
    {
      Serial.print("errorWord");
      for (int i = 0; i < num; i++)
      {
        Serial.print(data[i]);
      }
      Serial.println("");
    }

    for (int x = 0; x < 100; x++)
    {
      recChar[x] = '\0';
    }
  }
}

int wordToInt(char *data, int &index)
{
  String tmp = "";
  while (data[index] != '_' && index < 100)
  {
    tmp += data[index++];
  }
  if (index >= 99)
  {
    dataErrorDeal(data[2]);
    return -1;
  }
  //  int result = atoi(tmp);
  int result = tmp.toInt();
  return result;
}
void dataErrorDeal(char data)
{
  char tmpInfo[6] = "??";
  tmpInfo[2] = data;
  tmpInfo[3] = '*';
  tmpInfo[4] = '*';
  tmpInfo[5] = '\0';
  server.write(tmpInfo);
  Serial.print("ERRER:server Say:");
  Serial.println(tmpInfo);
}
void timerIsr()
{
  unsigned long timeTmp = millis();
  digitalWrite(kick, LOW);
  Serial.print("Galileo kick :");
  Serial.print(kickNum);
  Serial.print(" :stop:");
  Serial.println(timeTmp);
}
void phoGraph()
{
  if (isStartDec)
  {
    unsigned long timeTmp = millis();
    while (millis() - timeTmp < 2)
    {
      if (digitalRead(sensor1) != 0)
      {
        return;
      }
    }
    digitalWrite(photograph, HIGH);   // sets the LED on

    sensor1Num++;
    //    EthernetClient client = server.available();
    if (client) {
      //      server.println("");
      char timeStr[100];
      String test1 = "&&X_";
      test1.concat(sensor1Num);
      test1.concat("_##");
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
    }
    else
    {
      client = server.available();
      if (client) {
        char timeStr[100];
        String test1 = "&&X_";
        test1.concat(sensor1Num);
        test1.concat("_##");
        test1.toCharArray(timeStr, 100);
        server.write(timeStr);
        Serial.print("server say:");
        Serial.println(timeStr);
      }
    }
    int index = (sensor1Num - 1)  % 6;
    resultSensor1[index].b_time = millis();
    resultSensor1[index].b_id = sensor1Num;
    Serial.println("camera");
    digitalWrite(photograph, LOW);   // sets the LED on

  }
}

void fber2()
{
  unsigned long timeTmp = millis();
  if (isStartDec)
  {

    while (millis() - timeTmp < 2)
    {
      if (digitalRead(sensor2) != 0)
      {
        return;
      }
    }
    matchSensor2Num++;
    sensor2Num++;
    Serial.print("sensor2Num :");
    Serial.print(sensor2Num);
    Serial.print(" :");
    Serial.println(timeTmp);
    /*sensor2Time[(sensor2Num-1)%6]=tmpTime;*/

    int index = 0;
    int temp = 0;
    //    boolean isFind = false;
    for (index = 0; index < 6; index++)
    {
      if (timeTmp > resultSensor1[index].b_time)
      {
        temp = abs(timeTmp + timeDiff - fiber_1_2_Time - resultSensor1[index].b_time );
        if ( temp <= 50)
        {
          //          isFind = true;
          if (matchSensor2Num != resultSensor1[index].b_id )
          {
            matchSensor2Num = resultSensor1[index].b_id ;   //            sensor2Num += resultSensor1[index].b_id - tmp_ofs_num;
            //count_infor1=( tmp_ofs_num - 1 )%MAX_BUFFER_NUM;
          }
          break;
        }
      }
    }

    //    if ( !isFind)
    if ( index >= 6)
    {
      //if (isWriteLog)
      //{
      //	CString medium_kick("") ;
      //	medium_kick.Format( _T("\nERROR1\t%d\t%d\t%d"), /*pDlg->*/m_ofs_num2_Front,m_counkick, m_fiber_infor2_Front[m_counkick].now_Last) ;
      //	fiber_kick_infor += medium_kick ;
      //}
      kickNum++;

      digitalWrite(kick, HIGH);   // sets the LED on
      unsigned long timeTmp1 = millis();
      Serial.print("kickNum :");
      Serial.print(kickNum);
      Serial.print(" :");
      Serial.println(timeTmp1);
      digitalWrite(kick, LOW);
    }
    else if (resultSensor1[index].imageInfoA.img_dataStatus > 0)
    {
      kickNum += resultSensor1[index].imageInfoA.img_dataStatus;
      digitalWrite(kick, HIGH);   // sets the LED on
      unsigned long timeTmp1 = millis();
      Serial.print("kickNum :");
      Serial.print(kickNum);
      Serial.print(" :");
      Serial.println(timeTmp1);
      digitalWrite(kick, LOW);
      //      digitalWrite(kick, HIGH);   // sets the LED on
      //
      //      digitalWrite(kick, LOW);
      //      timerKick.after(kickTime * resultSensor1[index].imageInfoA.img_dataStatus, timerIsr);
    }
    else
    {
      sensor2Index = (sensor2Index++) % 6;
      sensor2Time[sensor2Index] = timeTmp;
    }

    //    EthernetClient client = server.available();
    if (client) {
      char timeStr[100];
      String test1 = "&&Y_";
      test1.concat(sensor2Num);
      test1.concat("_##");
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
      test1 = "&&U_";
      test1.concat(matchSensor2Num);
      test1.concat("_##");
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
      test1 = "&&T_";
      test1.concat(kickNum);
      test1.concat("_##");
      test1.toCharArray(timeStr, 100);
      server.write(timeStr);
      Serial.print("server say:");
      Serial.println(timeStr);
    }
    else
    {
      client = server.available();
      if (client) {
        char timeStr[100];
        String test1 = "&&Y_";
        test1.concat(sensor2Num);
        test1.concat("_##");
        test1.toCharArray(timeStr, 100);
        server.write(timeStr);
        Serial.print("server say:");
        Serial.println(timeStr);
        test1 = "&&U_";
        test1.concat(matchSensor2Num);
        test1.concat("_##");
        test1.toCharArray(timeStr, 100);
        server.write(timeStr);
        Serial.print("server say:");
        Serial.println(timeStr);
        test1 = "&&T_";
        test1.concat(kickNum);
        test1.concat("_##");
        test1.toCharArray(timeStr, 100);
        server.write(timeStr);
        Serial.print("server say:");
        Serial.println(timeStr);
      }
    }
  }
}

void fber3()
{
  if (isStartDec)
  {
    unsigned long timeTmp = millis();
    while (millis() - timeTmp < 2)
    {
      if (digitalRead(sensor3) != 0)
      {
        return;
      }
    }
    sensor3Num++;
    //    EthernetClient client = server.available();
    if (client)
    {
      char timeStrD[100];
      String test1 = "&&Z_";
      test1.concat(sensor3Num);
      test1.concat("_##");
      test1.toCharArray(timeStrD, 100);
      server.write(timeStrD);
      Serial.print("server say:");
      Serial.println(timeStrD);
    }
    else
    {
      client = server.available();
      if (client) {
        char timeStrD[100];
        String test1 = "&&Z_";
        test1.concat(sensor3Num);
        test1.concat("_##");
        test1.toCharArray(timeStrD, 100);
        server.write(timeStrD);
        Serial.print("server say:");
        Serial.println(timeStrD);
      }
    }
    int index = 0;
    //    boolean isFind = false;
    for (index = 0; index < 6; index++)
    {
      if (timeTmp > sensor2Time[index])
      {
        int temp = abs(timeTmp + timeDiff - fiber_2_3_Time - sensor2Time[index] );
        if ( temp <= 50)
        {
          //          isFind = true;
          break;
        }
      }
    }

    if (index >= 6)
    {
      //if (isWriteLog)
      //{
      //	CString medium_kick("") ;
      //	medium_kick.Format( _T("\nERROR1\t%d\t%d\t%d"), /*pDlg->*/m_ofs_num2_Front,m_counkick, m_fiber_infor2_Front[m_counkick].now_Last) ;
      //	fiber_kick_infor += medium_kick ;
      //}
      stopMotorNum++;
      digitalWrite(stopMotor, HIGH);   // sets the LED on
      //      EthernetClient client = server.available();
      if (client)
      {
        char thisChar[] = "&&R_##";
        server.write(thisChar);
        Serial.print("server say:");
        Serial.println(thisChar);
        char timeStrD[100];
        String test1 = "&&S_";
        test1.concat(stopMotorNum);
        test1.concat("_##");
        test1.toCharArray(timeStrD, 100);
        server.write(timeStrD);
        Serial.print("server say:");
        Serial.println(timeStrD);
      }
      else
      {
        client = server.available();
        if (client)
        {
          char thisChar[] = "&&R_##";
          server.write(thisChar);
          Serial.print("server say:");
          Serial.println(thisChar);
          char timeStrD[100];
          String test1 = "&&S_";
          test1.concat(stopMotorNum);
          test1.concat("_##");
          test1.toCharArray(timeStrD, 100);
          server.write(timeStrD);
          Serial.print("server say:");
          Serial.println(timeStrD);
        }
      }
    }
  }
}


