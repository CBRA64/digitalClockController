unsigned long current_time_ms = 0;

uint8_t command_buffer_position = 0;
const uint8_t COMMAND_BUFFER_SIZE = 64;
char COMMAND_BUFFER[COMMAND_BUFFER_SIZE];
bool command_started = false;
unsigned long COMMAND_RECEIVING_LAPSE_MS = 3000;
unsigned long command_started_time_ms = 0;

void commandHandler(){
  // INCREMENT TIME
  // "%I" + string of time to increment
  // DECREMENT TIME
  // "%D" + string of time to decrement
  String cmd_data = "";
  for(uint8_t i = 2; i < COMMAND_BUFFER_SIZE; i++){
    if (COMMAND_BUFFER[i] == 0){
      break;
    } 
    cmd_data += COMMAND_BUFFER[i];
  } 

  int32_t delta_time = cmd_data.toInt();
  
  if (COMMAND_BUFFER[1] == 'I'){
    Serial.print("Incremented: ");
    Serial.println(delta_time);
  } else if (COMMAND_BUFFER[1] == 'D'){
    Serial.print("Decremented: ");
    Serial.println(delta_time);
  } 
}

void setup(){
  Serial.begin(115200);
  Serial.println("TEST STARTED.");
  pinMode(13,OUTPUT);
}

void loop(void){
  if(command_started){
    current_time_ms = millis();
    if(current_time_ms - command_started_time_ms >= COMMAND_RECEIVING_LAPSE_MS){
      command_started = false;
    }
  }
  serial_event();
}

void serial_event(){
  char RXChar;
  
  while (Serial.available() > 0) {
    RXChar = Serial.read();
    
    if(RXChar == '\n' || RXChar == '%' || isAlphaNumeric(RXChar)){
      if (RXChar == '%'){
        command_started = true;
        command_buffer_position = 0;
        delay(1);
      }
      if (command_started){
        COMMAND_BUFFER[command_buffer_position++] = RXChar;
        if (command_buffer_position >= (COMMAND_BUFFER_SIZE-1)){
          command_started = false;
        }
        if (RXChar == '\n'){
          command_started = false;
          commandHandler();
          command_buffer_position = 0;
        }
      }
    }
  }

}