#include "item.h"
#include <string>
#include <sstream>

#define _CRT_SECURE_NO_WARNINGS


peel::Item::Item( const char * name_)
{
    if(strlen(name_) >= NAMELEN)
        strcpy(name, "OVERFLOW-ERROR");
    else
        strcpy(name, name_);
}

peel::Segment::Segment( const char *name_, float tx_, float ty_, float tz_,
                        float rx_, float ry_, float rz_, float rw_ )
    : Item(name_)
{
        tx = tx_;
        ty = ty_;
        tz = tz_;
        rx = rx_;
        ry = ry_;
        rz = rz_;
        rw = rw_;
}

peel::Marker::Marker( const char *name_, float tx_, float ty_, float tz_ )
    : Item(name_)
{
        tx = tx_;
        ty = ty_;
        tz = tz_;
}

peel::Joystick::Joystick(const char *name_, float x_, float y_)
	: Item(name_)
{
	x = x_;
	y = y_;
}


size_t peel::writeHeader(char* buf, size_t buflen, char id, const char *name, short **datalen)
{
	unsigned char namelen = strlen(name);

	if (4 + namelen >= buflen) return -1;

	buf[0] = id;
	buf[1] = namelen;

	*datalen = (short*)(buf+2);
	**datalen = 0;

	memcpy(buf+4, name, namelen);
	return 4 + namelen;

}

size_t peel::serializeItems( std::vector<peel::Item*> &items,  char *buffer, size_t buflen)
{
	short *datalen = NULL;

#ifdef _DEBUG
    memset( buffer, -2, buflen);
#endif
    char *ptr = buffer;

    if(buflen < 2)
        return 0;

    *(ptr++) = 42; 
    *(ptr++) = (unsigned char) items.size(); 

    int ret;

    for( size_t i = 0; i < items.size(); i++)
    {

        Item *item = items[i];
        Segment  *segment = dynamic_cast<peel::Segment*>( item );
        Marker   *marker  = dynamic_cast<peel::Marker*> (item);
		Joystick *jstick  = dynamic_cast<peel::Joystick*> (item);

        if( segment != nullptr)
        {
			// Write the header
			size_t ret = writeHeader(ptr, buflen - (ptr - buffer), 43, segment->name, &datalen);
			if (ret == -1) return ptr - buffer; // overflow
			ptr += ret;

            float *eachFloat = &segment->tx;

            for( int i =0 ; i < 7; i++)
            {
                if ( ptr - buffer + 10 >= buflen )
                    return ptr-buffer;

                ret = sprintf( ptr, "%f",  *(eachFloat++) );
                ptr += ret + 1;
                *datalen += ret + 1;
            };
        }
        if( marker != nullptr)
        {
			// Write the header
			size_t ret = writeHeader(ptr, buflen - (ptr - buffer), 44, marker->name, &datalen);
			if (ret == -1) return ptr - buffer; // overflow
			ptr += ret;

            float *eachFloat = &marker->tx;

            for( int i =0 ; i < 3; i++)
            {
                if ( ptr - buffer + 10 >= buflen )
                    return ptr-buffer;

                ret = sprintf( ptr, "%f",  *(eachFloat++) );
                ptr += ret + 1;
                *datalen += ret + 1;
            };
        }
		if (jstick != nullptr)
		{
			// Write the header
			
			size_t ret = writeHeader(ptr, buflen - (ptr - buffer), 45, jstick->name, &datalen);
			if (ret == -1) return ptr - buffer; // overflow
			ptr += ret;

			if (ptr - buffer + 20 >= buflen)
				return ptr - buffer;

			ret = sprintf(ptr, "%f", jstick->x);
			ptr += ret + 1;
			*datalen += ret + 1;

			ret = sprintf(ptr, "%f", jstick->y);
			ptr += ret + 1;
			*datalen += ret + 1;
		}

    }

    return ptr-buffer;
}



