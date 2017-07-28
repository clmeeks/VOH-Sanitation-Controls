
long dosingtimeON = 5000;                                           // Controls how long of a runtime both dosing pumps have.
long dosingtimeOFF = 15000;                                          // Controls how long of a break is taken between doses for both dosing pumps.
int dosingalternator = 1;                                          // Used to alternaite between dosing pumps 1 and 2 as the start pump in a cycle. This is done to balance the effects of the delay after the input pump runs. 
long dosingdelay = 0;                                              // Later used to calculate the gaps between the dosing pumps using the equiation dosingdelay = dosingdelay = (dosingtimeOFF-dosingtimeON)/2

long inputtimeON = 10000;                                            // Controls how long the input pump runs for
long inputtimeOFF = 15000;                                           // Controls how long of a break there is after the input pump runs
int inputratio = 3;                                                // Controls the ratio of input pump to dosing pump cycles. For example a ratio of 3 means that input pump will run once for every three cycles of dosing pumps
int ratiocount = 0;                                                // Used to advance the dosing cycle reletive to the inputratio.

long sludgetimeON = 30000;                                           // controls how long the sludge pump runs for when it is on
long sludgetimeOFF = 300000;                                         // controls how long the sludge pump is scheduled wait before kicking back on

int dosing1state = LOW;                                              // changes the on or off state of dosing pump 1, HIGH = ON and LOW = OFF
int dosing2state = LOW;                                              // changes the on or off state of dosing pump 2, HIGH = ON and LOW = OFF
int inputstate = LOW;                                                // changes the on or off state of input pump, HIGH = ON and LOW = OFF
int sludgestate = LOW;                                               // changes the on or off state of sludge pump, HIGH = ON and LOW = OFF

int stage = 0;                                                       // Used to advance the different stages of the dosing cycles.
int sludgestage = 0;                                                 // Used to initialize sludgtargettime

long internaltime = 0;                                               // Later used to store the current internal time of the arduino using the equation internaltime = millis()
long targettime = 0;                                                 // Used throughout the program to sheduled the input and dosing pumps.
long sludgetargettime = 1000;                                        // Used to shedule the target time for the sludge pump.
long armageddoncheck = 0;

void setup() 
{
  
pinMode(13, OUTPUT);                                                 // Defines pin 13 to be used as an output 
pinMode(12, OUTPUT);                                                 // Defines pin 12 to be used as an output 
pinMode(11, OUTPUT);                                                 // Defines pin 11 to be used as an output 
pinMode(10, OUTPUT);                                                 // Defines pin 10 to be used as an output 

}

