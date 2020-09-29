#include "requesthandlerfactory.hpp"
#include "requesthandler.hpp"
reqeusthandlerfactory::reqeusthandlerfactory(const std::list<streaming_list::stream_values> &targetlist) :
	_targetlist(targetlist){}
Poco::Net::HTTPRequestHandler* reqeusthandlerfactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
	lm_liveserver_log(dlog::normal, "request from %s\n",
			request.clientAddress().toString().c_str());

	do
	{
		if(request.getURI().empty() ||
				request.getURI()[0] != '/')
		{
			break;
		}
		std::vector<std::string > urls = url_split(request.getURI());
		if(urls.size() < 3)
		{
			/*min /livestreams/?*/
			break;
		}
		if(urls[1] != "livestreams")
		{
			break;
		}
		if(urls[2] == "list")
		{
			return new list_requesthandler(_targetlist);
		}
		/*now check /livestreams/target/?*/
		if(urls.size() < 4)
		{
			break;
		}
		/*check our list*/
		for(auto &it : _targetlist)
		{
			if(std::get<streaming_list::index::contentsindex>(it) == urls[2])
			{
				if(urls[3] == "information")
				{
					return new information_requesthandler(it);
				}

				if(urls[3] == "play")
				{
					return new play_requesthandler(it);
				}

				if(contain_string(urls[3].c_str(),  "thumbnail"))
				{
					return new thumbnail_requesthandler(it);
				}

				break;
			}
		}


	}while(0);

	return new notfound_404_requesthandler();
}
std::vector<std::string> reqeusthandlerfactory::url_split(const std::string &url)
{
	std::vector<std::string> url_split;
	std::string token;
	std::stringstream ss(url);
	char split = '/';
	while(std::getline(ss, token, split))
	{
		url_split.push_back(token);
	}
	return url_split;
}
