#include "pingCommand.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MArgList.h>

#include "o3ds/pair.h"

const char * PingCommand::urlArg     = "-u";
const char * PingCommand::urlArgLong = "-url";
const char * PingCommand::msgArg      = "-msg";
const char * PingCommand::msgArgLong  = "-message";

MSyntax PingCommand::newSyntax()
{
	MSyntax s;
	MStatus stat;
	stat = s.addFlag(urlArg, urlArgLong, MSyntax::kLong);
	if (!stat) { MGlobal::displayError("Error adding url flag");  stat.perror("adding url flag"); return s; }

	stat = s.addFlag(msgArg,  msgArgLong,  MSyntax::kString );
	if (!stat) { MGlobal::displayError("Error adding msg flag");  stat.perror("adding msg flag"); return s; }

	return s;
}


MStatus PingCommand::doIt( const MArgList &args )
{
    MStatus status;

    MArgDatabase db(syntax(), args);
	
	if(db.isFlagSet( urlArg)) 
    {
		db.getFlagArgument(urlArg, 0, url);
    }
    if(db.isFlagSet(msgArg)) 
    {
         db.getFlagArgument( msgArg, 0, msg );
    }
    return this->redoIt();
}


MStatus PingCommand::redoIt()
{
	if( url.length() == 0 )
	{
		MGlobal::displayError("Invalid url");
		return MS::kFailure;
	}
	if( msg.length() == 0)
	{
		MGlobal::displayError("No message");
		return MS::kFailure;
	}


	O3DS::ClientPair client;
	if(!client.start(url.asChar()))
	{
		MGlobal::displayError("Could not create socket");
		return MS::kFailure;
	}

	client.write(msg.asChar(), msg.length() );
    
	return MS::kSuccess;
}
