#include <drogon/drogon.h>
#include <plugins/FacadeProvider.h>
int main() {
	//Set HTTP listener address and port
	//Load config file
	drogon::app().loadConfigFile("config.json");

	drogon::app().run();
	return 0;
}
