#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <vector>


class Display {

    public:
        void create_display();
        void close_display();
        void refresh_frame(std::vector<std::vector<uint8_t>>&); //takes the 256x240 pixel array.
        

    private:

     

};




#endif