void loop() 
{
dosingdelay = (dosingtimeOFF-dosingtimeON)/2;                        // Defines the delay time between the dosing pumps using dosingtimeOFF and dosingtimeON variables.
internaltime = millis();                                             // Updates internaltime variable with the arduino's current time.

if(internaltime >= 10000)
{
  internaltime = internaltime - 10000;
}

if(1 + internaltime < armageddoncheck && internaltime == 0)            // When the interal timer resets to 0 this makes ure the sludgetargettime and targettime variables are reletive to the actual internal time.
{
sludgetargettime = sludgetargettime - armageddoncheck;
targettime = targettime - armageddoncheck;
}

armageddoncheck = millis();                                           // Updates armageddoncheck with the current inernal time so that the above code can check if the interal clock has reset to 0.

if(armageddoncheck >= 10000)
{
  armageddoncheck = armageddoncheck - 10000;
}
                                                                 
                                                                         // When the system boots up it starts out by running the sludge pump for sludgetimeON
  
  if(sludgestage == 0 && stage == 0 || sludgetargettime > internaltime)  // Runs sluge pump for duration of sludgetimeON as soon as the dosing pumps finish a cyle and are not running.
  {
    if(sludgestage == 0)                                                 // If it is time to start the sludge pump then this defines the target run time reletive to the interaltime using the slugetimeON variable
  {
  sludgetargettime = internaltime + sludgetimeON;
  sludgestage = 1;
  }
    sludgestate = HIGH;
  }
  else if((sludgetargettime + sludgetimeOFF) > internaltime || sludgestage == 0)          // If the sludge pump is done running then the input and dosing cycle begins and continues until after sludgetargettime + sludge tarter time off or sludgestep = 0.
  {
  sludgestate = LOW;                                                  // Turns the sludge pump off.
                                                                   
    if(ratiocount == 0)                                               // Ratiocount keeps track of how many times the dosing pumps have been run reletive to the input pump. This checks to see if we are at the start of a cycle.
    {
      if(stage == 0)
      {
      targettime = internaltime + inputtimeON;                       // If it is time to start the input pump then this defines the targettime reletive to the interaltime using the dosingtimeON variable.
      stage = 1;
      }
    
      if(targettime > internaltime)                                  // If the target time has not yet been reached then this returns true and runs the input pump. If it returns false then it goes to the next else if.
      {
      inputstate = HIGH;
      }
      else if(targettime + dosingdelay > internaltime)               // If the target time plus the off time has not yet been reached then this keeps the input pump in an off state.
      {
      inputstate = LOW;                                              // Keeps the system off for dosingdelay time.
      }
      else
      {
      ratiocount = 1;                                                // After the input pump has run the ratiocount advances to 1 which kicks the code to the next else if.
      stage = 0;
      }
    }
    else if(ratiocount <= inputratio)                                // This runs both of the dosing pumps for inputratio number of times.
    {
        if(stage == 0)                                               // Stages are used to advance the code through the schedule. Stage 0 is used to define the target cutoff time reletive to the current time using dosingtimeON.
        {
        targettime = internaltime + dosingtimeON;
        stage = 1;
        }
        else if(stage == 1)                                          // Stage 1 keeps the first pump on for dosingtimeON ammount of time.
        {
          if(targettime > internaltime)                              // If the target time has not been reached then the first dosing pump in the cycle is kept in an ON state.
          {
            if(dosingalternator == 1)                                // Chooses to activate pump 1 or 2 as the first pump based on an alternating variable. This is done to keep one dosing tank from having an imballenced dosing and delay time.
            {
            dosing1state = HIGH; 
            }
            else
            {
            dosing2state = HIGH;
            }  
          }
          else                                                       // If the target time has been reached or passed then the stage is advanced to stage 2.
          {
          stage = 2;
          }
        }
        else if(stage == 2)                                          // Stage 2 keeps the first pump off for dosingtimeOFF ammount of time.
        {
          if((targettime + dosingdelay) > internaltime)
          {
          if(dosingalternator == 1)                                  // Chooses to shut off pump 1 or 2 as the first pump based on an alternating variable. This is done to keep one pump from havin an imballenced dosing and delay time.
            {
            dosing1state = LOW; 
            }
            else
            {
            dosing2state = LOW;
            }    
          }
          else
          {
          stage = 3;                                                 // Once the first pump's dosing delay time has been reached this kicks the code to stage 3.
          }
        }
        else if(stage == 3)                                          // Stage 3 is used to redefine the target cutoff time reletive to the current time using dosingtimeON.
        {
        targettime = internaltime + dosingtimeON;
        stage = 4;                                                   // This kicks the code to stage 4.
        }
        else if(stage == 4)                                          // Stage 4 keeps the second pump on for dosingtimeON ammount of time.
        {
         if(targettime > internaltime)                               // If the target time has not been reached then dosing pump 2 is kept in an ON state.
         {
           if(dosingalternator == 1)                                 // Chooses to activate pump 1 or 2 as the second pump based on an alternating variable. This is done to keep one pump from havin an imballenced dosing and delay time.
            {
            dosing2state = HIGH; 
            }
            else
            {
            dosing1state = HIGH;
            } 
         }
         else
          {
          stage = 5;                                                  // Once the dosing delay has been reached this kicks the code to stage 5.
          }
         }
         else if(stage == 5)                                          // Stage 5 keeps the second pump off for dosingtimeOFF ammount of time.
         {
            if((targettime + dosingdelay) > internaltime)             // Until target time plus the dosing delay time has not been reached this keeps the dosting pump 2 in an off state.
            {
              if(dosingalternator == 1)                               // Chooses to turn off pump 1 or 2 as the second pump based on an alternating variable. This is done to keep one pump from having an imballenced dosing and delay time.
              {
              dosing2state = LOW; 
              }
              else
              {
              dosing1state = LOW;
              }  
            }
            else
            {
            ratiocount = ratiocount + 1;                             // Once both pumps have had their on times as well as their delay times the ratiocount is advanced.
            stage = 0;                                               // The stage variable is reset for use in the next cycle.
            }
          }
        } 
      else
      {
        if (dosingalternator == 1)                                   // This alternates the first dosing pump in the dosing cycle between pump one and pump two.
         {
         dosingalternator = 2;
         }
         else
        {
        dosingalternator = 1;
        }
       ratiocount = 0;                                               // Resets ratiocount so that the dosing cycle can start again with the input pump.
       }
  }
  else
  {
  sludgestage = 0;                                                 // This allows the sludge pump to run once the sludgetimeOFF time is up.
  }

digitalWrite(13,dosing1state);                                     // Turns Dosing Pump 1 on and off by setting digital pin 13 to HIGH or LOW depending on the variable dosing1state.
digitalWrite(12,dosing2state);                                     // Turns Dosing Pump 2 on and off by setting digital pin 12 to HIGH or LOW depending on the variable dosing2state.
digitalWrite(11,inputstate);                                       // Turns Input Pump on and off by setting digital pin 11 to HIGH or LOW depending on the variable inputstate.
digitalWrite(10,sludgestate);                                      // Turns Sludge Pump on and off by setting digital pin 10 to HIGH or LOW depending on the variable sludgestate.

}
