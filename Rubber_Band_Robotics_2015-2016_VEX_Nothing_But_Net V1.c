#pragma config(Sensor, in1,    SCpot,          sensorPotentiometer)
#pragma config(Sensor, dgtl1,  speed,          sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  ball,           sensorTouch)
#pragma config(Sensor, dgtl4,  emergency,      sensorTouch)
#pragma config(Sensor, dgtl5,  launcherAngle,  sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  led,            sensorLEDtoVCC)
#pragma config(Sensor, dgtl12, LED,            sensorLEDtoVCC)
#pragma config(Motor,  port1,           leftLauncherTop, tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           rightLauncherTop, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           leftFrontDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           rightFrontDrive, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           intake1,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           intake2,       tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rightRearDrive, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           leftRearDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           rightLauncherBottom, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          leftLauncherBottom, tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int RPMAverage2 = 0;
int RPMAverage = 0;
long launcherTemp111 = 0;
long launcherTemp222 = 0;
long launcherRPM = 0;
float Kp = 0.004;
float Kd = 0.0;
float Kp2 = 0;
float Kd2 = 0;
int error = 0;
int target = 3400;
int derivative = 0;
int previousError = 0;
int error2 = 0;
int target2 = 0;
int derivative2 = 0;
int previousError2 = 0;
int rampRPM = 2000;
float temp1 = 100;
float rampingSpeed = 0;
float PDout = 0;
int time = 0;
bool PDenabled = false;
bool controlEnabled = false;
bool launcherEnabled = false;
string mode2 = "disabled";
bool isAuton = false;
bool master = false;
string test;
int ballNumber = 0;
//int stuff = 0;
//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Competition_Control.c"	 //Main competition background code...do not modify!



void setLauncher(float speed)
{
	motor[leftLauncherBottom] = (int) speed;
	motor[leftLauncherTop] = (int) speed;
	motor[rightLauncherBottom] = (int) speed;
	motor[rightLauncherTop] = (int) speed;
}

void setIntake(int power)
{
	motor[intake1] = power;
	motor[intake2] = power;
}

void rampDown()
{
	rampingSpeed = temp1;
	while(launcherRPM > 500)
	{
		rampingSpeed = (rampingSpeed - 1); //Should take about 5 seconds
		if(rampingSpeed < 10)
		{
			rampingSpeed = 10;
		}
		setLauncher(rampingSpeed);
		delay(50);
	}
	setLauncher(0);
}

void rampUp()
{
	rampingSpeed = 30;

	while(launcherRPM < rampRPM)
	{
		rampingSpeed = (rampingSpeed + 1); //Should take about 5 seconds
		setLauncher(rampingSpeed);
		delay(50);
	}
}


task launcherSpeedAverage()
{
	int RPMA1 = 0;
	int RPMA2 = 0;
	int RPMA3 = 0;
	int RPMA4 = 0;
	int RPMA5 = 0;
	int RPMA6 = 0;
	int RPMA7 = 0;
	int RPMA8 = 0;
	int RPMA9 = 0;
	int RPMA10 = 0;
	while(true)
	{
		for(int i = 0; i <= 9; i++)
		{
			RPMA10 = RPMA9;
			RPMA9 = RPMA8;
			RPMA8 = RPMA7;
			RPMA7 = RPMA6;
			RPMA6 = RPMA5;
			RPMA5 = RPMA4;
			RPMA4 = RPMA3;
			RPMA3 = RPMA2;
			RPMA2 = RPMA1;
			RPMA1 = launcherRPM;
			delay(50);
		}

		RPMAverage = ((RPMA1 + RPMA2 + RPMA3 + RPMA4 + RPMA5 + RPMA6 + RPMA7 + RPMA8 + RPMA9 + RPMA10) / 10);
	}
}

task launcherSpeedAverage2()
{
	int RPMA21 = 0;
	int RPMA22 = 0;
	int RPMA23 = 0;
	while(true)
	{
		for(int i = 0; i < 3; i++)
		{
			RPMA23 = RPMA22;
			RPMA22 = RPMA21;
			RPMA21 = launcherRPM;
			delay(50);
		}

		RPMAverage2 = ((RPMA21 + RPMA22 + RPMA23) / 3);
	}
}

