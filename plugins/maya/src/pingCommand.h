#ifndef PING_COMMAND_H
#define PING_COMMAND_H


#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>


class PingCommand : public MPxCommand
{
public:
    PingCommand() {}
    virtual ~PingCommand() {}

    MStatus doIt( const MArgList &);
    MStatus redoIt();
    bool isUndoable() const { return false; }
    bool hasSyntax() const { return true; }
    static MSyntax newSyntax();

    static void* creator() { return new PingCommand; }

    static const char * portArg;
    static const char * portArgLong;
    static const char * ipArg;
    static const char * ipArgLong;
    static const char * msgArg;
    static const char * msgArgLong;

private :
    int port;
    MString ip;	
    MString msg;
};


#endif // PING_COMMAND_H
