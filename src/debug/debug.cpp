#include "debug.h"

#ifdef DEBUG_MODE
#define DEBUG(message, flag) \
{ \
        if (flag == FAILED) \
            std::cerr << RED << message << " failed." << RESET << std::endl; \
        else if (flag == SUCCESS) \
		{\
            std::cout << GREEN << message << " successfully." <<RESET << std::endl; \
			sleep(1);\
		}\
}
#else
#define DEBUG(message, flag) ((void)0)
#endif


void	debug(const std::string &message, int flag)
{
	#ifdef DEBUG_MODE
		DEBUG(message, flag);
    #else
	{
		if (flag == THROW_ERR)
			throw(std::runtime_error(message));
        else if (flag == FAILED)
            std::cerr << RED << message << " failed." << RESET << std::endl;
	}
    #endif
}
