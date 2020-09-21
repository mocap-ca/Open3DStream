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

    static const char * urlArg;
    static const char * urlArgLong;
    static const char * msgArg;
    static const char * msgArgLong;

private :
    MString url;
    MString msg;
};


#endif // PING_COMMAND_H
