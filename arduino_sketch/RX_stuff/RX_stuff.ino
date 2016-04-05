#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
#define LED_ON 7
#define LED_OFF 8
#define LED_ALERT 4
#define PIPE 0xE8E8F0F0E1LL
#define BUFFER_SIZE 32
#define DIM_GROUP 3

char stuff[BUFFER_SIZE];
bool full_string_complete = false, end_str = false;
int current_line = 0, previous_line = 4;
String rec = "";
RF24 radio(CE_PIN, CSN_PIN);

void activityLed (bool in_activity = true);
int conv_hex_to_dec(const char * shex, const unsigned int len);
int _pow (unsigned int base, unsigned int exponent);
void scan_str();

void setup() {
  Serial.begin(9600);
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_OFF, OUTPUT);
  pinMode(LED_ALERT, OUTPUT);
  digitalWrite(LED_OFF, HIGH);
  radio.begin();
  radio.openReadingPipe(1, PIPE);
  radio.setChannel(125);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();
  memset(stuff, 0, BUFFER_SIZE);
}//setup

void loop() {
  if (radio.available()) {
    while (!radio.read( stuff, BUFFER_SIZE )){}//wait transmission done and receive data.
    unsigned int i = 0;
    while (i < BUFFER_SIZE && !(end_str = stuff[i] == '!'))
      rec += stuff[i++];
    memset(stuff, 0, BUFFER_SIZE);
  }//if-available
  if (end_str) {
    activityLed();
    current_line = rec.charAt(0) - '0';
    digitalWrite(LED_ALERT, !((current_line == 0 && previous_line == 4) || (current_line == previous_line+1)));
    previous_line = current_line;
    scan_str();
    rec = "";
    end_str = false;
    activityLed(false);
  }//if
}//loop

void activityLed (bool in_activity){
  digitalWrite(LED_OFF, !in_activity);   
  digitalWrite(LED_ON, in_activity);
}//activityLed

//Scan rec string. Every 3 character function captures string and convert hex to dec.
void scan_str() {
  char group[DIM_GROUP];
  int m = 0;
  for (unsigned int i = 2; i < rec.length(); i++) {
    group[m] = rec.charAt(i);
    if ((i - 1) % DIM_GROUP == 0) {
      Serial.print(conv_hex_to_dec(group, DIM_GROUP));
      Serial.print(' ');
      m = 0;
    }//if
    else
      m++;
  }//for
  Serial.println();
}//scan_str

/*
  IP: shex const char rappresentation of hexadecimal number in char array.
  IV: len int shex's length.
  OR: shex converted into integer number.
*/
int conv_hex_to_dec(const char * shex, const unsigned int len) {
  unsigned int dec = 0;
  for (unsigned int i = 0; i < len; i++)
    dec += (_pow(16, len - i - 1)) * ( (shex[i] >= '0' && shex[i] <= '9') * (shex[i] - '0') + (shex[i] >= 'A' && shex[i] <= 'F') * (shex[i] - 'A' + 10) );
  return dec;
}//conv_hex_to_dec

//pow function for integer. So, the original one return result minus 1...?
int _pow (unsigned int base, unsigned int exponent) {
  unsigned int ris = 1;
  for (unsigned int i = 0; i < exponent; i++)
    ris *= base;
  return ris;
}


