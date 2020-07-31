#ifndef PEEL_MEMORY_NODE_H
#define PEEL_MEMORY_NODE_H



#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxNode.h>

#include <maya/MFnNumericAttribute.h>

#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MTransformationMatrix.h>

class Memory : public MPxNode
{
public:
                                Memory();
        virtual                ~Memory();

        virtual MStatus         compute( const MPlug& plug, MDataBlock& data );

        static  void*           creator();
        static  MStatus         initialize();

        static  MObject         inTranslation;         
        static  MObject         inRotation;
	static  MObject         inX;
	static  MObject         inY;
        static  MObject         outCameraTranslation;
        static  MObject         outCameraRotation;   
        static  MObject         outParentTranslation;
        static  MObject         outParentRotation;   
        static  MTypeId         id;

	float lastX;
	float lastY;

	MTransformationMatrix  transform;
};





#endif // PEEL_MEMORY_NODE_H


