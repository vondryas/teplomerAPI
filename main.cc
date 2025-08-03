#include <facades/utils/passwdhash/passwdhash.hpp>
#include <drogon/drogon.h>

int main() {
	//Set HTTP listener address and port
	//Load config file
	std::string hashed = drassist::passwdhash::hash("12456");
	// ARGON2:QrrB8NQCF5JlfWoA:$argon2id$v=19$m=65536,t=2,p=1$+HCM3JuBVPkeF3QiGo7PdA$zIOLPhwKVdSCUfMtq1zd+rCJ9DSMVhkSV+43TRUGtLw
	std::string has = drassist::passwdhash::hash("omg wtf");

	LOG_DEBUG << "Hashed password: " << hashed;
	LOG_DEBUG << "Has password: " << has;
	// Verify the password
	LOG_DEBUG << "Verifying password...";
	if (drassist::passwdhash::verify("12456", hashed)) {
		LOG_DEBUG << "Password verified successfully!";
	}
	else {
		LOG_ERROR << "Password verification failed!";
	}
	LOG_DEBUG << "Verifying password ABCDEF:";
	if (drassist::passwdhash::verify("ABCDEF", hashed)) {
		LOG_DEBUG << "Password verified successfully!";
	}
	else {
		LOG_ERROR << "Password verification failed!";
	}

	drogon::app().loadConfigFile("config.json");
	drogon::app().run();
	return 0;
}
