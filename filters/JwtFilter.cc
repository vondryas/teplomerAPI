#include "JwtFilter.h"

using namespace facade;
using namespace drogon;
using namespace filters;

// Funkce na převod std::any → string
std::string anyToString(const std::any& value)
{
	if (value.type() == typeid(std::string))
	{
		return std::any_cast<std::string>(value);
	}
	else if (value.type() == typeid(const char*))
	{
		return std::string(std::any_cast<const char*>(value));
	}
	else if (value.type() == typeid(int64_t))
	{
		return std::to_string(std::any_cast<int64_t>(value));
	}
	else if (value.type() == typeid(int))
	{
		return std::to_string(std::any_cast<int>(value));
	}
	return "<unknown type>";
}


void JwtFilter::doFilter(const HttpRequestPtr& request, FilterCallback&& fcb, FilterChainCallback&& fccb) {
	// Skip the verification on method Options
	if (request->getMethod() == HttpMethod::Options) return fccb();

	std::string token;

	if (request->session()->find("Authorization"))
	{
		token = request->session()->get<std::string>("Authorization");
	}
	else
	{
		token = request->getHeader("Authorization");
	}

	// If the authorization header is empty or if the length is lower than 7 characters, means "Bearer " is not included on authorization header string.
	if (token.length() < 7) {
		// Return the response and let's tell this endpoint request was cancelled
		return fcb(responses::unauthorizedResponse("No header authentication!"));
	}


	// Remove the string "Bearer " on token and decode it
	std::map<std::string, std::any> jwtAttributes = JWT::decodeToken(token.substr(7));
	if (jwtAttributes.empty()) {
		return fcb(responses::unauthorizedResponse("Token is invalid!"));
	}

	// Save the claims on attributes, for on next endpoint to be accessible
	for (auto& attribute : jwtAttributes)
	{
		request->getAttributes()->insert("jwt_" + attribute.first, attribute.second);
	}

	// If everything is right, just move to other endpoint
	return fccb();
}