task launcherSpeed()
{
	SensorValue[speed] = 0;

	while(true)
	{
		launcherTemp111 = SensorValue[speed];

		delay(50);
		launcherTemp222 = (SensorValue[speed] - launcherTemp111);

		launcherRPM = (((launcherTemp222 * 1200) / 360) * 35);	//need to take into account gear ratio between launcher wheel
		//and sensor. also, do I need to worry about how long it takes to perform the above calculations?
		//what about how often the quad encoder updates?
		//should I unplug the second port because I dont need to know which way its going?
	}
}

task PD()
{
	error = 0;
	previousError = 0;

	while(true)
	{
		if(PDenabled == true)
		{
			error = target - RPMAverage2;

			derivative = error - previousError;

			previousError = error;

			PDout = (error * Kp) + (derivative * Kd);
		}

		delay(50);
	}
}

task control()
{

	while(true)
	{
		if(controlEnabled == true)
		{
			temp1 += PDout ;

			if(temp1 < 0)
			{
				temp1 = 0;
			}
			setLauncher(temp1);

			delay(100);

			if(temp1 > 127)
			{
				temp1 = 127;
			}
			else if(temp1 < -127)
			{
				temp1 = -127;
			}
		}
	}
}

task PD2()
{
	while(true)
	{
		error2 = target2 - SensorValue[launcherAngle];

		derivative2 = error2 - previousError2;

		previousError2 = error2;

		//motor[launcherAngleAdjust] = (error2 * Kp2) + (derivative * Kd2);

		delay(50);
	}
}

task driveControl()
{
	while(true)
	{
		delay(50);
		if(vexRT[Btn8R] == 1)
		{
			ballNumber++;
			while(SensorValue[ball] == 1 && vexRT[Btn8R] == 1)
			{
				setIntake(-127);
				delay(10);
			}
			while(SensorValue[ball] == 0 && vexRT[Btn8R] == 1)
			{
				setIntake(-127);
				delay(10);
			}
			setIntake(10);
			delay(100);
			setIntake(0);
			delay(500);
			while(RPMAverage2 < 3100 && vexRT[Btn8R] == 1)
			{
				setIntake(0);
				delay(10);
			}
		}
		else
		{
			if(vexRT[Btn5U] == 1)
			{
				motor[intake1] = -127;
				//motor[intake2] = -127;
			}
			else if(vexRT[Btn5D] == 1)
			{
				motor[intake1] = 127;
				//motor[intake2] = 127;
			}
			else
			{
				motor[intake1] = 0;
				//motor[intake2] = 0;
			}

			if(vexRT[Btn6U] == 1)
			{
				motor[intake2] = -127;
			}
			else if(vexRT[Btn6D] == 1)
			{
				motor[intake2] = 127;
			}
			else
			{
				motor[intake2] = 0;
			}
		}

		if(abs(vexRT[Ch3]) > 10)
		{
			motor[leftFrontDrive] = vexRT[Ch3];
			motor[leftRearDrive] = vexRT[Ch3];
		}
		else
		{
			motor[leftFrontDrive] = 0;
			motor[leftRearDrive] = 0;
		}

		if(abs(vexRT[Ch2]) > 10)
		{
			motor[rightFrontDrive] = vexRT[Ch2];
			motor[rightRearDrive] = vexRT[Ch2];
		}
		else
		{
			motor[rightFrontDrive] = 0;
			motor[rightRearDrive] = 0;
		}
		if(RPMAverage2 >= 3100)
		{
			SensorValue[LED] = 1;
		}
		else
		{
			SensorValue[LED] = 0;
		}
	}
}

task distance()
{
while(true)
{
	if(vexRT[Btn8U] == 1)
	{
		target = 3200;
	}
	else if(vexRT[Btn8D] == 1)
	{
		target = 2800;
	}
	else if(vexRT[Btn8L] == 1)
	{
		target = 2400;
	}
	delay(50);
}
}



