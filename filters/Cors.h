#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include "trantor/utils/Logger.h"



/// Configure Cross-Origin Resource Sharing (CORS) support.
///
///  This function registers both synchronous pre-processing advice for handling
///  OPTIONS preflight requests and post-handling advice to inject CORS headers
///  into all responses dynamically based on the incoming request headers.
void setupCors();
