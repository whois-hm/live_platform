#pragma once
#include "rootheader.hpp"
#include "streaming_list.hpp"
class basic_requesthandler : public Poco::Net::HTTPRequestHandler
/*
 	 class. client's request handler
 */
{
public:
	virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) = 0;
};
struct notfound_404_requesthandler : public basic_requesthandler
/*
 	 class. any fail our supported response
 */
{
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
struct list_requesthandler : public basic_requesthandler
/*
 	 class. our streamlist response
 */
{
private:
	const std::list<streaming_list::stream_values> &_targetlist;
public:
	list_requesthandler(const std::list<streaming_list::stream_values> &targetlist);
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
struct information_requesthandler : public basic_requesthandler
/*
 	 class. contents information response
 */
{
private:
	streaming_list::stream_values _target;/*we sure that _target has valid value, check from "reqeusthandlerfactory"*/
	void handle_message_uvc(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	void handle_message_proxy(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	void handle_message_file(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
public:
	information_requesthandler(const streaming_list::stream_values &target);
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

struct thumbnail_requesthandler : public basic_requesthandler
/*
 	 class thumnail response
 */
{
private:
	streaming_list::stream_values _target;/*we sure that _target has valid value, check from "reqeusthandlerfactory"*/
	triple_int getvalue(Poco::Net::HTTPServerRequest& request, bool &res);
public:
	thumbnail_requesthandler(const streaming_list::stream_values &target);
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

struct play_requesthandler : public basic_requesthandler
/*
 	 class. playing server
 */
{
private:
	streaming_list::stream_values _target;/*we sure that _target has valid value, check from "reqeusthandlerfactory"*/
	std::string make_url();
	std::string get_session_name();
	int get_port();
	std::string get_our_server_ip();
	std::string get_auth_id();
	std::string  get_auth_pwd();
	void start_server_process();
public:
	play_requesthandler(const streaming_list::stream_values &target);
	virtual void handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

