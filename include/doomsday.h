#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace doomsday{
    using Payload = void (*)(void);
    class Doomsday{
        private:
            // C6 - 16 : LED
            // E0 - 8 : KEY
            // E1 - 9 : TRIGGER
            // D4-7 - 4-7 : DIP SWITCHES
            enum PINS :  uint8_t {
                   STATUS_LED = PIN_C6, // ADD RESISTOR ~= 220-360 OHMs 
                   TRIGGER    = PIN_C4, 
                   KEY        = PIN_C3,
                   DIP_1      = PIN_D4,
                   DIP_2      = PIN_D5,
                   DIP_3      = PIN_C1,
                   DIP_4      = PIN_D7
             };

            const unsigned long GRACETIME   { 10000UL }, // I use a special button, you probably 
                                                         // want less grace time
                                LEDTIMING   { 500UL };
            const int INFO    { 0 },
                      EXEC    { 1 };
    
            unsigned long event             { 0UL },
                          ledLastFlash      { 0UL };
            unsigned int  ledStatus         { HIGH };
            static int    infoPrinted,
                          lastInfoPrinted;

            Payload payload[16][2];

            bool    checkKey(void)                           noexcept;
            bool    checkTrigger(void)                const  noexcept;
            static void printInfo(const char* msg)           noexcept;
            
        public:

            Doomsday(void)                                   noexcept;
            void    checkStatus(void)                        noexcept;
            uint8_t getPayload(void)                   const noexcept;

    };

    int Doomsday::infoPrinted { 0 };
    int Doomsday::lastInfoPrinted { -1 };

    Doomsday::Doomsday(void) noexcept
      : payload{ 
         { [](void){ Doomsday::printInfo("Selected Payload: 0 -> return"); },
           [](void){ Serial.println("Executing Payload: 0 -> return"); 
                     Keyboard.press(KEY_ENTER);
                     Keyboard.release(KEY_ENTER); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 1 -> left click"); },
           [](void){ Serial.println("Executing Payload: 1 -> left click"); 
                     Mouse.set_buttons(1, 0, 0);
                     Mouse.click();
            }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 2 -> doomsday.sh"); },
           [](void){ Serial.println("Executing Payload: 2 -> doomsday.sh"); 
                     Keyboard.println("./doomsday.sh");          
            }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 3"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 4"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 5"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 6"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 7"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 8"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 9"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 10"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 11"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 12"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 13"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 14"); },
           [](void){ Serial.println("Not assigned!"); }
         },
         { [](void){ Doomsday::printInfo("Selected Payload: 15"); },
           [](void){ Serial.println("Not assigned!"); }
         }
      }
    {
          pinMode(STATUS_LED, OUTPUT);
          pinMode(KEY, INPUT_PULLUP);
          pinMode(TRIGGER, INPUT_PULLUP);
          pinMode(DIP_1, INPUT_PULLUP);
          pinMode(DIP_2, INPUT_PULLUP);
          pinMode(DIP_3, INPUT_PULLUP);
          pinMode(DIP_4, INPUT_PULLUP);
          Serial.begin(9600);
    }

    void  Doomsday::printInfo(const char* msg) noexcept{
        if(Doomsday::infoPrinted != Doomsday::lastInfoPrinted){
           Serial.println(msg); 
           Doomsday::lastInfoPrinted = Doomsday::infoPrinted;
        }
    }

    uint8_t Doomsday::getPayload(void) const noexcept{
        return ( digitalRead(DIP_1) == LOW ? 1 : 0 ) +
               ( digitalRead(DIP_2) == LOW ? 2 : 0 ) +
               ( digitalRead(DIP_3) == LOW ? 4 : 0 ) +
               ( digitalRead(DIP_4) == LOW ? 8 : 0 );
    }

    bool Doomsday::checkKey(void) noexcept{
        if(digitalRead(KEY) == LOW ){
           Doomsday::infoPrinted = getPayload();
           payload[infoPrinted][INFO]();
           if( event == 0UL || millis() - event >= GRACETIME ){
                digitalWrite(STATUS_LED, HIGH); 
           } else {
                if( millis() - ledLastFlash >= LEDTIMING){
                    ledStatus = !ledStatus;
                    digitalWrite(STATUS_LED, ledStatus); 
                    ledLastFlash = millis();
                }
           }
           return true;
        } else {   
           event = 0UL;
           ledLastFlash = 0UL;
           digitalWrite(STATUS_LED, LOW);  
           return false;
        }
    }

    bool Doomsday::checkTrigger(void) const noexcept{
        if(digitalRead(TRIGGER) == HIGH )
           return true;
        else    
           return false;
    }

    void Doomsday::checkStatus(void) noexcept{
       if(checkKey()) 
         if(checkTrigger()){
              if( millis() - event >= GRACETIME){
                  payload[getPayload()][EXEC]();
                  Doomsday::lastInfoPrinted = -1;
                  event = millis();
              }
         }
    }
}
