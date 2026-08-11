#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>


class Display {

    public:
        void create_display();
        void close_display();
        void refresh_frame(uint8_t**); //takes the 256x240 pixel array.
        

    private:
     

};




#endif
