#include "listenCommand.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>

ListenThread listener;

MSyntax ListenCommand::newSyntax() 
{
    MSyntax s;
    s.addFlag(portFlag,  portFlagLong , MSyntax::kUnsigned );
    s.addFlag(closeFlag, closeFlagLong );
    s.addFlag(queryFlag, queryFlagLong );
	s.addFlag(moduleFlag, moduleFlagLong, MSyntax::kString);
    return s;

}


MStatus ListenCommand::doIt( const MArgList &args )
{
    MStatus status;

    MArgDatabase db(syntax(), args);

    port   = 0;
    create = false;
    close  = false;
    query  = false;
	module = "";

    if (db.isFlagSet(queryFlag) )
    {
        query = true;
    }

	if (db.isFlagSet(portFlag))
	{
		status = db.getFlagArgument(portFlag, 0, port);
		if (status) { create = true; }
        else { status.perror("getting port"); }
	}
	
	if (db.isFlagSet(moduleFlag))
	{
		status = db.getFlagArgument(moduleFlag, 0, module);
	}

    if( db.isFlagSet(closeFlag) )
    {
        close = true;
    }

    return this->redoIt();
}


MStatus ListenCommand::redoIt()
{
    if( query ) 
    {
        if(listener.running)
        {  
            setResult(listener.mPort);
        }
        else
        {
            setResult(-1);
        }

    }
    if( create && close )
    {
        MGlobal::displayError("Create or close - not both");
        return MS::kFailure;
    }


    if(create && port > 1024)
    {
		if (module == "")
		{
			MGlobal::displayError("No module specified");
			return MS::kFailure;
		}
        if( listener.start(port, module) == 0 )
        {
            MGlobal::displayInfo("started");
        }
        else
        {
            MGlobal::displayError("could not start");
        }
    }

    if(close)
    {
        MGlobal::displayInfo("stopping");
        listener.stop();
    }
    return MS::kSuccess;
}
