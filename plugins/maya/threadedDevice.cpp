#include "threadedDevice.h"
#include "o3ds/subscriber.h"

// For sleep
#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>

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
#include <queue>
#include <string>
#include <map>
//#include <pair>

/*
Usage:

import maya.cmds as m
m.loadPlugin("O3DSMaya2020Deviced.mll")
x = m.createNode('peelRealtimeMocap')
loc = m.spaceLocator()
m.connectAttr( x + ".mocap[0].outputTranslate", loc[0] + ".t")
m.connectAttr( x + ".mocap[0].outputRotate", loc[0] + ".r")
m.setAttr( x + ".url", "tcp://127.0.0.1:6001", typ="string")
m.setAttr( x + ".subject", "beta", typ="string")

m.setAttr( x+ ".live", 1)

m.getAttr(x + ".mocap[0].outputTranslate")


loadme = None
for f in [ "C:/cpp/git/github/Open3DStream/test_data/beta_anim.fbx",
		   "e:/git/github/Open3DStream/test_data/beta_anim.fbx"] :
	if os.path.isfile(f):
		loadme = f
		break


if not loadme:
    raise RuntimeError("Could not find the test file to load")

m.file(loadme, i=True)

x = m.createNode('peelRealtimeMocap')
loc = m.spaceLocator()
m.connectAttr( x + ".mocap[0].outputTranslate", loc[0] + ".t")
m.connectAttr( x + ".mocap[0].outputRotate", loc[0] + ".r")
m.setAttr( x + ".url", "tcp://127.0.0.1:6001", typ="string")
m.setAttr( x + ".subject", "beta", typ="string")

m.setAttr( x+ ".live", 1)

m.getAttr(x + ".mocap[0].outputTranslate")

// The connection link needs to be delayed to make sure the data has started streaming

def connect():

	for i in range(m.getAttr(x + ".mocap", size=True)):
		node = "%s.mocap[%d]" % (x, i)
		name = m.getAttr(node + ".name" )
		if m.objExists("beta:" + name):
			m.connectAttr("%s.ot" % node , "beta:%s.t" % name)
			m.connectAttr("%s.orot" % node , "beta:%s.r" % name)

import PySide2

PySide2.QtCore.QTimer.singleShot(100, connect)


*/


#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define SLEEP Sleep

#else

#include <unistd.h>
#define SLEEP usleep 

#endif

MObject ThreadedDevice::mocap;
MObject ThreadedDevice::scale;
MObject ThreadedDevice::outputName;
MObject ThreadedDevice::outputTranslate;
MObject ThreadedDevice::preRotation;
MObject ThreadedDevice::outputRotate;

MObject ThreadedDevice::info;

MObject ThreadedDevice::url;
MObject ThreadedDevice::mode;
MObject ThreadedDevice::subject;

MTypeId ThreadedDevice::id(0x001126D1);

#define MCONT(s, msg) if( s != MS::kSuccess) { s.perror(msg); continue; }
#define MCHECKERROR(x, msg) { if(x!=MS::kSuccess) { x.perror(msg); return x;} }

#define BUFSIZE 9200


