#ifndef ITEM_H
#define ITEM_H

/*  
Items are serialized as :

2 byte header - 42, n-items
   1 byte item header - 43
   1 byte name length
   1 byte message length
   name
   message

message is formatted as a series of string encoded floats, seperated by '0'
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#define NAMELEN 255

namespace peel {

// Item struct for passing data from thread

class Item
{
public :
    Item() {}
    virtual ~Item() {}
    Item( const char *name );
    char  name[NAMELEN];

};

class Segment : public Item
{
public:
    Segment() {}

    Segment( const char *name_, float tx_, float ty_, float tz_,
                        float rx_, float ry_, float rz_, float rw_ );
    float tx;
    float ty;
    float tz;
    float rx;
    float ry;
    float rz;
    float rw;
};

class Marker : public Item
{
public:
    Marker() {}

    Marker( const char *name_, float tx_, float ty_, float tz );

    float tx;
    float ty;
    float tz;
};

class Joystick : public Item
{
public:
	Joystick() {}
	Joystick(const char *name_, float x_, float y_);
	float x;
	float y;
};



size_t writeHeader(char* buf, size_t buflen, char id, const char *name, short **datalen);

size_t serializeItems( std::vector<Item*> &items,  char *buffer, size_t buflen);
        
void dumpData( const char *) ;
void dumpItems(std::vector<Item*> &);

size_t parseItems(const char *buffer, size_t len, std::vector<Item*> *items );

}
#endif // ITEM_H
