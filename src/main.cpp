#include <cstdlib>
#include <cstring>
#include "VBoxWrapperService.h"
#include "ServiceInstaller.h"
#include "Logger.h"

int main(int argc, char *argv[])
{
    if ((argc > 1))
    {
		if(strcmp(argv[1], "-install") == 0)
		{
			ServiceInstaller::install();
		}
		else if(strcmp(argv[1], "-remove") == 0)
		{
			ServiceInstaller::remove();
		}
    }
	else
	{
		if(!VBoxWrapperService::start())
		{
			Logger::log("Main", __func__, InfoLevel::ERR, "cannot start service.");
		}		
	}
	Logger::log("Main", __func__, InfoLevel::ERR, "Why You ARE HERE?!?!?!");

    exit(EXIT_SUCCESS);
}
