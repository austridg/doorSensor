// simple door sensor using obstacle avoidance sensor
int sensor = 2;      
int detectionCounter = 0;
bool previousState = HIGH;
unsigned long lastDetectionTime = 0;
unsigned long cooldown = 3000; // 3 seconds

void setup() {
  pinMode(sensor, INPUT);
  Serial.begin(9600);
}

void loop() {
  int currentState = digitalRead(sensor);
  unsigned long currentTime = millis();

  // sensor detects infrared - previous state didn't
  if (currentState == 0 && previousState == 1) {
    // cooldown for detection - avoid repeat detections for opening and closing
    if (currentTime - lastDetectionTime > cooldown) {
      detectionCounter++;
      Serial.print("DETECTION - TOTAL DETECTIONS: ");
      Serial.println(detectionCounter);
      lastDetectionTime = currentTime;
    }
  }
  previousState = currentState;
}

/*
ISSUE

sensor is inconsistent.
sometimes (every so often) it doesn't detect door movement when there is door movement.
if the door opens quickly it has trouble detecting.
range of only 2-10cm.
try IR sensor in future iterations maybe.
*/


/*
TODO

use with pi to log and save detections in text file
*/