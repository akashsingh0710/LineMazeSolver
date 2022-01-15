#define leftFarSensor      8
#define leftNearSensor     7
#define leftCenterSensor   6
#define rightCenterSensor  5
#define rightNearSensor    4
#define rightFarSensor     9

int leftCenterReading;
int leftNearReading;
int leftFarReading;
int rightCenterReading;
int rightNearReading;
int rightFarReading;

int leftNudge;
int replaystage;
int rightNudge;

#define leapTime 140
#define turnSpeed 100
#define forwardSpeed 170
#define correctionSpeed 140
#define turnSoftner 40

#define leftMotor1  A4
#define leftMotor2  A3
#define rightMotor1 A5
#define rightMotor2 A2
#define leftEnable  10
#define rightEnable 11

int correction = 0;
  int Deviation = 0;
  int PreviousDeviation = 0;
  
  #define Kp 0  //PID constants
  #define Kd 0


char path[100] = {};
int pathLength;
int readLength;




void setup() {
   pinMode(leftCenterSensor, INPUT);
  pinMode(leftNearSensor, INPUT);
  pinMode(leftFarSensor, INPUT);
  pinMode(rightCenterSensor, INPUT);
  pinMode(rightNearSensor, INPUT);
  pinMode(rightFarSensor, INPUT);
    
  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(leftEnable, OUTPUT);
  pinMode(rightEnable, OUTPUT);


  pinMode(3,OUTPUT);  
  pinMode(2, OUTPUT);
  
  digitalWrite(2, HIGH);
  digitalWrite(3,LOW);
  

  analogWrite(leftEnable, 100-7);
  analogWrite(rightEnable, 100);
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(50);

}


void loop(){
 readSensors();
  KeepMoving(); 
                                                                                     
 
 if(leftFarReading == 1 && rightFarReading == 1 && (leftCenterReading == 0 || rightCenterReading == 0) )                          
                                                                                                                             //If we do not see a left or right turn
                                                                                                                            // and at least 1 center sensor sees the black line
   {straight();}                                                                                                              //keep driving straight                               
 
  else{                                                                                              
    leftHandWall();                                                                                                         //else we must make a decision                                                                             
      }

}

void readSensors(){
  
  leftCenterReading  = digitalRead(leftCenterSensor);
  leftNearReading    = digitalRead(leftNearSensor);
  leftFarReading     = digitalRead(leftFarSensor);
  rightCenterReading = digitalRead(rightCenterSensor);
  rightNearReading   = digitalRead(rightNearSensor);
  rightFarReading    = digitalRead(rightFarSensor);  

}


void leftHandWall(){
  analogWrite(leftEnable, 100-7);
  analogWrite(rightEnable, 100);                                                                  // full speed ahead for leap
  
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);

  if( leftFarReading == 0 && rightFarReading == 0)
                                                                                                    //both sensors on black
  {   delay(leapTime);                                                                              //leap
      readSensors();                                                                                 //recheck sensors after leap
   
    if(leftFarReading == 0 || rightFarReading == 0)                                   // if both are still on black then we are done
      {  done(); }                                                                                // we are done solving
    
    if(leftFarReading == 1 && rightFarReading == 1)                                    //both are on white which means we need to turn left now
      {  pause(); turnLeft(); }                                                                             //turn left
  }   
  
  if(leftFarReading == 0){                                                                  // if you can turn left then turn left
    delay(leapTime);                                                                              //leap
    readSensors();                                                                                //recheck sensors after leap
    if(leftFarReading == 1 && rightFarReading == 1)                                 // both are one white so turn left
     {  pause(); turnLeft();  }                                                                               // turn left
    
    else
     { done();    }                                                                                // one sees black? then we are done
    
  }
   
  if(rightFarReading == 0){                                                                 //no left turn, maybe we need to turn right then
    delay(10);                                                                                    // drive forward a hair
    readSensors();                                                                                //recheck sensors
    if(leftFarReading == 0){                                                                // are we positive we can't turn left
      delay(leapTime-10);                                                                         // leap
      readSensors();                                                                              //read sensors
      if(rightFarReading == 0 && leftFarReading == 0){                                // if all are black
        done();                                                                                   // we are done
      }
      else{ 
         pause(); turnLeft();                                                                               //else we need to turn left
        return;
      }
    }
    delay(leapTime-10);                                                                           //leap
    readSensors();                                                                                // rechekc sensors
    if(leftFarReading == 1 && leftCenterReading == 1 &&
      rightCenterReading == 1 && rightFarReading == 1){                               //all are white meaning this is a required right turn
      pause(); turnRight();                                                                                // turn right
      return;
    }
    // all was not white so we can continue driving forward
    path[pathLength]='S';
    pathLength++;
    if(path[pathLength-2]=='B'){
      shortPath();
    }
    straight();
  }
  
  readSensors();
  if(leftFarReading == 1 && leftCenterReading == 1 && rightCenterReading == 1 
    && rightFarReading == 1 && leftNearReading == 1 && rightNearReading == 1){                              // all is white, turn around.
    turnAround();
  }
}


