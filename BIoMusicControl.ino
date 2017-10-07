#define POT_MOTOR A0
#define POT_A A1
#define POT_B A2

#define TOGGLE_MOTOR 2
#define TOGGLE_A 6

#define BUTTON_A 3
#define BUTTON_B 4
#define BUTTON_C 5

#define BUTTON_RESET 7
#define BUTTON_PAC 8
#define BUTTON_GHOST 9

#define MOTOR_1 10
#define MOTOR_2 11

#define DTHRES 10
#define BDELAY 1000

bool _state_toggle_motor;
bool _state_toggle_a;

int _val_pot_motor;
int _val_pot_a;
int _val_pot_b;

int _val_button_a,_val_button_b,_val_button_c;
int _val_button_reset,_val_button_pac,_val_button_ghost;

int _val_remote_motor;



void setup(){
  pinMode(TOGGLE_MOTOR,INPUT);
  pinMode(BUTTON_A,INPUT);
  pinMode(BUTTON_B,INPUT);
  pinMode(BUTTON_C,INPUT);
  pinMode(TOGGLE_A,INPUT);
  pinMode(BUTTON_RESET,INPUT);
  pinMode(BUTTON_PAC,INPUT);
  pinMode(BUTTON_GHOST,INPUT);

  pinMode(MOTOR_1,OUTPUT);
  pinMode(MOTOR_2,OUTPUT);

  _state_toggle_motor=digitalRead(TOGGLE_MOTOR)==HIGH;
  _state_toggle_a=digitalRead(TOGGLE_A)==HIGH;

  _val_pot_a=analogRead(POT_A);
  _val_pot_b=analogRead(POT_B);
  _val_pot_motor=analogRead(POT_MOTOR);
  sendVal("speed_a",map(_val_pot_a,0,1024,0,255));
  sendVal("speed_b",map(_val_pot_b,0,1024,0,255));
  

  _val_button_a=_val_button_b=_val_button_c=0;
  _val_button_reset=_val_button_pac=_val_button_ghost=0;

  _val_remote_motor=_val_pot_motor;
  
  Serial.begin(9600);
}

void loop(){

  //read motor
  if(Serial.available()){
    String str=Serial.readStringUntil('#');
    
    if(str.indexOf("motor_toggle")>-1){
      _state_toggle_motor=!_state_toggle_motor;
    }else if(str.indexOf("motor_speedup")!=-1){
       //Serial.println(_val_pot_motor);
      _val_remote_motor=512;
      //if(_val_remote_motor>1024) _val_remote_motor=1024;
      
    }else if(str.indexOf("motor_slowdown")!=-1){
      // Serial.println(_val_pot_motor);
      _val_remote_motor=0;
     // if(_val_remote_motor<0) _val_remote_motor=0;
    }else if(str.indexOf("motor_toggle")!=-1){
      _state_toggle_motor=!_state_toggle_motor;
    }
  }

  
  if(_val_button_a<=0){
      if(digitalRead(BUTTON_A)==HIGH){
        sendMode(0);
        _val_button_a=BDELAY;
      }
  }else _val_button_a--;
  
  
  if(_val_button_b<=0){
      if(digitalRead(BUTTON_B)==HIGH){
        sendMode(1);
        _val_button_b=BDELAY;
      }
  }else _val_button_b--;
  
  
  if(_val_button_c<=0){
      if(digitalRead(BUTTON_C)==HIGH){
        sendMode(2);
        _val_button_c=BDELAY;
      }
  }else _val_button_c--;
  

  if(_val_button_reset<=0){
      if(digitalRead(BUTTON_RESET)==HIGH){
        sendSignal("blob_reset");
        _val_button_reset=BDELAY;
      }
  }else _val_button_reset--;
  
  if(_val_button_pac<=0){
      if(digitalRead(BUTTON_PAC)==HIGH){
        sendSignal("add_pac");
        _val_button_pac=BDELAY;
      }
  }else _val_button_pac--;
  

  if(_val_button_ghost<=0){
      if(digitalRead(BUTTON_GHOST)==HIGH){
        sendSignal("add_ghost");
        _val_button_ghost=BDELAY;
      }
  }else  _val_button_ghost--;
  
  
  bool smotor=digitalRead(TOGGLE_MOTOR)==HIGH;
  if(smotor!=_state_toggle_motor){
    if(smotor) sendSignal("motor_clock");
    else sendSignal("motor_reverse");
   
    _state_toggle_motor=smotor;
  }
  bool sa=digitalRead(TOGGLE_A)==HIGH;
  if(sa!=_state_toggle_a){
    if(sa) sendSignal("scan_radial");
    else sendSignal("scan_line");
    _state_toggle_a=sa;
  }

 
  int va=analogRead(POT_A);
  if(abs(va-_val_pot_a)>DTHRES){
    _val_pot_a=va;
    sendVal("speed_a",map(_val_pot_a,0,1024,0,255));
  }
  int vb=analogRead(POT_B);
  if(abs(vb-_val_pot_b)>DTHRES){
    _val_pot_b=vb;
    sendVal("speed_b",map(_val_pot_b,0,1024,0,255));
  }

  int vm=analogRead(POT_MOTOR);
  if(abs(vm-_val_pot_motor)>DTHRES){
    _val_pot_motor=vm;
    _val_remote_motor=_val_pot_motor; // update if controlled
   // sendVal("speed_motor",map(_val_pot_motor,0,1024,0,255));
  }

   //write motor
  runMotor(_val_remote_motor);

}

void sendMode(int set_){
  Serial.println("#set_"+String(set_)+"#");
}
void sendSignal(String tag_){
  Serial.println("#"+tag_+"#");
}
void sendVal(String tag_,int val_){
  Serial.println("#"+tag_+"|"+val_+"#");
}

void runMotor(int val_){
  if(_state_toggle_motor){
    analogWrite(MOTOR_1,map(val_,0,1024,0,255));
    analogWrite(MOTOR_2,0);
  }else{
    analogWrite(MOTOR_2,map(val_,0,1024,0,255));
    analogWrite(MOTOR_1,0);
  }
//  Serial.print("motor_val=");
//  Serial.print(val_);
//  Serial.println("#");
}


