#define PWMA 29
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
}

void loop() {
  senAsigner();
  sensorPrinter();
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
    Serial.println(motorBaseSpeed + motorPower);
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
    Serial.println("Stoped...");
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
  senA = !digitalRead(senInA);
  senB = !digitalRead(senInB);
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