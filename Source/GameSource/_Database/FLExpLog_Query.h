#include "FLQueryCommand.h"



class FLExpLog_Query : public FLQueryCommand
{
public:
	FLExpLog_Query( CMover & kMover, const char command );
	virtual ~FLExpLog_Query();

	virtual bool 			Execute( CQuery & query );

private:
	CMover					m_kMover;
	char					m_command;
};