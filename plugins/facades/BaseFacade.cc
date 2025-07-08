/**
 *
 *  facade_BaseFacade.cc
 *
 */

#include "BaseFacade.h"

using namespace drogon;
using namespace facade;

void facade::BaseFacade::getDbClient()
{
	dbClient = app().getDbClient();
}
