#include <facades/utils/passwdhash/passwdhash.hpp>
#include <drogon/drogon.h>
#include <filters/Cors.h>

int main() {
	//Set HTTP listener address and port
	//Load config file

	drogon::app().loadConfigFile("config.json");
	setupCors();
	drogon::app().enableSession(2592000);
	drogon::app().run();
	return 0;
}
