#ifndef __MESSAGE__
#define __MESSAGE__

struct message{
    uint8_t type;
    uint32_t size;
    void *data;
};

#endif