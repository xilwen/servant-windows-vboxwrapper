#include <iostream>
#include <cstdlib>
#include "Server.h"
#include "Interpreter.h"
#include "VBoxMainController.h"
#include "WindowsRegistry.h"

int main()
{
    auto vbmc = new VBoxMainController;
    auto interpreter = new Interpreter(vbmc);
    auto svr = new Server(interpreter);
    svr->waitForConnect();
    svr->handShakeWithClient();
    svr->runInterpreterDaemon();
    delete svr;
    delete interpreter;
    delete vbmc;

    exit(EXIT_SUCCESS);
}
