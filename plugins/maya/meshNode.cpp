#include "meshNode.h"
//#include "mesh.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>


#include <tchar.h>


#define PIPENAME _T("\\\\.\\pipe\\vrmocapmesh")
#define _CRT_SECURE_NO_WARNINGS

MObject MocapMesh::in_inMesh;
MObject MocapMesh::out_message;

MocapMesh::MocapMesh()
	: value(0.0) {}

MocapMesh::~MocapMesh()
{
	if (hPipe != INVALID_HANDLE_VALUE) CloseHandle(hPipe);
}

void* MocapMesh::creator() { return new MocapMesh(); }
MTypeId MocapMesh::id(0x001126D3);

MStatus MocapMesh::initialize()
{
	MStatus stat;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	in_inMesh  = tAttr.create("inMesh", "im",MFnData::kMesh);
	tAttr.setKeyable(true);
	tAttr.setStorable(true);
	tAttr.setReadable(true);
	tAttr.setWritable(true);
	stat = addAttribute(in_inMesh);
	if(!stat) { stat.perror("adding inMesh"); return stat; }

	out_message  = nAttr.create("test", "test", MFnNumericData::kDouble, 0.0);
	nAttr.setKeyable(false);
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setReadable(true);
	stat = addAttribute(out_message);
	if(!stat) { stat.perror("adding test"); return stat; }

	stat = attributeAffects(in_inMesh,out_message);
	if(!stat) { stat.perror("attributeAffects"); return stat; }
	return MS::kSuccess;
}


void MocapMesh::postConstructor()
{
	hPipe = CreateNamedPipe(PIPENAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 2, 0, 0, 0, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
		printf("Could not create pipe\n");
}

MStatus MocapMesh::compute( const MPlug &plug, MDataBlock &data )
{
	MStatus stat;
	MDataHandle indata_inMesh = data.inputValue(in_inMesh, &stat);
	if(stat != MS::kSuccess) return stat;

	MDataHandle hMessage = data.outputValue( out_message, &stat );
	if(stat != MS::kSuccess) return stat;

	double &v = hMessage.asDouble();
	value += 0.001;
	v = value;
	

	MObject oMesh = indata_inMesh.asMesh();
	MFnMesh fnMesh( oMesh );

	MPointArray points;
	fnMesh.getPoints( points );

	MIntArray vertCount, vertList;
	fnMesh.getTriangles( vertCount, vertList );



	if (hPipe)
	{
		size_t pointLen = points.length() * sizeof(float) * 3;
		size_t vertLen = vertList.length() * sizeof(int);
		size_t headLen = 3 * sizeof(int);
		size_t packetLen = pointLen + vertLen + headLen;
			

		char *data = (char*) malloc(packetLen);

		unsigned int * header = (unsigned int*)data;

		header[0] = 0x3223;
		header[1] = pointLen;
		header[2] = vertLen;
		
		float *fptr = (float*) (data + headLen);

		for (int i = 0; i < points.length(); i++)
		{
			*fptr = (float)points[i].x;  fptr++;
			*fptr = (float)points[i].y;  fptr++;
			*fptr = (float)points[i].z;  fptr++;
		}

		int *iptr = (int*) ( data + headLen + pointLen );
		for (int i = 0; i < vertList.length(); i++)
		{
			*iptr = vertList[i];  iptr++;
		}

		DWORD written;
		if (!WriteFile(hPipe, data, packetLen, &written, NULL))
		{
			TCHAR buf[1024];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, buf, 1024 * sizeof(TCHAR), 0);
			printf("Error Writing to pipe: %s", buf);
		}

		free(data);

		


	}

	



	

	MDataHandle outdata_message = data.outputValue(out_message, &stat);
	if(stat != MS::kSuccess) return stat;

	return MS::kSuccess;
}