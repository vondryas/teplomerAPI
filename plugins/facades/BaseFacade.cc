/**
 *
 *  facade_BaseFacade.cc
 *
 */

#include "BaseFacade.h"

using namespace drogon;
using namespace facade;

void BaseFacade::initAndStart(const Json::Value& config)
{
	dbClient = drogon::app().getDbClient();
}

void BaseFacade::shutdown()
{

}
