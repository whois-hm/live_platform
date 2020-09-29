#pragma once
#include "rootheader.hpp"
#include "streaming_list.hpp"

class reqeusthandlerfactory : public Poco::Net::HTTPRequestHandlerFactory
/*
 	 class. receving the client's request and response class create
 */
{
private:
	/*just refernce*/
	const std::list<streaming_list::stream_values> &_targetlist;
	std::vector<std::string> url_split(const std::string &url);
public:
	reqeusthandlerfactory(const std::list<streaming_list::stream_values> &targetlist/*reference*/);
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
};

