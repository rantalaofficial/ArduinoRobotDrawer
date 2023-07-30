#include <Arduino.h>
#include <math.h>
#include "RobotController.h"

float t = 0;

int ledPin = 7;
int servoPin1 = 9;
int servoPin2 = 10;
RobotController controller(17, 17, 17, 17, 1);

bool animate = false;

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

	if (animate) animateObject(t);
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
				// This code moves to the start of the animation
				animateObject(t);
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


void animateObject(float& t)
{
	// CIRCLE
	//float x = controller.getDefaultX() + sin(t) * 6;
	//float y = controller.getDefaultY() + cos(t) * 6;

	// HEART
	float radius = 0.4;
	float x = controller.getDefaultX() + radius * 16 * pow(sin(t), 3);
	float y = controller.getDefaultY() + radius * (13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t));

	controller.moveTo(x, y);
	t += 0.005;
}