//void done(){
//  digitalWrite(leftMotor1, LOW);
//  digitalWrite(leftMotor2, LOW);
//  digitalWrite(rightMotor1, LOW);
//  digitalWrite(rightMotor2, LOW);
//  replaystage=1;
//  path[pathLength]='D';
//  pathLength++;
// while(digitalRead(leftFarSensor) == 0){
//  // digitalWrite(led, LOW);
//  // delay(150);
//  // digitalWrite(led, HIGH);
//  // delay(150);
// }
// delay(1000);
//  replay();
//}
void done()
{
 straight(); 
}

void turnLeft(){
  
  while(digitalRead(rightCenterSensor) == 0 || digitalRead(leftCenterSensor) == 0){
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(leftEnable, turnSpeed-7);
    analogWrite(rightEnable, turnSpeed);
  }
    
  while(digitalRead(leftFarSensor) == 1){
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(leftEnable, turnSpeed-7);
    analogWrite(rightEnable, turnSpeed);
  }
  
  while(digitalRead(leftCenterSensor) == 1){
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(leftEnable, turnSpeed-(turnSoftner)-7);
    analogWrite(rightEnable, turnSpeed-(turnSoftner));
  }   
   
  if(replaystage==0){
    path[pathLength]='L';
    pathLength++;
    if(path[pathLength-2]=='B'){
      shortPath();
    }
  }
  correct(); 
  pause();
  straight();
}

void turnRight(){
   while(digitalRead(rightFarSensor) == 1){
     digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite(leftEnable, turnSpeed-7);
    analogWrite(rightEnable, turnSpeed);
  }
   while(digitalRead(rightCenterSensor) == 1){
     digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite(leftEnable, turnSpeed-(turnSoftner)-7);
    analogWrite(rightEnable, turnSpeed-(turnSoftner));
  }  

  if(replaystage==0){
    path[pathLength]='R';
    pathLength++;
    if(path[pathLength-2]=='B'){
      shortPath();
    }
  }
  correct();
  pause();
  straight();
}

void straight(){
  readSensors();
  digitalWrite(leftMotor1,HIGH);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,HIGH);
  digitalWrite(rightMotor2,LOW);
  analogWrite(leftEnable,forwardSpeed+correction-7);
  analogWrite(rightEnable,forwardSpeed-correction);

}


//void straight(){
//  if( digitalRead(leftCenterSensor) == 1){
//    digitalWrite(leftMotor1, HIGH);
//    digitalWrite(leftMotor2, LOW);
//    digitalWrite(rightMotor1, HIGH);
//    digitalWrite(rightMotor2, LOW);
//    analogWrite(leftEnable, 163);
//    analogWrite(rightEnable, correctionSpeed);
//    return;
//  }
//  else if(digitalRead(rightCenterSensor) == 1){
//    digitalWrite(leftMotor1, HIGH);
//    digitalWrite(leftMotor2, LOW);
//    digitalWrite(rightMotor1, HIGH);
//    digitalWrite(rightMotor2, LOW);
//    analogWrite(leftEnable, correctionSpeed-7);
//    analogWrite(rightEnable, 170);
//    return;
//  }
//  else{
//    digitalWrite(leftMotor1, HIGH);
//    digitalWrite(leftMotor2, LOW);
//    digitalWrite(rightMotor1, HIGH);
//    digitalWrite(rightMotor2, LOW);
//    analogWrite(leftEnable, forwardSpeed-7);
//    analogWrite(rightEnable, forwardSpeed);
//  }
//}