void ThreadedDevice::postConstructor()
{
	mConnector = nullptr;
	mMode = -1;

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


void ThreadedDevice::passMessage(const char *message, size_t msglen, unsigned char mode)
{
    // pass the message length and data lengths as two size_t's, then send the message and the data

	MStatus status;

	if (message == NULL)
		return;

    if(msglen == 0)
    {
        // c string was passed without length
         msglen = strlen(message);
    }

    if ( msglen + 1  >= BUFSIZE )
		return; // Overflow
	
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
    unsigned char *mode_ptr = (unsigned char*) buffer.ptr();
	size_t *size_ptr = (size_t*)(buffer.ptr() + 1);
	char   *msgblock  = buffer.ptr() + sizeof(size_t) + 1;

    // thread safe copy data
    beginThreadLoop();
    {
        *mode_ptr = mode;
        * size_ptr = msglen;
        if(msglen > 0) memcpy(msgblock,  message, msglen);
        pushThreadData(buffer);
    }
    endThreadLoop();
}


void ThreadedDevice::threadHandler()
{
	// Called by maya as the entry point to the device thread

    MStatus status;
    setDone(false);

	char buffer[BUFSIZE];

	//passMessage("Starting");

    while(!isDone())
    {
        if(!isLive()) continue;

        if (!mConnector)
        {			
			mConnector = new O3DS::Subscriber();
			mConnector->start(mUrl.asChar());
        }
           
	    while(!isDone() && isLive()  )
		{
			size_t sz = mConnector->readMsg(buffer, BUFSIZE);
			if(sz > 0) passMessage(buffer, sz, 1);
        }
    }
	
	if (mConnector)
	{
		delete mConnector;
		mConnector = nullptr;
	}
	
	passMessage("Stopped");

    printf("Thread Finished\n");

    setDone(true);
}

void ThreadedDevice::threadShutdownHandler()
{
    setDone(true);
}


MStatus ThreadedDevice::initialize()
{
        MStatus status;
        MFnNumericAttribute   numAttr;
        MFnCompoundAttribute  cAttr;
        MFnTypedAttribute     tAttr;
        MFnMessageAttribute   mAttr;
		MFnEnumAttribute      eAttr;

		// url
		url = tAttr.create("url", "u", MFnData::kString, MObject::kNullObj, &status);
		status = addAttribute(url);
		MCHECKERROR(status, "creating url attribute");
		attributeAffects(url, mocap);

		// Mode 
		mode = eAttr.create("mode", "m", 0, &status);
		eAttr.addField("Subscribe", MODE_SUBSCRIBE);

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

		scale = numAttr.create("scale", "sc", MFnNumericData::kDouble , 1.0, &status);
		MCHECKERROR(status, "creating scale");
		status = addAttribute(scale);
		MCHECKERROR(status, "adding scale");

		// save file name
		subject = tAttr.create("subject", "subj", MFnData::kString, MObject::kNullObj, &status);
		MCHECKERROR(status, "creating subject");
		status = addAttribute(subject);
		MCHECKERROR(status, "adding subject");

        // Name 
        outputName = tAttr.create("name", "n", MFnData::kString, MObject::kNullObj, &status);
		MCHECKERROR(status, "creating name");
        status = addAttribute(outputName);
		MCHECKERROR(status, "adding name");

        // Translate 
        outputTranslate  = numAttr.create("outputTranslate", "ot", MFnNumericData::k3Double, 0.0, &status);
		MCHECKERROR(status, "creating output translate");
        numAttr.setWritable(false);
        status = addAttribute(outputTranslate);
		MCHECKERROR(status, "adding output translate");

		// Rotate 
		preRotation = numAttr.create("preRotation", "prerot", MFnNumericData::k3Double, 0.0, &status);
		MCHECKERROR(status, "creating pre rotate");
		numAttr.setWritable(true);
		status = addAttribute(preRotation);
		MCHECKERROR(status, "adding output rotate");

        // Rotate 
        outputRotate  = numAttr.create("outputRotate", "orot", MFnNumericData::k3Double, 0.0, &status);
		MCHECKERROR(status, "creating output rotate");
        numAttr.setWritable(false);
        status = addAttribute(outputRotate);
		MCHECKERROR(status, "adding output rotate");

        // Parent "Mocap" attribute (array)
        mocap = cAttr.create("mocap", "mc", &status);
        cAttr.setArray(true);
        status = addAttribute(mocap);
        status = cAttr.addChild(outputName);
        status = cAttr.addChild(outputTranslate);
		status = cAttr.addChild(preRotation);
        status = cAttr.addChild(outputRotate);
        
        attributeAffects(live, mocap);
        attributeAffects(frameRate, mocap);
		attributeAffects(subject, mocap);

        attributeAffects(mocap, outputTranslate);
        attributeAffects(mocap, outputRotate);

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

	// Subject
	MDataHandle hSubject = block.inputValue(subject, &status);
	MCHECKERROR(status, "getting subject data");
	mSubject = hSubject.asString();

	// Port 
	MDataHandle hUrl = block.inputValue(url, &status);
	MCHECKERROR(status, "getting port data");
	mUrl = hUrl.asString();

	// mode
	MDataHandle hMode = block.inputValue(mode, &status);
	MCHECKERROR(status, "getting mode data");
	mMode = hMode.asInt();

	// scale (double)
	MDataHandle hScale = block.inputValue(scale, &status);
	MCHECKERROR(status, "saving scale");
	double scaleValue = 1.0;
	if (status)
	{
		scaleValue = hScale.asDouble();
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

    unsigned char* mode   = (unsigned char*)buffer.ptr();
    size_t*        msglen = (size_t*)(buffer.ptr() + 1);
    const char *message = buffer.ptr() + sizeof(size_t) + 1;
 
    if( *mode == 0)
    {
        MDataHandle infoHandle = block.outputValue( info, &status );
		if (status)
		{
			infoHandle.set(MString(message, (int)*msglen));
		}
		else
		{
			status.perror("setting info");
		}
    }

    if( *mode == 1)
    {
        // Output
        MArrayDataHandle outHandle = block.outputArrayValue( mocap, &status );
        MCHECKERROR( status, "mocap out handle");
		mSubjectList.Parse(message, *msglen);

		O3DS::Subject *subject = mSubjectList.findSubject(mSubject.asChar());

		subject->update();

		if (subject != nullptr)
		{

			MPlug tPlug(thisNode, outputTranslate);
			MPlug rPlug(thisNode, outputRotate);
			MPlug nPlug(thisNode, outputName);

			int arrayIndex;
			unsigned int id = 0;

			int last = 0;

			id = 0;

			for (auto i = subject->mTransforms.begin(); i != subject->mTransforms.end(); i++, id++)
			{
				arrayIndex = -1;

				O3DS::Transform *transform = *i;
				std::string nodeName = (*i)->mName;

				// Find the channel with a name that matches
				std::vector<std::string>::iterator channelId = std::find(mNameMap.begin(), mNameMap.end(), nodeName);
				if (channelId != mNameMap.end())
				{
					arrayIndex = (int)std::distance(mNameMap.begin(), channelId);
				}
				else
				{
					arrayIndex = (int)mNameMap.size();
					mNameMap.push_back(nodeName);
					status = nPlug.selectAncestorLogicalIndex(arrayIndex, mocap);
					nPlug.setValue(nodeName.c_str());
				}

				// Translation Handle
				status = tPlug.selectAncestorLogicalIndex(arrayIndex, mocap);
				MCHECKERROR(status, "Selecting translate attribute");

				MDataHandle tHandle = tPlug.constructHandle(block);
				MCHECKERROR(status, "Creating translation handle");

				double3& otrans = tHandle.asDouble3();

				// Rotation 
				status = rPlug.selectAncestorLogicalIndex(arrayIndex, mocap);
				MCHECKERROR(status, "Selecting rotate attribute");

				MDataHandle rHandle = rPlug.constructHandle(block);
				MCHECKERROR(status, "adding rotation element");

				double3& orot = rHandle.asDouble3();

				// Translation
				//auto tr = transform->mMatrix.GetTranslation();
				auto tr = transform->translation.value;
				otrans[0] = tr.v[0] * scaleValue;
				otrans[1] = tr.v[1] * scaleValue;
				otrans[2] = tr.v[2] * scaleValue;

				// Rotation
				auto ro = transform->rotation.value;
				//auto ro = transform->mMatrix.GetQuaternion();
				MQuaternion q(ro.v[0], ro.v[1], ro.v[2], ro.v[3]);
				MEulerRotation e = q.asEulerRotation();

				orot[0] = e.x * 57.295779513;
				orot[1] = e.y * 57.295779513;
				orot[2] = e.z * 57.295779513;

				tPlug.setValue(tHandle);
				tPlug.destructHandle(tHandle);

				rPlug.setValue(rHandle);
				rPlug.destructHandle(rHandle);

				// Name
				status = nPlug.selectAncestorLogicalIndex(arrayIndex, mocap);
				MCHECKERROR(status, "Selecting name attribute");
			}
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

