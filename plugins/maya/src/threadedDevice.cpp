#include "threadedDevice.h"
#include "udpSocket.h"
#include "item.h"

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MString.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MThreadAsync.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>


#include <iostream>
#include <vector>
#include <string>
//#include <pair>

/*
Usage:
import maya.cmds as m
m.loadPlugin('C:/cpp/git/github/mayaMocap/plugin/build_vs2015/x64/Debug/build_vs2015.mll')
x = m.createNode('peelRealtimeMocap')
loc = m.spaceLocator()
m.connectAttr( x + ".mocap[0].outputTranslate", loc[0] + ".t")
m.connectAttr( x + ".mocap[0].outputRotate", loc[0] + ".r")
m.setAttr( x+ ".live", 1)
*/


#ifdef _WIN32

#include "pipeDevice.h"
#define PIPENAME _T("\\\\.\\pipe\\vrmocap")
#define _CRT_SECURE_NO_WARNINGS
#define SLEEP Sleep

#include <Windows.h>
#include <tchar.h>

#else

#include <unistd.h>
#define SLEEP usleep 

#endif

#define DEFAULT_PORT    9119
#define DEFAULT_CMDPORT 9120


MObject ThreadedDevice::fileName;
MObject ThreadedDevice::save;

MObject ThreadedDevice::mocap;
MObject ThreadedDevice::scale;
MObject ThreadedDevice::outputName;
MObject ThreadedDevice::outputTranslate;
MObject ThreadedDevice::outputRotate;

MObject ThreadedDevice::info;

MObject ThreadedDevice::port;
MObject ThreadedDevice::mode;

MTypeId ThreadedDevice::id(0x001126D1);

#define MCONT(s, msg) if( s != MS::kSuccess) { s.perror(msg); continue; }
#define MCHECKERROR(x, msg) { if(x!=MS::kSuccess) { x.perror(msg); return x;} }

#define BUFSIZE 9200




void ThreadedDevice::postConstructor()
{
	fp = NULL;
	iPort = -1;
	mMode = -1;
#ifdef _WIN32
	hPipe = INVALID_HANDLE_VALUE;
#endif

    MObjectArray oArray;
    oArray.append( ThreadedDevice::mocap );
    setRefreshOutputAttributes(oArray);
    createMemoryPools(1, BUFSIZE, 1);
}

	ThreadedDevice::~ThreadedDevice()
	{
		destroyMemoryPools();
	}


/*
 *  Device Thread - gets the data from the socket and saves it in the maya storage buffer
 */

void ThreadedDevice::sendData(const char *message, size_t msglen, const char *data, size_t datalen)
{
    // Send the message length and data lengths as two size_t's, then send the message and the data

    MStatus status;
    if(message != NULL && msglen == 0)
    {
        // c string was passed without length
         msglen = strlen(message);
    }

    if ( datalen + msglen + 2  >= BUFSIZE )
    {
        // Overflow
        return;
    }

	if (mSave)
	{
		if (fp == NULL)
		{
			fp = fopen(mFileName.asChar(), "wb");
		}

		if (fp != NULL)
		{
			size_t header[3];
			header[0] = 0x2337;
			header[1] = msglen;
			header[2] = datalen;
			fwrite(&header, sizeof(size_t), 3, fp);
			fwrite(message, msglen, 1, fp);
			fwrite(data, datalen, 1, fp);
		}
	}
	else
	{
		if (fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
	}

    MCharBuffer buffer;
    status = acquireDataStorage(buffer);
    if( status != MS::kSuccess )
    {
#ifdef _DEBUG
        printf("X");
        fflush(stdout);
#endif
        return;
    }

    // Position of data blocks
    size_t *header    = (size_t*) buffer.ptr();
    char   *msgblock  = buffer.ptr() + sizeof(size_t)*2;
    char   *datablock = msgblock + msglen;

    //printf("Sizes: %d %d\n", msglen, datalen);

    // thread safe copy data
    beginThreadLoop();
    {
        header[0] = msglen;
        header[1] = datalen;
        if(msglen > 0) memcpy(msgblock,  message, msglen);
        if(datalen > 0 ) memcpy(datablock, data, datalen);
        pushThreadData(buffer);
    }
    endThreadLoop();
}




void ThreadedDevice::threadHandler()
{
	// Called by maya as the entry point to the device thread

    MStatus status;
    setDone(false);

    printf("Threaded Device\n");

    int i;
    
    char receiveBuffer[BUFSIZE];

	sendData("Init");

    printf("Starting thread\n");
    
	char buffer[BUFSIZE];


    while(!isDone())
    {
        if(!isLive()) continue;


        if (!this->isConnected())
        {
			
			// Connect should send a status message
            this->connect();

            SLEEP(500);

			continue;
        }

		
           
	    while(!isDone() && isLive()  )
		{
			// receive data should send a status message if return <= 0
			size_t sz = this->receiveData(buffer, BUFSIZE);
			if (sz == -1) break;
			if (sz == 0) {
                SLEEP(500);
				continue;
			}
			if(sz > 0) sendData( "Receiving", 0, buffer, sz);
        }
    }

	disconnect();
	
	sendData("Stopped");

    printf("Thread Finished\n");


    setDone(true);
}

void ThreadedDevice::threadShutdownHandler()
{
    setDone(true);
}




bool ThreadedDevice::connect()
{

	if ( mMode == MODE_UDP)
	{
		if (socket.isConnected()) return true;

		if (iPort < 1024)
		{
			fprintf(stderr, "Invalid port: %d\n", iPort);
			sendData("Invalid Port");
			return false;
		}

		if (!socket.bind(iPort))
		{
			fprintf(stderr, "Cannot connect\n");
			sendData("Cannot connect");
			return false;
		}

		return true;
	}

#ifdef _WIN32
	if ( mMode == MODE_PIPE)
	{
		if (hPipe != INVALID_HANDLE_VALUE) return true;

		hPipe = CreateNamedPipe(PIPENAME, PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 0, 1024, 50, NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			TCHAR buf[1024];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, buf, 1024 * sizeof(TCHAR), 0);
			sendData(buf);
			printf("Could not create pipe: %s", buf);
			return false;
		}
		return true;
	}


#endif

	return false;
}



