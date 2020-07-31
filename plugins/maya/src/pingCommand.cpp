#include "pingCommand.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MArgList.h>

#include "udpSocket.h"

const char * PingCommand::portArg     = "-p";
const char * PingCommand::portArgLong = "-port";
const char * PingCommand::ipArg       = "-ip";
const char * PingCommand::ipArgLong   = "-ipAddress";
const char * PingCommand::msgArg      = "-msg";
const char * PingCommand::msgArgLong  = "-message";

MSyntax PingCommand::newSyntax()
{
	MSyntax s;
	MStatus stat;
	stat = s.addFlag(portArg, portArgLong, MSyntax::kLong);
	if (!stat) { MGlobal::displayError("Error Adding port");  stat.perror("setting port flag"); return s; }

	stat = s.addFlag(ipArg,   ipArgLong,   MSyntax::kString );
	if (!stat) { MGlobal::displayError("Error Adding ip");  stat.perror("setting ip flag"); return s; }

	stat = s.addFlag(msgArg,  msgArgLong,  MSyntax::kString );
	if (!stat) { MGlobal::displayError("Error Adding msg");  stat.perror("setting msg flag"); return s; }

	return s;
}


MStatus PingCommand::doIt( const MArgList &args )
{
    MStatus status;

    MArgDatabase db(syntax(), args);
	
	/*
    unsigned int l = args.length();

    for(unsigned i = 0; i < l; i++)
    {
        MString s;
        args.get(i, s);
        MGlobal::displayInfo(s);
    }
	*/
    port = 0;

    if( db.isFlagSet( portArg   ) )
    {
         db.getFlagArgument( portArg,  0,  port );
    }
	if( db.isFlagSet( ipArg     ) ) 
    {
         db.getFlagArgument( ipArg,    0,  ip );
    }
    if( db.isFlagSet( msgArg    ) ) 
    {
         db.getFlagArgument( msgArg,   0,  msg );
    }
    return this->redoIt();
}


MStatus PingCommand::redoIt()
{
    if( port < 1024)
    {
	MGlobal::displayError("Invalid port");
	return MS::kFailure;
    }
    if( ip.length() == 0 )
    {
	MGlobal::displayError("Invalid ip");
	return MS::kFailure;
    }
    if( msg.length() == 0)
    {
        MGlobal::displayError("No message");
        return MS::kFailure;
    }


    UdpClient client;
    if(!client.create())
    {
	MGlobal::displayError("Could not create socket");
	return MS::kFailure;
    }

    client.sendDatagram( ip.asChar(), port, msg.asChar(), msg.length() );
    
    return MS::kSuccess;
}