task launcherMaster()
{
PDenabled = false;
controlEnabled = false;
//	int suff = 9001;
master = true;
while(true)
{

	//mode == "usercontrol"
	if(!isAuton)
	{
		while(vexRT[Btn7U] == 0)
		{
			delay(100);
		}
		time1[T1] = 0;

		rampUp();
		temp1 = rampingSpeed;
		PDenabled = true;
		controlEnabled = true;

		while(vexRT[Btn7D] == 0)
		{
			delay(100);
		}

		PDenabled = false;
		controlEnabled = false;
		rampDown();
	}
	else if(isAuton)
	{
		while(launcherEnabled == false && isAuton)
		{
			delay(100);
		}
		time1[T1] = 0;

		rampUp();
		temp1 = rampingSpeed;
		PDenabled = true;
		controlEnabled = true;

		while(launcherEnabled == true && isAuton)
		{
			delay(100);
		}

		PDenabled = false;
		controlEnabled = false;
		rampDown();
	}


}

}

task timers()
{
while(true)
{
	delay(1000);
	time = time + 1;

}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//													Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()														//I might need to move the pre_auton call in the Vex_Competition_includes to line 54 instead
//of line 61.
{
mode2 = "preAuton";
bStopTasksBetweenModes = false;

//motor[launcherAngleAdjust] = -50;					//Can I move motors in preauton? should I move the call to it in Vex_Competition_includes to a
//waitUntil(SensorValue[angleReset] == 1);	//different spot?
//motor[launcherAngleAdjust] = 0;
//SensorValue[launcherAngle] = 0;

mode2 = "afterPreAuton";



}

/////////////////////////////////////////////////////////////////////////////////////////
//
//																 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
{
isAuton = true;
startTask(launcherMaster);							//All of this is for the launcher, and for diagnostics
PDenabled = false;
//startTask(PD2);
startTask(PD);
startTask(control);
startTask(launcherSpeed);
startTask(launcherSpeedAverage);
startTask(launcherSpeedAverage2);
startTask(timers);
mode2 = autonomous;


delay(100);


test = "AUTON_RUNNING";

//AutonomousCodePlaceholderForTesting();	// Remove this function call once you have "real" code.

launcherEnabled = true;

while(RPMAverage2 < 3200 && time < 13)
{
	delay(10);
}
for(int i = 0; i < 4; i++)
{
	if(time < 13)
	{
		ballNumber++;
		while(SensorValue[ball] == 1 && time < 13)
		{
			setIntake(-90);
			delay(10);
		}
		while(SensorValue[ball] == 0 && time < 13)
		{
			setIntake(-90);
			delay(10);
		}
		setIntake(10);
		delay(100);
		setIntake(0);
		delay(500);
		while(RPMAverage2 < 3300 && time < 13)
		{
			setIntake(0);
			delay(10);
		}
		delay(500);
	}
}

setIntake(-127);

launcherEnabled = false;

//delay(14000);

//launcherEnabled = false;


//motor[leftFrontDrive] = motor[leftRearDrive] = motor[rightFrontDrive] = motor[rightRearDrive] = 127;

//delay(3000);

//motor[leftFrontDrive] = motor[leftRearDrive] = -127;
//setLauncher(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//																 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{
isAuton = false;
mode2 = usercontrol;
startTask(driveControl);								//Start drive/intake code. Should I move the code in that task to the usercontrol task?
startTask(launcherMaster);
startTask(launcherMaster);							//All of this is for the launcher, and for diagnostics
PDenabled = false;
//startTask(PD2);
startTask(PD);
startTask(control);
startTask(launcherSpeed);
startTask(launcherSpeedAverage);
startTask(launcherSpeedAverage2);
startTask(timers);
startTask(distance);

// User control code here, inside the loop

while (true)
{
	delay(100);
	// This is the main execution loop for the user control program. Each time through the loop
	// your program should update motor + servo values based on feedback from the joysticks.

	// .....................................................................................
	// Insert user code here. This is where you use the joystick values to update your motors, etc.
	// .....................................................................................

	// Remove this function call once you have "real" code.
}
}