bool ThreadedDevice::isConnected()
{
	if ( mMode == MODE_UDP)
	{
		return socket.isConnected();
	}

#ifdef _WIN32
	if ( mMode == MODE_PIPE)
	{
		return hPipe != INVALID_HANDLE_VALUE;
	}
#endif
	return false;
}

bool ThreadedDevice::disconnect()
{
	if (mMode == MODE_UDP)
	{
		socket.close();
		return true;
	}

#ifdef _WIN32
	if (mMode == MODE_PIPE)
	{
		if (hPipe != INVALID_HANDLE_VALUE) CloseHandle(hPipe);
		hPipe = INVALID_HANDLE_VALUE;
		return true;
	}
#endif

	return false;

	
}

size_t ThreadedDevice::receiveData(char *data, size_t buflen)
{
	if (mMode == MODE_UDP)
	{
		int ret = socket.receive();  // get the data, or timeout
									 //int ret = socket.stub();
		if (ret == -1) return -1;  // error
		if (ret == 0) return 0; // timeout

		char buf[128];
		//std::cout << "Listening on port: " << iPort;
		if (socket.buflen > buflen)
		{
			fprintf(stderr, "Buffer overflow\n");
			return -1;
		}
		memcpy(data, socket.readBuffer, socket.buflen);

		return socket.buflen;
	}

#ifdef _WIN32
	if (mMode == MODE_PIPE)
	{
		// Should send a message if return <= 0

		DWORD len = buflen;
		BOOL ret = ReadFile(hPipe, data, 1024, &len, NULL);
		if (ret) return len;

		DWORD err = GetLastError();

		if (err == ERROR_PIPE_LISTENING)
		{
			// Other end has not opened
			sendData("Listening");
			return 0;
		}

		if (err == ERROR_BROKEN_PIPE)
		{
			CloseHandle(hPipe);
			hPipe = INVALID_HANDLE_VALUE;
			sendData("No Connection");
			return 0;
		}

		TCHAR buf[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buf, 1024 * sizeof(TCHAR), 0);
		printf("%s", buf);

		sendData(buf);

		return -1;
	}

#endif
	return -1;
}




