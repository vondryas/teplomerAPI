#include <facades/utils/passwdhash/passwdhash.hpp>
#include <drogon/drogon.h>

int main() {
	//Set HTTP listener address and port
	//Load config file

	drogon::app().loadConfigFile("config.json");
	drogon::app().run();
	drogon::app().enableSession(0);
	return 0;
}
