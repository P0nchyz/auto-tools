
char CURRENT_PATH[9]; //Array made to store each of the digits of the driection numbers
const unsigned int MAX_MESSAGE_LENGHT = 12; //max lenght allowed on the serial buffer array
int CURRENT_PATH_POSITION = 0;

#define PWMA 27
#define PWMB 28
#define AIN1 27
#define AIN2 26
#define BIN1 22
#define BIN2 23
#define STBY 20

#define senIn1 21
#define senIn2 18
#define senIn3 19
#define senIn4 2
#define senIn5 3
#define senInA 10
#define senInB 15

bool sen1;
bool sen2;
bool sen3;
bool sen4;
bool sen5;
bool senA;
bool senB;

const int motorBaseSpeed = 128;
const float Kp = 0.3;
float controlSignal = 0;
float speedDiferential = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); //serial1 is comminucating with ESP32

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  pinMode(senIn1, INPUT_PULLUP);
  pinMode(senIn2, INPUT_PULLUP);
  pinMode(senIn3, INPUT_PULLUP);
  pinMode(senIn4, INPUT_PULLUP);
  pinMode(senIn5, INPUT_PULLUP);
  pinMode(senInA, INPUT_PULLUP);
  pinMode(senInB, INPUT_PULLUP);

  pinMode(LED_BUILTIN, OUTPUT);

  CURRENT_PATH[0] = '1';
  CURRENT_PATH[1] = '1';
  CURRENT_PATH[2] = '2';
  CURRENT_PATH[3] = '2';
  CURRENT_PATH[4] = '1';
  CURRENT_PATH[5] = '1';
  CURRENT_PATH[6] = '0';
  CURRENT_PATH[7] = '0';
  CURRENT_PATH[8] = '0';
}

void loop() {
  serialDecoder();
  senAsigner();
  //sensorPrinter();
  senMover();
}

void move(String direction,int motorPower){
  if(direction == "FWD"){
    //Serial.println("Moving forward...");
    digitalWrite(STBY, HIGH);

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, motorBaseSpeed + motorPower);
    analogWrite(PWMB, motorBaseSpeed - motorPower);
  }
  else if (direction == "RGT") {
    Serial.println("Moving right...");
    digitalWrite(STBY, HIGH);

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, 255);
    analogWrite(PWMB, 255);
  }
  else if (direction == "LFT") {
    Serial.println("Moving left...");
    digitalWrite(STBY, HIGH);

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, 255);
    analogWrite(PWMB, 255);
  }
  else if (direction == "BKW") {
    Serial.println("Moving backwards...");
    digitalWrite(STBY, HIGH);

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, 255);
    analogWrite(PWMB, 255);
  }
  else if(direction == "STP"){
    //Serial.println("Stoped...");
    digitalWrite(STBY, LOW);

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
  }
  else {
    Serial.println("ERROR: Invalid input for move()");
  }
}

void senAsigner(){
  sen1 = !digitalRead(senIn1);
  sen2 = !digitalRead(senIn2);
  sen3 = !digitalRead(senIn3);
  sen4 = !digitalRead(senIn4);
  sen5 = !digitalRead(senIn5);
  senA = digitalRead(senInA);
  senB = digitalRead(senInB);
}

void sensorPrinter()
{
  Serial.println();
  Serial.print("##");
  Serial.print(senA);
  Serial.println("##");
  Serial.print(sen1);
  Serial.print(sen2);
  Serial.print(sen3);
  Serial.print(sen4);
  Serial.println(sen5);
  Serial.print("##");
  Serial.print(senB);
  Serial.println("##");
  Serial.println();
  delay(100);
}

bool senBool(bool s1,bool s2,bool s3,bool s4,bool s5){
  if(s1==sen1 && s2==sen2 && s3==sen3 && s4==sen4 && s5==sen5){
    return true;
  }
  else{
    return false;
  }
}