MStatus ThreadedDevice::initialize()
{
        MStatus status;
        MFnNumericAttribute   numAttr;
        MFnCompoundAttribute  cAttr;
        MFnTypedAttribute     tAttr;
        MFnMessageAttribute   mAttr;
		MFnEnumAttribute      eAttr;



		// port
		port = numAttr.create("port", "p", MFnNumericData::kInt, DEFAULT_PORT, &status);
		status = addAttribute(port);

		attributeAffects(port, mocap);

		// Mode 
		mode = eAttr.create("mode", "m", 0, &status);
		eAttr.addField("UDP", MODE_UDP);
#ifdef _WIN32
		eAttr.addField("PIPE", MODE_PIPE);
#endif

		MCHECKERROR(status, "creating mode attribute");
		status = addAttribute(mode);
		MCHECKERROR(status, "adding mode attribute");

        // Info 
        info = tAttr.create("info", "ifo", MFnData::kString, MObject::kNullObj, &status);
        MCHECKERROR(status, "creating info");
        tAttr.setStorable(false);
        //tAttr.setWritable(false);
        //tAttr.setReadable(true);
        tAttr.setKeyable(false);
        //tAttr.setConnectable(false);
        status = addAttribute(info);
        MCHECKERROR(status, "add info");

		scale = numAttr.create("scale", "sc", MFnNumericData::kFloat, 1.0, &status);
		MCHECKERROR(status, "creating scale");
		status = addAttribute(scale);
		MCHECKERROR(status, "adding scale");

		// save file name
		fileName = tAttr.create("fileName", "fn", MFnData::kString, MObject::kNullObj, &status);
		MCHECKERROR(status, "creating filename");
		status = addAttribute(fileName);
		MCHECKERROR(status, "adding filename");

		// save data (bool)
		save = numAttr.create("save", "sv", MFnNumericData::kBoolean, 0, &status);
		MCHECKERROR(status, "creating save");
		status = addAttribute(save);
		MCHECKERROR(status, "adding creating save");

        // Name 
        outputName = tAttr.create("name", "n", MFnData::kString, MObject::kNullObj, &status);
		MCHECKERROR(status, "creating name");
        status = addAttribute(outputName);
		MCHECKERROR(status, "adding name");

        // Translate 
        outputTranslate  = numAttr.create("outputTranslate", "ot", MFnNumericData::k3Float, 0.0, &status);
		MCHECKERROR(status, "creating output translate");
        numAttr.setWritable(false);
        status = addAttribute(outputTranslate);
		MCHECKERROR(status, "adding output translate");

        // Rotate 
        outputRotate  = numAttr.create("outputRotate", "orot", MFnNumericData::k3Float, 0.0, &status);
		MCHECKERROR(status, "creating output rotate");
        numAttr.setWritable(false);
        status = addAttribute(outputRotate);
		MCHECKERROR(status, "adding output rotate");

        // Parent "Mocap" attribute (array)
        mocap = cAttr.create("mocap", "mc", &status);
        cAttr.setArray(true);
        status = addAttribute(mocap);
        status = cAttr.addChild(outputName);
        status = cAttr.addChild( outputTranslate );
        status = cAttr.addChild( outputRotate );
        
        attributeAffects( live, mocap);
        attributeAffects( frameRate, mocap);

        attributeAffects( mocap, outputTranslate );
        attributeAffects( mocap, outputRotate );

		attributeAffects(save, outputTranslate);
		attributeAffects(fileName, outputTranslate);


        return MS::kSuccess;
}



