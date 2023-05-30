#define PWMA 29
#define PWMB 28
#define AIN1 27
#define AIN2 26
#define BIN1 22
#define BIN2 23
#define STBY 20

bool sens1;
bool sens2;
bool sens3;
bool sens4;
bool sens5;
bool sens6;
bool sens7;

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

  pinMode(21, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
}

void loop() {
  sensAsigner();
  //sensorPrinter();
  if (sens4){
    controlSignal = 0 * Kp;
  }
  if (sens3){
    controlSignal = -1 * Kp;
  }
  if (sens5){
    controlSignal = 1 * Kp;
  }
  speedDiferential = controlSignal * motorBaseSpeed;
  if(sens4 || sens3 || sens5){
    move("FWD", speedDiferential);
  }
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
    Serial.println("ERROR");
  }
}

void sensAsigner(){
  sens1 = !digitalRead(21);
  sens2 = !digitalRead(18);
  sens3 = !digitalRead(19);
  sens4 = !digitalRead(2);
  sens5 = !digitalRead(3);
  sens6 = !digitalRead(10);
  sens7 = !digitalRead(15);
}

void sensorPrinter()
{
  Serial.print("#");
  Serial.print("#");
  Serial.print(sens1);
  Serial.print("#");
  Serial.println("#");
  Serial.print(sens2);
  Serial.print(sens3);
  Serial.print(sens4);
  Serial.print(sens5);
  Serial.println(sens6);
  Serial.print("#");
  Serial.print("#");
  Serial.print(sens7);
  Serial.print("#");
  Serial.println("#");
  Serial.println();
}