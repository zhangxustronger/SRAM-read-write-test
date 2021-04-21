
#include <SPI.h>
#include <SD.h>

#define OE_LOW()    digitalWrite(39, 0)   //Sets OE LOW
#define CE_LOW()    digitalWrite(40, 0)   //Sets CE LOW
#define WE_LOW()    digitalWrite(41, 0)   //Sets WE LOW
#define WE_HIGH()   digitalWrite(41, 1)   //Sets WE HIGH

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

   if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  
   pinMode(39,OUTPUT);
   pinMode(40,OUTPUT);
   pinMode(41,OUTPUT);

   DDRA = 0b11111111; //Pins 22-29 Set as Output (Lower Byte of Address)
   DDRC = 0b01111111; //Pins 37-31 Set as Output (Upper Byte of Address)

}

void loop() {
    
    uint16_t address = 0;  
    //uint8_t write_data; 
           
    long savedNum = 0;
    long numberOne = 0;
       
    myFile = SD.open("#26", FILE_READ);
                   
//    for (address = 0; address < 0x8000; ++address)  //Start at 0, sequence through all (2^15) - 1 addresses
//    {  
//      //myFile.read(write_data,3);
//      write_data = myFile.parseInt();
//      Serial.println(myFile.read());
//      //Serial.println(myFile.read[0]+myFile.read[1]*10+myFile.read[2]*100);
//      
//    }

    
    while (myFile.available()) { // Something left to read
     for (address = 0; address < 0x8000;)  //Start at 0, sequence through all (2^15) - 1 addresses
       { 
        char c = myFile.read(); // Get the next character
        if (isdigit(c)) { // It's a number 0-9
            savedNum *= 10; // Make room by decimal shifting to the left
            savedNum += (c - '0'); // Convert the character to a number and add it to the total
        } else if (c == '\n') { // End of line
            //Serial.print(F("The number is: "));
            write_data(address, savedNum);
            //Serial.println(read_data(address)); // Show us the number
            numberOne = savedNum;  // Store it somewhere else - enhance this for multiple numbers.
            savedNum = 0;  // Reset to 0 ready to read the next line
             ++address;
        }
    }
    }


     myFile.close();
    Serial.println("write done"); // Show us the number
    Serial.println("please pull up WE!"); // Show us the number
    Serial.println("please carry out SD and mcu!"); // Show us the number
    delay(500);   //Delay 500 milliseconds to ensure all data is properly displayed from Serial Monitor
    exit(0);      //Exit program
    
   
}

void write_data(uint16_t address, uint8_t data)
{
  /*
   * Write operation uses a WE (Write Enabled Controlled) Write Cycle.
   */
  OE_LOW();             //OE is continuously LOW
  CE_LOW();             //CE is continuously LOW
  WE_HIGH();            //WE starts off HIGH
  delay(1);             //Delay just to ensure signals stay HIGH/LOW long enough
  
  set_addr(address);    //Address applied first
  WE_LOW();             //WE goes from HIGH to LOW
  data_op('w', data);   //Data applied to data bus
  WE_HIGH();            //WE goes from LOW to HIGH

}

/*
 * Read Data Function
 * Arguments: Address to be used to obtain data
 */

uint8_t read_data(uint16_t address)
{

  /*
   * Read Operation uses "Read Cycle 1", see PDF documentation linked
   */
  WE_HIGH();     //WE set to HIGH at all times for Type 1 Read
  OE_LOW();      //OE set to LOW at all times for Type 1 Read
  CE_LOW();      //CE set to LOW at all times for Type 1 Read
  
  set_addr(address);        //Address applied first
  return (data_op('r', NULL));  //Read data operation is committed, data obtained is returned
  
}

/*
 * Set Address Function, Outputs the address through two pin registers
 * Arguments: Address to be outputted
 */

void set_addr (uint16_t address)
{

  PORTA = address & 0xff;         //Takes the first half of the address and sets it to PORTA (Lower Address Byte)
  PORTC = (address >> 8) & 0xff;  //Takes the second half of the address and sets it to PORTC (Upper Address Byte)
  
}

/*
 * Data Operation Function, controls the data IO from the Arduino
 * Arguments: Read/Write option and data to be written
 */

uint8_t data_op (char rw, uint8_t data)
{
  if(rw == 'w') 
  {
    /*
     * If RW option is set to char 'w' for WRITE
     */
    DDRL = 0xff;   //Set Data Direction on L register pins to OUTPUT
    PORTL = data;  //Output Data through L pin data register
    return PORTL;  //Return the data outputted as confirmation
  }
  else if (rw == 'r')
  {
   /*
    * If RW option is set to char 'r' for READ, read data
    */
    PORTL = 0x00;  //Erase any data still being held from previous write operation in L pin data register
    DDRL = 0x00;   //Set L register pins to INPUT
    return PINL;   //Return values read in the pin register
  }

}
