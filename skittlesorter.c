int colourReading()
{
	int readingsCount[8] = {0,0,0,0,0,0,0,0};
	int readings[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	for (int reading =0; reading <15; reading++)
	{
		motor[motorA] = 30;
		wait1Msec(20);
		motor[motorA] = 0;
		wait1Msec(20);
		motor[motorA] = -30;
		wait1Msec(10);
		motor[motorA] = 0;
		wait1Msec(20);
		readings[reading] = SensorValue[S1];
	}
	for (int reading =0; reading < 15; reading++)
		readingsCount[readings[reading]] ++;

	for (int size =0; size <8; size++)
	{
		if (readingsCount[size] > 6)
		{
			displayString(5, "%d", SensorValue[S1]);
			return size;
		}
	}
	return 0;
}

void userInput(int *jars)
{
	string colourNames[3] = {"yellow", "blue", "brown"};//DO NOT REMOVE
	for(int count =0; count <3; count++)
	{
		displayString(3,"What jar would you like %s skittles in?", colourNames[count]);

		while(!getButtonPress(buttonAny))
		{}

		if(getButtonPress(buttonLeft))//if statement for each colour
		{
			while(getButtonPress(buttonLeft))
			{}
			jars[count] = -90; //populate array with motor encoder values of each jar
		}
		else if (getButtonPress(buttonUp))
		{
			while(getButtonPress(buttonUp))
			{}
			jars[count] = -60;
		}

		else if(getButtonPress(buttonRight))
		{
			while(getButtonPress(buttonRight))
			{}
			jars[count] = -30;
		}
	}
	eraseDisplay();
}

bool rotate()
{
	time1[T1] =0;

	motor[motorB] = 30;

	while(SensorValue[S1] == 6  && time1[T1] < 20000)//may see something when nothing is there and give a false positive
	{}
	motor[motorB] = 0;
	displayString(15,"Time: %d", time1[T1]);
	if(time1[T1] > 19998)
		return false;
	return true;

}

void openChamber(tMotor motorA)
{
	motor[motorA] = -50; //make sure that the correct motor is being called and the power moves it in the right direction
	while(nMotorEncoder[motorA] >-30)
	{}
	motor[motorA] =0;
	wait1Msec(50); //test for time that works

	motor[motorA] = 50; //make sure that the correct motor is being called and the power moves it in the right direction
	while(nMotorEncoder[motorA] <0)
	{}
	motor[motorA] =0;
}

void display(int time, int skittleCount, int*coloursCount)
{
	string colourNames[3] = {"yellow", "blue", "brown"};//DO NOT REMOVE

	displayString(3, "Total # of skittles sorted: %d", skittleCount/2);
	int line = 4;

	for (int index = 0; index < 3; index++)
	{
		displayString(line+index, "%s skittle sorted: %d", colourNames[index], coloursCount[index]);
	}

	displayString(7, "Skittles sorted / second: %f ", (float)skittleCount/((time-20000)/1000));//time-20... yes or no?
	displayString(8, "Seconds to sort 1: %f ", 1/( (float)skittleCount/((time-20000)/1000)) ) ;
	displayString(9, "Time: %f",time);
}

void turn_arm(tMotor motorM, int index, int speed) {
	int i_angle = nMotorEncoder[motorM];
	if(index - i_angle > 0)
	{
		motor[motorM] = speed;
		while(nMotorEncoder[motorM] < i_angle + (index - i_angle))
		{}
		motor[motorM] = 0;
	}
	else
	{
		motor[motorM] = -speed;
		while(nMotorEncoder[motorM] >= i_angle + (index - i_angle))
		{}
		motor[motorM] = 0;
	}
}

void zeroArm()
{
	motor[motorD] =	40;
	while(SensorValue[S2] == 0){}
	motor[motorD] = 0;
	nMotorEncoder[motorD] = 0;
}

void CalibrateMotors(){
	zeroArm();
	if(SensorValue[S1] != (int)colorWhite)
	{
		motor[motorA] = 30;
		wait1Msec(400);
		while(SensorValue[S1] != (int)colorWhite){}
		motor[motorA] = motor[motorD] = 0;
	}
	nMotorEncoder[motorA] = 0;
}


task main()
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Touch;
	CalibrateMotors();


	int currentColour =0, skittleCount = 0;
	int colours[3] = {4,2,7};
	int jarsIndex[3] ={0,0,0};
	string colourNames[3] = {"yellow", "blue", "brown"};
	int coloursCount[3] = {0,0,0};

	userInput(jarsIndex);

	time1[T2] = 0; //timing the entire sorting process

	while(rotate())
	{
		currentColour = colourReading();
		displayString(5, "%d", currentColour);
		if (currentColour == 0)
		{
			turn_arm(motorD, 0, 30);
		}
		else
		{
			for (int index = 0; index <3; index++)
			{
				if(currentColour == 1 && colours[index] == 7)
				{
					turn_arm(motorD,jarsIndex[index], 40);
					coloursCount[2]++;
					skittleCount++;
				}

				else if (colours[index] == currentColour)
				{
					displayString(1,"%d", jarsIndex[index]);
					turn_arm(motorD, jarsIndex[index], 40);//move leading arm
					if(colours[index] == 2)
					{
						coloursCount[1]++;
						skittleCount++;
					}
					else if(colours[index] == 4)
					{
						coloursCount[0]++;
						skittleCount++;
					}
					else if(colours[index] == 7)
					{
						coloursCount[2]++;
						skittleCount++;
					}


				}
			}
		}
		wait1Msec(200);
		openChamber(motorA);//make sure its the right motor
		skittleCount ++;
	}

	zeroArm();

	/*
	for (int index = 0; index < 3; index++)
	{
		displayString(4+index, "%s skittle sorted: %d", colourNames[index], coloursCount[index]);
	}
	displayString(7, "Skittles per second: %f", (float)skittleCount/(time1[T2]-20));

	*/


	display(time1[T2], skittleCount, coloursCount);
	wait1Msec(200000000);
}