MStatus ThreadedDevice::compute( const MPlug &plug, MDataBlock& block)
{
    MStatus status;

	MObject thisNode = thisMObject();

#ifdef _DEBUG
    printf("Compute: %s\n", plug.name().asChar() );
#endif

	// Lock info
    MPlug pInfo( thisMObject(), info);
    pInfo.setLocked(true);

	// Port 
	MDataHandle hPort = block.inputValue(port, &status);
	MCHECKERROR(status, "getting port data");
	iPort = hPort.asInt();

	// mode
	MDataHandle hMode = block.inputValue(mode, &status);
	MCHECKERROR(status, "getting mode data");
	mMode = hMode.asInt();

	// save (bool)
	MDataHandle hSave = block.inputValue(save, &status);
	MCHECKERROR(status, "getting save plug data");
	if (status && hSave.asBool())
	{
		MDataHandle hFileName = block.inputValue(fileName);
		mFileName = hFileName.asString();
		hFileName.setClean();
		
		mSave = true;
	}
	else
	{
		mSave = false;
	}

	// scale (float)
	MDataHandle hScale = block.inputValue(scale, &status);
	MCHECKERROR(status, "saving scale");
	float scaleValue = 1.0f;
	if (status)
	{
		scaleValue = hScale.asFloat();
	}

    // Get an entry
    MCharBuffer buffer;
    status = popThreadData(buffer);
    if(status != MS::kSuccess)
    {
        printf(".");
        fflush(stdout);
        return MS::kSuccess;
    }

    size_t *header = (size_t*)buffer.ptr();
    size_t msglen  = header[0];
    size_t datalen = header[1];

    const char *message = buffer.ptr() + sizeof(size_t) * 2;
    const char *data    = message + msglen;

    if( msglen != 0)
    {
        MDataHandle infoHandle = block.outputValue( info, &status );
		if (status)
		{
			infoHandle.set(MString(message, msglen));
		}
		else
		{
			status.perror("setting info");
		}
    }

    if( datalen != 0)
    {
        // Output
        MArrayDataHandle outHandle = block.outputArrayValue( mocap, &status );
        MCHECKERROR( status, "mocap out handle");

        std::vector<Item*> items;

        parseItems( data, datalen, &items );
        
        MPlug tPlug(thisNode, outputTranslate);
        MPlug rPlug(thisNode, outputRotate);
        MPlug nPlug(thisNode, outputName);

        int arrayIndex;
        int id = 0;

        int last = 0;
        typedef std::vector< std::pair<int, MString> > TNameMap;
        TNameMap nameMap;

        //printf("Len: %d\n", outHandle.elementCount() );

        for(id = 0; id < outHandle.elementCount(); id++)
        {
            // Name
            status = nPlug.selectAncestorLogicalIndex( id, mocap );
            MCHECKERROR(status, "Selecting name attribute");

            MString name;
            nPlug.getValue( name );
            nameMap.push_back( std::make_pair(id, name) );
        }
            
           
        id = 0; 

        for(std::vector<Item*>::iterator i = items.begin(); i != items.end(); i++, id++)
        {

            // Find a matching name
            arrayIndex =  -1;
            for( TNameMap::iterator j = nameMap.begin(); j != nameMap.end(); j++)
            {
                if( (*j).second == (*i)->name )
                { 
                    arrayIndex = (*j).first;
                    break;
                }
            }

            // Find a blank entry
            if(arrayIndex == -1)
            {
                for( TNameMap::iterator j = nameMap.begin(); j != nameMap.end(); j++)
                {
                    if( (*j).second.length() == 0 )
                    { 
                        arrayIndex = (*j).first;
                        break;
                    }
                }
            }

            // Append
            if(arrayIndex == -1)
                arrayIndex = outHandle.elementCount();

            // Translation Handle
            status = tPlug.selectAncestorLogicalIndex( arrayIndex, mocap );
            MCHECKERROR(status, "Selecting translate attribute");

            MDataHandle tHandle = tPlug.constructHandle(block);
            MCHECKERROR(status, "Creating translation handle");

            peel::Marker  *marker  = dynamic_cast<peel::Marker*> (*i);
            peel::Segment *segment = dynamic_cast<peel::Segment*> (*i);
			peel::Joystick *jstick = dynamic_cast<peel::Joystick*> (*i);

            float3& otrans = tHandle.asFloat3();

            // Rotation 
            status = rPlug.selectAncestorLogicalIndex( arrayIndex, mocap );
            MCHECKERROR(status, "Selecting rotate attribute");

            MDataHandle rHandle = rPlug.constructHandle(block);
            MCHECKERROR( status, "adding rotation element" );

            float3& orot = rHandle.asFloat3();

            if(marker) 
            {

                otrans[0] = marker->tx * scaleValue;
                otrans[1] = marker->ty * scaleValue;
                otrans[2] = marker->tz * scaleValue;
                orot[0] = 0.0f;
                orot[1] = 0.0f;
                orot[2] = 0.0f;

  
            }

            if (segment) 
            {
                otrans[0] = segment->tx * scaleValue;
                otrans[1] = segment->ty * scaleValue;
                otrans[2] = segment->tz * scaleValue;

                MQuaternion q( segment->rx, segment->ry, segment->rz, segment->rw); 
                MEulerRotation e = q.asEulerRotation();

                orot[0] = e.x * 57.2958f;
                orot[1] = e.y * 57.2958f;
                orot[2] = e.z * 57.2958f;
            }

			if (jstick)
			{
				otrans[0] = jstick->x * scaleValue;
				otrans[1] = jstick->y * scaleValue;
				otrans[2] = 0.0f;
				orot[0] = 0.0f;
				orot[1] = 0.0f;
				orot[2] = 0.0f;

			}


            tPlug.setValue(tHandle);
            tPlug.destructHandle(tHandle);

            rPlug.setValue(rHandle);
            rPlug.destructHandle(rHandle);

            // Name
            status = nPlug.selectAncestorLogicalIndex( arrayIndex, mocap );
            MCHECKERROR(status, "Selecting name attribute");

            MDataHandle nHandle = nPlug.constructHandle(block);
            MCHECKERROR( status, "adding name element" );
        
            nHandle.set(MString((*i)->name));

            nPlug.setValue(nHandle);
            nPlug.destructHandle(nHandle);

            delete *i;

        }
    }

    //block.setClean( plug );
    block.setClean( mocap );
    block.setClean( outputName );
    block.setClean( outputTranslate );
    block.setClean( outputRotate );
    //block.setClean( output );

    releaseDataStorage(buffer);

    return MS::kSuccess;
}

