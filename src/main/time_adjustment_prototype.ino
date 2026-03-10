
uint8_t command_buffer_position = 0;
const uint8_t COMMAND_BUFFER_SIZE = 64;
char COMMAND_BUFFER[COMMAND_BUFFER_SIZE];
bool command_started = false;
unsigned long COMMAND_RECEIVING_LAPSE_MS = 3000;
unsigned long command_started_time_ms = 0;
// unsigned long current_time_ms = 0;

void commandHandler(){
  // SET DATE
  // "%SXXXX,XX,XX,XX,XX,XX"
  // INCREMENT TIME
  // "%I" + string of time to increment
  // DECREMENT TIME
  // "%D" + string of time to decrement
}

void setup_prototype(){
  
}

void loop_prototype(void){
  if(command_started){
    current_time_ms = millis();
    if(current_time_ms - command_started_time_ms >= COMMAND_RECEIVING_LAPSE_MS){
      command_started = false;
    }
  }
}

void serialEvent(){
  while (Serial.available()) {
    char RXChar = (char)Serial.read();
    
    if (!command_started){
      if (RXChar == '%'){
        command_started = true;
        command_buffer_position = 0;
      }
    }
    if (command_started){
      COMMAND_BUFFER[command_buffer_position++] = RXChar;
      if (command_buffer_position == COMMAND_BUFFER_SIZE){
        command_started = false;
      }
      if (RXChar == '\n'){
        command_started = false;
        commandHandler();
      } 
    }
  }
}