void turnAround(){
   analogWrite(leftEnable, 100-7);
   analogWrite(rightEnable, 100);
   digitalWrite(leftMotor1, HIGH);
   digitalWrite(leftMotor2, LOW);
   digitalWrite(rightMotor1, HIGH);
   digitalWrite(rightMotor2, LOW);
   delay(leapTime);
   
   while(digitalRead(leftFarSensor) == 1){
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(leftEnable, turnSpeed-7);
    analogWrite(rightEnable, turnSpeed);
   }
   while(digitalRead(leftCenterSensor) == 1){
     digitalWrite(leftMotor1, LOW);
     digitalWrite(leftMotor2, HIGH);
     digitalWrite(rightMotor1, HIGH);
     digitalWrite(rightMotor2, LOW);
     analogWrite(leftEnable, turnSpeed-(turnSoftner+30)-7);
     analogWrite(rightEnable, turnSpeed-(turnSoftner+30));
   }

  path[pathLength]='B';
  pathLength++;
  correct();
  pause();
  straight();
}

void pause()
{    digitalWrite(leftMotor1, HIGH);
     digitalWrite(leftMotor2, HIGH);
     digitalWrite(rightMotor1, HIGH);
     digitalWrite(rightMotor2, HIGH);
     analogWrite(leftEnable, 255);
     analogWrite(rightEnable, 255);
delay(100);
}

void correct()
{  digitalWrite(leftMotor1,HIGH);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,HIGH);
  digitalWrite(rightMotor2,LOW);
  analogWrite(leftEnable,turnSpeed+correction-7);
  analogWrite(rightEnable,turnSpeed-correction);
}

//void correction()
//{while(digitalRead(leftCenterSensor)!=0 || digitalRead(rightCenterSensor)!=0)
// {readSensors(); 
//  
//   if(leftFarSensor == 0 && leftNearSensor == 0)
//    { while(digitalRead(leftCenterSensor)!=0 || digitalRead(rightCenterSensor)!=0)
//      {
//      analogWrite(leftEnable,turnSpeed-(turnSoftner)-8  );
//      analogWrite(rightEnable, turnSpeed-(turnSoftner) );
//      digitalWrite(leftMotor1, LOW);
//      digitalWrite(leftMotor2, HIGH);
//      digitalWrite(rightMotor1, HIGH);
//      digitalWrite(rightMotor2, LOW);
//        }}
//      
// 
// if(leftFarSensor == 1 && leftNearSensor == 0)
//     { while(digitalRead(leftCenterSensor)!=0 || digitalRead(rightCenterSensor)!=0)
//       {
//       analogWrite(leftEnable,turnSpeed-(turnSoftner)-16);
//       analogWrite(rightEnable, turnSpeed-(turnSoftner) );
//       digitalWrite(leftMotor1, LOW);
//       digitalWrite(leftMotor2, HIGH);
//       digitalWrite(rightMotor1, HIGH);
//       digitalWrite(rightMotor2, LOW);
//         }}
// 
// if(rightFarSensor == 1 && rightNearSensor == 0)
//     { while(digitalRead(leftCenterSensor)!=0 || digitalRead(rightCenterSensor)!=0)
//       {
//       analogWrite(leftEnable,turnSpeed-(turnSoftner)-16);
//       analogWrite(rightEnable,turnSpeed-(turnSoftner));
//       digitalWrite(leftMotor1, HIGH);
//       digitalWrite(leftMotor2, LOW);
//       digitalWrite(rightMotor1, LOW);
//       digitalWrite(rightMotor2, HIGH);
//     }}
// 
// if(rightFarSensor == 1 && rightNearSensor == 0)
//     {while(digitalRead(leftCenterSensor)!=0 && digitalRead(rightCenterSensor)!=0)
//       {
//       analogWrite(leftEnable,turnSpeed-(turnSoftner)-8);
//       analogWrite(rightEnable,turnSpeed-(turnSoftner));
//       digitalWrite(leftMotor1, HIGH);
//       digitalWrite(leftMotor2, LOW);
//       digitalWrite(rightMotor1, LOW);
//       digitalWrite(rightMotor2, HIGH);
//     }}
//readSensors(); }
//}

