#include <Arduino.h>
#include <math.h>
#include "RobotController.h"

float t = 0;

int ledPin = 7;
int servoPin1 = 9;
int servoPin2 = 10;
RobotController controller(17, 17, 17, 17, 1);

bool animate = false;

const float END_MARKER = -1;

int shapeArrayX[200] = {23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,22,22,22,21,21,20,20,19,19,18,18,17,16,16,15,15,14,14,13,13,12,12,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,-1};
int shapeArrayY[200] = {28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,10,11,12,12,13,13,14,14,15,15,16,17,17,16,16,15,15,14,14,13,13,12,12,11,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,-1};



int calculateShapeSize() {
	int size = 0;
	while (shapeArrayX[size] != END_MARKER && size < 200) {
		size++;
	}
	return size-1;
}

const int shapeSize = calculateShapeSize();
const float shapeSpeed = 0.1;

void setup()
{	
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	pinMode(servoPin1, OUTPUT);
	pinMode(servoPin2, OUTPUT);
	Serial.begin(9600);

	delay(100);
	Serial.println("Robot ready");
}

void moveServo(float angle, int servoPin)
{
	int pulseWidth = map(angle, 0, 180, 500, 2500); // Map the angle to a pulse width between 500us to 2500us
	digitalWrite(servoPin, HIGH);					// Set the pin high
	delayMicroseconds(pulseWidth);					// Wait for the desired pulse width
	digitalWrite(servoPin, LOW);					// Set the pin low
	delay(20);										// Give some time before the next movement (adjust this if needed)
}

void loop()
{
	// getAngleInput(&targetAngles);
	getInput();

	//if (animate) animateObject(t);
	if (animate) animateShapeArray(t, shapeSpeed, shapeSize, shapeArrayX, shapeArrayY);
	// animateCircle(&targetAngles, &t);
	controller.updateAngles();
	moveServo(controller.getCurrentAngle1(), servoPin1);
	moveServo(controller.getCurrentAngle2(), servoPin2);
}

void getInput()
{
	if (Serial.available())
	{
		char serialData[10]; // Assuming the input won't exceed 10 characters
		byte index = 0;

		while (Serial.available() && index < 9)
		{
			char incomingChar = Serial.read();
			if (incomingChar == '\n')
			{
				break;
			}
			serialData[index++] = incomingChar;
		}
		serialData[index] = '\0';

		char command = serialData[0];

		bool inRange = false;
		if (command == 'x' || command == 'y')
		{
			int value = strtol(serialData + 1, NULL, 10);

			if (command == 'x')
			{
				inRange = controller.moveTo(value, controller.getTargetY());
			}
			else if (command == 'y')
			{
				inRange = controller.moveTo(controller.getTargetX(), value);
			}
		}
		else if (command == 'a')
		{
			inRange = controller.moveX(2);
		}
		else if (command == 'd')
		{
			inRange = controller.moveX(-2);
		}
		else if (command == 'w')
		{
			inRange = controller.moveY(2);
		}
		else if (command == 's')
		{
			inRange = controller.moveY(-2);
		}
		else if (command == 'r')
		{
			inRange = controller.moveToDefault();
		}
		else if (command == 'c') {
			animate = !animate;

			if (animate) {
				Serial.println("Moving to beginning of animation...");
				digitalWrite(ledPin, LOW);
				t = 0;
				//animateObject(t);
				controller.moveTo(shapeArrayX[0], shapeArrayY[0]);
				for (int i = 0; i < 100; i++) {
					controller.updateAngles();
					moveServo(controller.getCurrentAngle1(), servoPin1);
					moveServo(controller.getCurrentAngle2(), servoPin2);
				}
				digitalWrite(ledPin, HIGH);
			}

			Serial.println("Animation " + String(animate ? "enabled" : "disabled"));
			return;
		}
		else
		{
			return;
		}

		if (!inRange)
		{
			Serial.println("Location cannot be reached, maximum reach is " + String(controller.getArm1Length() + controller.getArm2Length()) + " cm");
		} else {
			Serial.println("Moving to (" + String(controller.getTargetX()) + ", " + String(controller.getTargetY()) + ")");
		}
	}
}

void animateShapeArray(float& t, float speed, int arraySize, int* xArray, int* yArray)
{
	if (t > arraySize) {
		animate = false;
		Serial.println("Animation finished");
		return;
	};

	float x, y;
	x = xArray[round(t)];
	y = yArray[round(t)];
	
	controller.moveTo(x, y);

	if (round(t + speed) != round(t)) {
		Serial.println(String(round(t)) + "/" + String(arraySize));
	}
	t += speed;
}


void animateObject(float& t)
{
	float x, y;
	// HEART
	float scaleX = 0.4;
	float scaleY = 0.5;
	x = controller.getDefaultX() + scaleX * 16 * pow(sin(t), 3);
	y = controller.getDefaultY() + scaleY * (13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t));

	if (t > 2 * PI) {
		float newT = t - 2 * PI;
		if (newT > PI && newT < 2 * PI) {
			x = controller.getDefaultX() + 2 * cos(PI);
			y = controller.getDefaultY() + 3 * sin(0) + (newT-PI) * 2;
		} else if (newT > 2*PI) {
			return;
		} else {
			x = controller.getDefaultX() + 2 * cos(newT);
			y = controller.getDefaultY() + 3 * sin(newT+PI);
		}
	}

	controller.moveTo(x, y);
	t += 0.005;
}


