
void setup() {
  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
}


void loop() {
	
		if (analogRead(A2) > 500) {
			digitalWrite(13, HIGH);   
			digitalWrite(11, HIGH);   
		}
		if (analogRead(A3) > 500) {
			digitalWrite(13, HIGH);   
			digitalWrite(10, HIGH);   
		}
	  delay(analogRead(A1));              
	  digitalWrite(13, LOW);                 
	  digitalWrite(11, LOW);   
	  digitalWrite(10, LOW);   
	  delay(analogRead(A0));              
}