void shortPath(){
  int shortDone=0;
  if(path[pathLength-3]=='L' && path[pathLength-1]=='R'){
    pathLength-=3;
    path[pathLength]='B';
    shortDone=1;
  }
  if(path[pathLength-3]=='L' && path[pathLength-1]=='S' && shortDone==0){
    pathLength-=3;
    path[pathLength]='R';
    shortDone=1;
  }  
  if(path[pathLength-3]=='R' && path[pathLength-1]=='L' && shortDone==0){
    pathLength-=3;
    path[pathLength]='B';
    shortDone=1;
  }
  if(path[pathLength-3]=='S' && path[pathLength-1]=='L' && shortDone==0){
    pathLength-=3;
    path[pathLength]='R';
    shortDone=1;
  }  
  if(path[pathLength-3]=='S' && path[pathLength-1]=='S' && shortDone==0){
    pathLength-=3;
    path[pathLength]='B';
    shortDone=1;
  }
  if(path[pathLength-3]=='L' && path[pathLength-1]=='L' && shortDone==0){
    pathLength-=3;
    path[pathLength]='S';
    shortDone=1;
  }
  path[pathLength+1]='D';
  path[pathLength+2]='D';
  pathLength++;
}




void replay(){
  readSensors();
  if(leftFarReading == 1 && rightFarReading == 1){
    straight();
  }
  else{
    analogWrite(leftEnable, 100);
    analogWrite(rightEnable, 100);
    if(path[readLength]=='D'){
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(100);
      digitalWrite(leftMotor1, LOW);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, LOW);
      digitalWrite(rightMotor2, LOW);
      endMotion();
    }
    if(path[readLength]=='L'){
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      turnLeft();
    }
    if(path[readLength]=='R'){
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      turnRight();
    }
    if(path[readLength]=='S'){
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      straight();
    }
    readLength++;
  }  
  replay();
}

void endMotion(){
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);  
 // digitalWrite(led, LOW);
 // delay(500);
 // digitalWrite(led, HIGH);
 // delay(200);
 // digitalWrite(led, LOW);
 // delay(200);
 // digitalWrite(led, HIGH);
 // delay(500);
  endMotion();
}

void KeepMoving(){

  readSensors();
  
  if(  leftFarReading==0 && leftNearReading ==1 && leftCenterReading==1 && rightCenterReading==1 && rightNearReading==1 && rightFarReading==1)
  Deviation=5;
  if(leftFarReading==0 && leftNearReading ==0 && leftCenterReading==1 && rightCenterReading==1 && rightNearReading==1 && rightFarReading==1)
  Deviation=4;
  if(leftFarReading==1 && leftNearReading ==0 && leftCenterReading==0 && rightCenterReading==1 && rightNearReading==1 && rightFarReading==1)
  Deviation=2;
  if(leftFarReading==1 && leftNearReading ==1 && leftCenterReading==0 && rightCenterReading==0 && rightNearReading==1 && rightFarReading==1)
  Deviation=0;
  if(leftFarReading==1 && leftNearReading ==1 && leftCenterReading==1 && rightCenterReading==0 && rightNearReading==0 && rightFarReading==1)
  Deviation=-2;
  if(leftFarReading==1 && leftNearReading ==1 && leftCenterReading==1 && rightCenterReading==1 && rightNearReading==0 && rightFarReading==0)
  Deviation=-4;
  if(leftFarReading==1 && leftNearReading ==1 && leftCenterReading==1 && rightCenterReading==1 && rightNearReading==1 && rightFarReading==0)
  Deviation=-5;

  correction = Kp*Deviation + Kd*(Deviation-PreviousDeviation);
  PreviousDeviation = Deviation;
  
}
