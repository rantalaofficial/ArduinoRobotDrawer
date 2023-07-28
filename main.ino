#include <Arduino.h>
#include <math.h>

const int servoPin1 = 9; 
const int servoPin2 = 10; 

struct Angles {
  float a1;
  float a2;
};

struct Point {
  float x;
  float y;
};

const Angles startAngles = {100, 100};
Angles servoAngles = startAngles;
Angles targetAngles = startAngles;

Point targetLocation = {10, 10};

int moveInterval = 100;

double t = 0;

struct AsyncTask {
  unsigned long interval; // The time interval in milliseconds
  unsigned long previousMillis; // Variable to store the last time the task was executed
  void (*taskFunction)(); // Function pointer for the task
};

const int MAX_TASKS = 1; // Maximum number of asynchronous tasks
AsyncTask tasks[MAX_TASKS]; 

void addAsyncTask(unsigned long interval, void (*taskFunction)()) {
  static int taskIndex = 0;

  if (taskIndex < MAX_TASKS) {
    tasks[taskIndex].interval = interval;
    tasks[taskIndex].previousMillis = millis();
    tasks[taskIndex].taskFunction = taskFunction;
    taskIndex++;
  }
}

void moveServo(int angle, int servoPin) {
  int pulseWidth = map(angle, 0, 180, 500, 2500); // Map the angle to a pulse width between 500us to 2500us
  digitalWrite(servoPin, HIGH); // Set the pin high
  delayMicroseconds(pulseWidth); // Wait for the desired pulse width
  digitalWrite(servoPin, LOW); // Set the pin low
  delay(20); // Give some time before the next movement (adjust this if needed)

  //Serial.println(angle);
}

void setup() {
  pinMode(servoPin1, OUTPUT);
  pinMode(servoPin2, OUTPUT);
  Serial.begin(9600);

  addAsyncTask(moveInterval, task); 

  delay(100);
  Serial.println("Robot ready");
}

void loop() {
	// Call all the asynchronous functions
	for (int i = 0; i < MAX_TASKS; i++) {
		if (millis() - tasks[i].previousMillis >= tasks[i].interval) {
		tasks[i].previousMillis = millis();
		tasks[i].taskFunction();
		}
	}
  
	//getAngleInput(&targetAngles);
  getInput(&targetAngles, &targetLocation);

  //animateCircle(&targetAngles, &t);

  moveTowardsTarget(&servoAngles, &targetAngles);

	moveServo(servoAngles.a1, servoPin1);
	moveServo(servoAngles.a2, servoPin2);
}

void calculateInverseKinematics(int x, int y, Angles& target) {
	int l1 = 17;
	int l2 = 17;

	float clamped_theta2 = min(1, max(-1, (x*x + y*y - l1*l1 - l2*l2) / (2*l1*l2)));
    
	float theta2 = acos(clamped_theta2);
    if (sin(theta2) != 0) {
		target.a1 = constrain((atan2(y, x) - atan2(l2*sin(theta2), l1 + l2*cos(theta2))) * 57.296, 0, 180);
		target.a2 = constrain((atan2(y, x) - atan2(-l2*sin(theta2), l1 + l2*cos(theta2))) * 57.296, 0, 180);

	}

}

void getInput(Angles* target, Point* p) {
  if (Serial.available()) {
    char serialData[10]; // Assuming the input won't exceed 10 characters
    byte index = 0;

    while (Serial.available() && index < 9) {
      char incomingChar = Serial.read();
      if (incomingChar == '\n') {
        break; // End of input reached
      }
      serialData[index++] = incomingChar;
    }
    serialData[index] = '\0'; // Null-terminate the string

    // Check if the command starts with 'A' or 'B'
    char command = serialData[0];
    if (command == 'x' || command == 'y') {
      // Convert the rest of the string (excluding the command) to an integer
      int value = strtol(serialData + 1, NULL, 10);

      // Adjust the angle based on the command
      if (value >= 5 && value <= 30) {
        if (command == 'x') {
          p->x = value;
        } else if (command == 'y') {
          p->y = value;
        }

        Serial.println("Target set to: " + String(p->x) + " " + String(p->y));

        calculateInverseKinematics(p->x, p->y, *target);
        Serial.println("Calculated angles: " + String(target->a1) + " " + String(target->a2));

      }
    } else {
      if (command == 'a') {
        target->a1 += 15;
      } else if (command == 'd') {
        target->a1 -= 15;
      } else if (command == 'w') {
        target->a2 -= 15;
      } else if (command == 's') {
        target->a2 += 15;
      } else if (command == 'r') {
        target->a1 = startAngles.a1;
        target->a2 = startAngles.a2;
      } else {
        return;
      }
      target->a1  = constrain(target->a1, 0, 180);
      target->a2 = constrain(target->a2, 0, 180);

      Serial.println(">" + String(command) + " " + String(target->a1) + ", " + String(target->a2));
    }
  }
}



void moveTowardsTarget(Angles* actual, Angles* target) {
	if (actual->a1 >  target->a1) {
	  actual->a1 -= 1;
	} else if (actual->a1 < target->a1) {
	  actual->a1 += 1;
	}

	if (actual->a2 > target->a2) {
	  actual->a2 -= 1;
	} else if (actual->a2 < target->a2) {
	  actual->a2 += 1;
	}
}

void animateCircle(Angles* target, double* t) {

  target->a1 = 110+sin(*t)*10;
  target->a2 = 110+cos(*t)*10;

  *t += 0.01;

  if (*t > 314.2) {
    *t = 0;
  }
}

void task() {

}


