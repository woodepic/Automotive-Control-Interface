int PWM_input = A4;

unsigned long duration = 0;
char buf[32];
unsigned long ulTest;



//minimum:1400
//max: 10000

void setup() {
    Serial.begin( 9600 );
    pinMode(PWM_input, INPUT);
    ulTest = 0x5A0F;
    sprintf(buf, "%ld", ulTest);
    Serial.println(buf);

}



void loop() {
  // put your main code here, to run repeatedly:
  duration = pulseIn(PWM_input, HIGH, 25000);
  sprintf(buf, "%ld", duration);
  Serial.println(buf);
  //Serial.println(duration);
  Serial.println("test");

}