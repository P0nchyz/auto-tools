//    RP2040

char dirB[10]; //Array made to store each of the digits of the driection numbers
const unsigned int MAX_MESSAGE_LENGHT = 12; //max lenght allowed on the serial buffer array

#define sens1 1
#define sens2 2
#define sens3 3
#define sens4 4
#define sens5 5
#define sens6 6
#define sens7 7

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200); //serial1 is comminucating with ESP32
  Serial.begin(115200);
  Serial.println("Sopas");
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorPrinter();
  serialDecoder();
  Serial.println(dirB);
  delay(1000);
}

void serialDecoder()
{
  while (Serial1.available() > 0) //runs whle there is data on the serial buffer
  {
    static char message[MAX_MESSAGE_LENGHT]; //stores the message from the serial buffer with the predefined max lenght
    static unsigned int message_pos = 0; //postion in the current char reading iteration

    char inByte = Serial1.read(); //temporary variable that stores the current character on the serial buffer, and deletes it

    if (inByte != 10 && (message_pos < MAX_MESSAGE_LENGHT - 1)) { //checks if the current byte is not a new line character and enforces the character limit
      message[message_pos] = inByte; //stores the current byte in its correspondet array position
      message_pos++; //increments the position
    }
    else {
      //message[message_pos] = '/0'; //ends the array with a 0 char
      Serial.println(message);
      message_pos = 0;//resets position
          if(message[0] == 68 && message[1] == 82){
            dirB[0] = message[2];
            dirB[1] = message[3];
            dirB[2] = message[4];
            dirB[3] = message[5];
            dirB[4] = message[6];
            dirB[5] = message[7];
            dirB[6] = message[8];
            dirB[7] = message[9];
            dirB[8] = message[10];
            dirB[9] = message[11];
          }
      memset(message,0,MAX_MESSAGE_LENGHT); //sets the values of the array to 0
    }
  }
}
void sensorPrinter()
{
  Serial.print("#");
  Serial.print("#");
  Serial.print(!digitalRead(sens1));
  Serial.print("#");
  Serial.println("#");
  Serial.print(!digitalRead(sens2));
  Serial.print(!digitalRead(sens3));
  Serial.print(!digitalRead(sens4));
  Serial.print(!digitalRead(sens5));
  Serial.println(!digitalRead(sens6));
  Serial.print("#");
  Serial.print("#");
  Serial.print(!digitalRead(sens7));
  Serial.print("#");
  Serial.println("#");
  Serial.println();
}