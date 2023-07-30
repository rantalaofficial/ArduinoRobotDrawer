// RobotController.h

#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

class RobotController {
public:
  // Constructor
  RobotController(float arm1Length, float arm2Length, float defaultX, float defaultY, float angleStepSize);

  // Methods to control the robot
  void updateAngles();
  bool moveTo(float targetX, float targetY);
  bool moveX(float amount);
  bool moveY(float amount);
  bool moveToDefault();

  // Getters
  float getTargetX();
  float getTargetY();
  float getDefaultX();
  float getDefaultY();
  float getCurrentAngle1();
  float getCurrentAngle2();
  float getArm1Length();
  float getArm2Length();

private:
  float arm1Length;
  float arm2Length; 

  float defaultX; 
  float defaultY;

  float targetX;
  float targetY;

  float currentAngle1;
  float currentAngle2;

  float angleStepSize;

  void calculateInverseKinematics(float x, float y, float &angle1, float &angle2);
  bool inRange(float x, float y);

};
#endif
