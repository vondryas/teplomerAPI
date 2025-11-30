#pragma once

#define NOMINMAX
#include "AdminFilter.h"
#include "JwtFilter.h"
#include "ResourceOwnerFilter.h"

#define LoginFilter "filters::JwtFilter"
#define AdminFilter "filters::JwtFilter", "filters::AdminFilter"
#define OwnerFilter "filters::JwtFilter", "filters::ResourceOwnerFilter"
// todo owner