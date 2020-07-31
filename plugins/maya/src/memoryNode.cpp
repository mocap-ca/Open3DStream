#include "memoryNode.h"

#include <maya/MFloatVector.h>

MTypeId     Memory::id( 0x80012 );
MObject     Memory::inTranslation;
MObject     Memory::inRotation;
MObject     Memory::inX;
MObject     Memory::inY;
MObject     Memory::outCameraTranslation;
MObject     Memory::outCameraRotation;
MObject     Memory::outParentTranslation;
MObject     Memory::outParentRotation;

Memory::Memory() : lastX(0.0f), lastY(0.0f) {}
Memory::~Memory() {}

#define MERR(s, msg) if(!s) { s.perror(msg); return s ; }

MStatus Memory::compute( const MPlug& plug, MDataBlock& data )
{

        MStatus status;

	printf("Plug: %s\n", plug.name().asChar() );

	MDataHandle inTranslationHandle = data.inputValue( inTranslation, &status);
	MERR(status, "getting in translation handle");

	MDataHandle inRotationHandle = data.inputValue( inRotation, &status);
	MERR(status, "getting in rotation handle");

	MDataHandle inXHandle = data.inputValue( inX, &status);
	MERR(status, "getting input X");

	MDataHandle inYHandle = data.inputValue( inY, &status);
	MERR(status, "getting input Y");


	MFloatVector &translation = inTranslationHandle.asFloatVector();
	MFloatVector &rotation    = inRotationHandle.asFloatVector();

	float x = inXHandle.asFloat();
	float y = inYHandle.asFloat();

	if( lastX - x + lastY - y  > 3.0 )
	{
		printf("Translation: %f %f %f\n", translation.x, translation.y, translation.z);
		printf("Rotation:    %f %f %f\n", rotation.x,    rotation.y,    rotation.z);
		printf("XY: %f %f\n", x, y);
		lastX = x;
		lastY = y;
	}

	/*
        if( plug == outputCameraTranslation )
        {
                MDataHandle inputData = data.inputValue( inX, &returnStatus );
		if( returnStatus != MS::kSuccess ) 
		{
			returnStatus.perror("getting input");
			return MS::kFailure;
		}

		return MS::kSuccess;

	}*/
	return MS::kUnknownParameter;
}

void * Memory::creator()
{
	return new Memory();
}

#define INPUT( attr ) { attr.setStorable( true ); }
#define OUTPUT( attr) { attr.setWritable( false );  attr.setStorable(false); }
#define ADDATTR( obj, msg ) { stat = addAttribute( obj ); if(!stat) { stat.perror(msg); return stat; } }

MStatus Memory::initialize()
{
	MFnNumericAttribute nAttr;
	MStatus stat;

	// IN X
	inX = nAttr.create( "inputX", "inx", MFnNumericData::kFloat, 0.0);
	INPUT( nAttr )
	ADDATTR( inX, "adding input x");

	// IN Y
	inY = nAttr.create( "inputY", "iny", MFnNumericData::kFloat, 0.0);
	INPUT( nAttr );
	ADDATTR( inY, "adding input y" );
	
	// In Translation
	inTranslation = nAttr.createPoint( "inputTranslation", "it" );
	INPUT( nAttr );
	ADDATTR( inTranslation, "adding input translation" );

	// In Rotation
	inRotation = nAttr.createPoint( "inputRotation", "ir" );
	INPUT( nAttr );
	ADDATTR( inRotation, "adding input rotation" );


	// Camera Translation
	outCameraTranslation = nAttr.createPoint( "outCameraTranslation", "oct");
	OUTPUT( nAttr );
	ADDATTR( outCameraTranslation, "adding out camera translation" );

	// Camera Rotation
	outCameraRotation = nAttr.createPoint( "outCameraRotation", "ocr");
	OUTPUT( nAttr  );
	ADDATTR( outCameraRotation, "adding out camera rotation" );

	// Parent Translation
	outParentTranslation= nAttr.createPoint( "outParentTranslation", "opt");
	OUTPUT( nAttr  );
	ADDATTR( outParentTranslation, "adding out parent translation" );

	// Parent Rotation
	outParentRotation = nAttr.createPoint( "outParentRotation", "opr");
	OUTPUT( nAttr  );
	ADDATTR( outParentRotation, "adding out parent rotation");


	attributeAffects(inX, outCameraTranslation);
	attributeAffects(inX, outCameraRotation);
	attributeAffects(inX, outParentTranslation);
	attributeAffects(inX, outParentRotation);
	attributeAffects(inTranslation, outCameraTranslation);
	attributeAffects(inTranslation, outCameraRotation);
	attributeAffects(inTranslation, outParentTranslation);
	attributeAffects(inTranslation, outParentRotation);
	attributeAffects(inRotation, outCameraTranslation);
	attributeAffects(inRotation, outCameraRotation);
	attributeAffects(inRotation, outParentTranslation);
	attributeAffects(inRotation, outParentRotation);

	return MS::kSuccess;
}

