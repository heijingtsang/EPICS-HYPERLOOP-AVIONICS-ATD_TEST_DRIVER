#include <SPI.h>

/**
 * Avionics System Program
 * 
 * @author Hei Jing Tsang
 * @date March 24, 2017
 * 
 * Description:
 * Program that deals with reading binary inputs from sensors via ATD module,
 * CMOS Single 8-Channel Analog Multiplexer/Demultuplexer with Logic-Level Conversion,
 * and a bit shifter module. This program will print out a 2D array of voltage values,
 * corresponding to each channel of input reading from different chips values, to show that the system works.
 * 
 */

#define SELECT_PIN_0 3
#define SELECT_PIN_1 4
#define SELECT_PIN_2 5
#define MASTER_PIN   2

byte channel_value = 0;
byte chip_value = 0;
double voltage[2][8];
unsigned int read_atd_value;
double converted_value;


void setup() {
  // put your setup code here, to run once:
  pinMode(MASTER_PIN, OUTPUT);
  pinMode(SELECT_PIN_0, OUTPUT);
  pinMode(SELECT_PIN_1, OUTPUT);
  pinMode(SELECT_PIN_2, OUTPUT);
  Serial.begin(115200);
  while(!Serial){;}
  Serial.println("Set up serial comms!");
  SPI.begin();
  SPI.beginTransaction(SPISettings(140000, MSBFIRST, SPI_MODE3));
  //Strobe pin to wake it up
  digitalWrite(MASTER_PIN, LOW);
  delay(1);
  digitalWrite(MASTER_PIN, HIGH);
  delay(1);
  digitalWrite(MASTER_PIN, LOW);
  delay(1);
  channel_selection(channel_value);
}

/*
 * Read from sensors and return the binary value which will be stored in read_adc_value.
 */
unsigned int read_atd(byte channel) {
  
  digitalWrite(MASTER_PIN, HIGH);
  
  unsigned int ATDVal;
  if (channel == 0) {
    SPI.transfer(0x06);
    ATDVal = SPI.transfer16(0x0000);
  } else if (channel == 1) {
    SPI.transfer(0x06);
    ATDVal = SPI.transfer16(0x4000);
  } else if (channel == 2) {
    SPI.transfer(0x06);
    ATDVal = SPI.transfer16(0x8000);
  } else if (channel == 3) {
    SPI.transfer(0x06);
    ATDVal = SPI.transfer16(0xC000);
  } else if (channel == 4) {
    SPI.transfer(0x07);
    ATDVal = SPI.transfer16(0x0000);
  } else if (channel == 5) {
    SPI.transfer(0x07);
    ATDVal = SPI.transfer16(0x4000);
  } else if (channel == 6) {
    SPI.transfer(0x07);
    ATDVal = SPI.transfer16(0x8000);
  } else if (channel == 7) {
    SPI.transfer(0x07);
    ATDVal = SPI.transfer16(0xC000);
  }
  
  ATDVal &= ATDVal&(0b0000111111111111);
  digitalWrite(MASTER_PIN, LOW);
  return ATDVal;
}

/*
 * Conversion of binary to decimal.
 */
double base_conversion(unsigned int input) {
  double result = (double)input * 5 / 4095;
  return result;
}

/*
 * Printing the output matrix, should be an 8x8 matrix.
 */
void output_packet() {

  Serial.print("[");
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 8; j++) {
      Serial.print(voltage[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.print("]");
  /*
  String fin = "[";
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      
      if (voltage[i][j] == NULL) {
        voltage[i][j] = 0;
      }
      
      fin = fin + voltage[i][j] + ', 
  */
}

void channel_selection(int channel) {

  if (channel == 0) {
    digitalWrite(SELECT_PIN_0, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_1, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_2, LOW);
    delay(1);
  } else if (channel == 1) {
    digitalWrite(SELECT_PIN_0, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_1, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_2, LOW);
    delay(1);    
  }/* else if (channel == 2) {
    digitalWrite(SELECT_PIN_0, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_1, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_2, LOW);
    delay(1);    
  } else if (channel == 3) {
    digitalWrite(SELECT_PIN_0, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_1, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_2, LOW);
    delay(1);    
  } else if (channel == 4) {
    digitalWrite(SELECT_PIN_0, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_1, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_2, HIGH);
    delay(1);    
  } else if (channel == 5) {
    digitalWrite(SELECT_PIN_0, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_1, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_2, HIGH);
    delay(1);    
  } else if (channel == 6) {
    digitalWrite(SELECT_PIN_0, LOW);
    delay(1);
    digitalWrite(SELECT_PIN_1, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_2, HIGH); 
    delay(1);   
  } else if (channel == 7) {
    digitalWrite(SELECT_PIN_0, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_1, HIGH);
    delay(1);
    digitalWrite(SELECT_PIN_2, HIGH);
    delay(1);    
  }*/
}

void loop() {
  // put your main code here, to run repeatedly:
  channel_selection(chip_value);
  Serial.println("\n\nSending value\n");
  
  read_atd_value = read_atd(channel_value);
  converted_value = base_conversion(read_atd_value);
  voltage[chip_value][channel_value] = converted_value;
  output_packet();

  if (channel_value < 8) {
    channel_value++;
  } else {
    channel_value = 0;
    if (chip_value < 2) {
      chip_value++;
    } else {
      chip_value = 0;
    }
  }
  
}
