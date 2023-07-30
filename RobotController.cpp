#include "RobotController.h"
#include <math.h>
#include <Arduino.h>

RobotController::RobotController(float arm1Length, float arm2Length, float defaultX, float defaultY, float angleStepSize)
: arm1Length(arm1Length), arm2Length(arm2Length), targetX(defaultX), targetY(defaultY),
      defaultX(defaultX), defaultY(defaultY), angleStepSize(angleStepSize)
{
	calculateInverseKinematics(defaultX, defaultY, currentAngle1, currentAngle2);
}

void RobotController::calculateInverseKinematics(float x, float y, float &angle1, float &angle2)
{
	double l1 = arm1Length;
	double l2 = arm2Length;

	double clamped_theta2 = constrain((x * x + y * y - l1 * l1 - l2 * l2) / (2 * l1 * l2), -1, 1);

	double theta2 = acos(clamped_theta2);
	if (sin(theta2) != 0)
	{
		angle1 = 180 - constrain((atan2(y, x) - atan2(-l2 * sin(theta2), l1 + l2 * cos(theta2))) * 57.296 - 10, 0, 180);
		angle2 = constrain(theta2 * 57.296 + 10, 0, 180);
	}
}

void RobotController::updateAngles() 
{
	float newAngle1, newAngle2;
	calculateInverseKinematics(targetX, targetY, newAngle1, newAngle2);

	float angle1Diff = newAngle1 - currentAngle1;
	float angle2Diff = newAngle2 - currentAngle2;

	// Limit angle change to step size
	currentAngle1 += (angle1Diff > angleStepSize) ? angleStepSize : (angle1Diff < -angleStepSize) ? -angleStepSize : angle1Diff;
	currentAngle2 += (angle2Diff > angleStepSize) ? angleStepSize : (angle2Diff < -angleStepSize) ? -angleStepSize : angle2Diff;

	currentAngle1 = constrain(currentAngle1, 0, 180);
	currentAngle2 = constrain(currentAngle2, 0, 180);
}

bool RobotController::inRange(float x, float y)
{
	float distanceSquared = x*x + y*y - 1;
	float maxDistanceSquared = (arm1Length + arm2Length) * (arm1Length + arm2Length);
	float minDistance = 1;
	return distanceSquared <= maxDistanceSquared && distanceSquared >= minDistance;
}

bool RobotController::moveTo(float x, float y)
{
	if (!inRange(x, y)) return false;
	targetX = x;
	targetY = y;
	return true;
}

bool RobotController::moveToDefault()
{
	return moveTo(defaultX, defaultY);
}

bool RobotController::moveX(float amount)
{
	return moveTo(targetX + amount, targetY);
}

bool RobotController::moveY(float amount)
{
	return moveTo(targetX, targetY + amount);
}

float RobotController::getTargetX()
{
	return targetX;
}

float RobotController::getTargetY()
{
	return targetY;
}

float RobotController::getDefaultX()
{
	return defaultX;
}

float RobotController::getDefaultY()
{
	return defaultY;
}

float RobotController::getCurrentAngle1()
{
	return currentAngle1;
}

float RobotController::getCurrentAngle2()
{
	return currentAngle2;
}

float RobotController::getArm1Length()
{
	return arm1Length;
}

float RobotController::getArm2Length()
{
	return arm2Length;
}