bool topTFlag;
bool leftTFlag;
bool rightTFlag;
void senMover() {
  if ((senBool(1,1,1,1,1) && !senA && senB) || topTFlag){
    if (!topTFlag){
      senIntersectionTurner("topT");
      }
    topTFlag = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Top T");
    if ((senBool(1,1,1,0,0) && senA && senB) || (senBool(0,0,1,1,1) && senA && senB)){
      for (int i = 0; i < 3000; i++){
        senLineFollow();
        Serial.println("delayed fwd");
        Serial.println(i);
      }
      topTFlag = false;
      Serial.println("Top T exited");
      senLineFollow();
    }
  }
  else if ((senBool(1,1,1,0,0) && senA && senB) || leftTFlag){
    if (!leftTFlag){
      senIntersectionTurner("leftT");
      }
    leftTFlag = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Left T");
    if ((senBool(1,1,1,1,1) && senA && !senB) || (senBool(1,1,1,0,0) && senA && senB)){
      for (int i = 0; i < 3000; i++){
        senLineFollow();
        Serial.println("delayed fwd");
        Serial.println(i);
      }
      leftTFlag = false;
      Serial.println("Left T exited");
      senLineFollow();
    }
  }
  else if ((senBool(0,0,1,1,1) && senA && senB) || rightTFlag){
    if (!rightTFlag){
      senIntersectionTurner("rightT");
      }
    rightTFlag = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Right T");
    if ((senBool(1,1,1,1,1) && senA && !senB) || (senBool(0,0,1,0,0) && senA && senB)){
      for (int i = 0; i < 3000; i++){
        senLineFollow();
        Serial.println("delayed fwd");
        Serial.println(i);
      }
      rightTFlag = false;
      Serial.println("Right T exited");
      senLineFollow();
    }
  }
  else {
    senLineFollow();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("No intersections");
  }
}

void senLineFollow(){
  if(senBool(0,0,1,0,0)){
    controlSignal = 0 * Kp;
  }
  if(senBool(0,0,0,1,0)){
    controlSignal = 1 * Kp;
  }
  if(senBool(0,1,0,0,0)){
    controlSignal = -1 * Kp;
  }
  if(senBool(0,0,0,0,1)){
    controlSignal = 2 * Kp;
  }
  if(senBool(1,0,0,0,0)){
    controlSignal = -2 * Kp;
  }

  speedDiferential = controlSignal * motorBaseSpeed;

  if (!senBool(0,0,0,0,0)){
    move("FWD",speedDiferential);
    Serial.print("RGT:  ");
    Serial.print(motorBaseSpeed + speedDiferential);
    Serial.print("    LFT:  ");
    Serial.println(motorBaseSpeed - speedDiferential);

  }
  else {
    move("STP", 0);
    Serial.println("No line detected");
  }
}

void senIntersectionTurner(String turningDirection){
  if(CURRENT_PATH[CURRENT_PATH_POSITION] == '0'){
    Serial.println("No More Directions");
  }
  else if (CURRENT_PATH[CURRENT_PATH_POSITION] == '1' && (turningDirection == "topT" || turningDirection == "leftT")){
    move("LFT", 0);
    Serial.println("current pos == 1; moving right");
    CURRENT_PATH_POSITION++;
  }
  else if (CURRENT_PATH[CURRENT_PATH_POSITION] == '1' && (turningDirection == "rightT")){
    move("FWD", 0);
    Serial.println("current pos == 1; moving forward");
    CURRENT_PATH_POSITION++;
  }
  else if(CURRENT_PATH[CURRENT_PATH_POSITION] == '2' && (turningDirection == "topT" || turningDirection == "rightT")){
    move("RGT", 0);
    Serial.println("current pos == 0; moving left");
    CURRENT_PATH_POSITION++;
  }
  else if(CURRENT_PATH[CURRENT_PATH_POSITION] == '2' && (turningDirection == "leftT")){
    move("FWD", 0);
    Serial.println("current pos == 0; moving forward");
    CURRENT_PATH_POSITION++;
  }
  else if (turningDirection != "leftT" || turningDirection != "topT" || turningDirection != "rightT"){
    Serial.println("INVALID PARAMETER FOR FUNCTION senItersectionTurner()");
  }
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
            CURRENT_PATH[0] = message[2];
            CURRENT_PATH[1] = message[3];
            CURRENT_PATH[2] = message[4];
            CURRENT_PATH[3] = message[5];
            CURRENT_PATH[4] = message[6];
            CURRENT_PATH[5] = message[7];
            CURRENT_PATH[6] = message[8];
            CURRENT_PATH[7] = message[9];
            CURRENT_PATH[8] = message[10];
            CURRENT_PATH[9] = message[11];
          }
      memset(message,0,MAX_MESSAGE_LENGHT); //sets the values of the array to 0
    }
  }
}