size_t peel::parseItems(const char *buffer, size_t len, std::vector<peel::Item*> *items )
{

	for (std::vector<peel::Item*>::iterator i = items->begin(); i != items->end(); i++)
		delete (*i);

	items->clear();

    if( len < 3) return 0;

    // The first character should be 42, the next should be the number if entries.
    if(buffer[0] != 42)
    {
        fprintf(stderr,"Invalid packet header:  %02X (%zu) - expected %02X", buffer[0], len, 42);
        return 0;
    }
    unsigned char count = buffer[1];

    if(count == 0) return 0;

    const char *ptr = buffer + 2;



    for (size_t i = 0; i < count && (ptr - buffer) < len; i++)
    {

        char id = *(ptr++);
        if (id != 43 && id != 44 && id != 45)
        {
            fprintf(stderr, "Invalid packet entry %02X, expected %02x or %02x\n", id, 43, 44);
            return 0;
        }

        unsigned char namelen = *(ptr++);
		unsigned short datalen = *(short*)(ptr);  ptr += 2;
        const char*   name = ptr;  ptr += namelen;
        const char*   data = ptr;  ptr += datalen;

        if (namelen > NAMELEN)
        {
           fprintf(stderr, "Name length overflow: %.*s\n", name, namelen);
           continue;
        }
            

        if (ptr - buffer > len)
        {
            fprintf(stderr, "Parser buffer overrun\n");
            return 0;
        }

        if( id == 43 )
        {
            Segment *segment = new Segment();

            // Name
            memcpy(segment->name, name, namelen);
            segment->name[namelen] = 0;

            float   *fptr = &segment->tx;
            size_t   dptr = 0;

            for (int j = 0; j < 7 && data + dptr < buffer + len; j++)
            {
                fptr[j] = atof(data+dptr);
                dptr += strlen(data+dptr)+1;
            }

            items->push_back(segment);

        }

        if( id == 44 )
        {
            Marker *marker = new Marker();

            // Name
            memcpy(marker->name, name, namelen);
            marker->name[namelen] = 0;

            float      *fptr = &marker->tx;
            size_t      dptr = 0;

            for (int j = 0; j < 3 && data + dptr < buffer + len; j++)
            {
                fptr[j] = atof(data + dptr);
                dptr += strlen(data + dptr) + 1;
            }

            items->push_back(marker);

        }

		if (id == 45)
		{
			char tmp[64];
			memcpy(tmp, data, datalen);
			tmp[datalen] = 0;

			float f1 = 0.0f;
			float f2 = 0.0f;
			int ret = sscanf(tmp, "%f %f", &f1, &f2);

			if (ret == 2)
			{
				peel::Joystick *joystick = new peel::Joystick();

				// Name
				memcpy(joystick->name, name, namelen);
				joystick->name[namelen] = 0;

				joystick->x = f1;
				joystick->y = f2;

				items->push_back(joystick);
			}
		}
    }

    return ptr-buffer;
}


void peel::dumpItems(std::vector<peel::Item*> &items)
{
	for (std::vector<peel::Item*>::iterator i = items.begin(); i != items.end(); i++)
	{
		peel::Segment *s = dynamic_cast<peel::Segment*>(*i);
		if (s != nullptr) printf("Segment: %s %f %f %f   %f %f %f %f\n", s->name, s->tx, s->ty, s->tz, s->rx, s->ry, s->rz, s->rw);

		peel::Marker *m = dynamic_cast<peel::Marker*>(*i);
		if (m != nullptr) printf("Marker: %s %f %f %f\n", m->name, m->tx, m->ty, m->tz);

		peel::Joystick *j = dynamic_cast<peel::Joystick*>(*i);
		if (j != nullptr) printf("Joystick: %s %f %f\n", j->name, j->x, j->y);
	}
}


void peel::dumpData( const char *buffer) 
{
    if(buffer[0] != 42 )
    {
        fprintf(stderr, "Invalid packet header\n");
        return;
    }

    unsigned char count = buffer[1];

    printf("Items: %d\n", count);

    const char *ptr = buffer + 2;

    for (unsigned char i = 0; i < count; i++)
    {
        char id = *(ptr++);

        if( id != 43 && id != 44 && id != 45)
        {
            fprintf(stderr, "Invalid Packet entry at byte: %zu\n", ptr-buffer);
            return;
        }

        unsigned char namelen = *(ptr++);
		unsigned short datalen = *(short*)(ptr); ptr += 2;

        const char* name = ptr;  ptr+= namelen;
        const char* data = ptr;  ptr+= datalen;

        printf("name   (%d)  %.*s\n", namelen, namelen, name );
        printf("floats (%d)\n", datalen );
        printf("  %s\n", data);

        for(unsigned char n = 0; n < datalen; n++)
        {
            if(data[n] == 0)
            {
                n++;
                if(n < datalen) printf("  %s\n", data+n);
            }
        }
    }
}

