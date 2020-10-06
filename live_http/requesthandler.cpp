#include "requesthandler.hpp"

void basic_requesthandler::handleRequest(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
{
	lm_liveserver_log(dlog::normal, "handle message > request from %s\n",
			request.clientAddress().toString().c_str());
	handle_message(request, response);
	std::ostream& ostr = response.send();
	ostr.flush();
}

void notfound_404_requesthandler::handle_message(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
{
	lm_liveserver_log(dlog::normal,"handle message 404 not found\n");
	response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND,
			Poco::Net::HTTPResponse::HTTP_REASON_NO_CONTENT);
}


list_requesthandler::list_requesthandler(const std::list<streaming_list::stream_values> &targetlist) :
	_targetlist(targetlist){}
void list_requesthandler::handle_message(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
{
	int listindex = 0;
	lm_liveserver_log(dlog::normal,"handle message 200 ok list\n");


	Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
	root->set("number", Poco::NumberFormatter::format(_targetlist.size()));

	Poco::JSON::Array::Ptr array = new Poco::JSON::Array();

	for(auto &it : _targetlist)
	{

		Poco::JSON::Object::Ptr array_object = new Poco::JSON::Object();

		char const *stype = std::get<streaming_list::index::type>(it) == source_type_uvc ? "uvc" :
				std::get<streaming_list::index::type>(it) == source_type_file ? "file" :
						"proxy";
		array_object->set("type", stype);
		array_object->set("name", std::get<streaming_list::index::name>(it));
		array_object->set("contentsindex", std::get<streaming_list::index::contentsindex>(it));
		array->set(listindex, array_object);
		listindex++;
	}
	root->set("list", array);

	std::ostringstream os;
	root->stringify(os, 2);
	response.send() << os.str();
}


information_requesthandler::information_requesthandler(const streaming_list::stream_values &target) : _target(target){}
void information_requesthandler::handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	lm_liveserver_log(dlog::normal,"handle message 200 ok information\n");
	handle_message_file(request, response);
	handle_message_uvc(request, response);
	handle_message_proxy(request, response);
}
void information_requesthandler::handle_message_file(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{

	if(std::get<streaming_list::index::type>(_target) != source_type_file)
	{

		return;
	}
	bool has = false;
	int index = 0;


	mediacontainer con(std::get<streaming_list::index::fullpath>(_target).c_str());


	Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
	root->set("name", std::get<streaming_list::index::name>(_target));
	duration_div duration = con.duration(has);

	if(std::get<4>(duration) > 0)
	{
		char stringbuffer[256] = {0, };
		snprintf(stringbuffer, 250, "%d:%d:%d:%d", std::get<0>(duration),
				std::get<1>(duration),
				std::get<2>(duration),
				std::get<3>(duration));
		root->set("duration", std::string(stringbuffer));
	} else root->set("duration", "N/A");

	root->set("bitrate", Poco::NumberFormatter::format(con.bitrate(has)) + "kb/s");

	Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
	Poco::JSON::Object::Ptr array_vobject = new Poco::JSON::Object();
	Poco::JSON::Object::Ptr array_aobject = new Poco::JSON::Object();

	const AVStream *vstrm = con.find_stream(AVMEDIA_TYPE_VIDEO);
	if(vstrm)
	{
		array_vobject->set("mediatype", std::string(av_get_media_type_string(vstrm->codec->codec_type)));
	array_vobject->set("codectype", std::string(avcodec_get_name(vstrm->codec->codec_id)));
		array_vobject->set("width", Poco::NumberFormatter::format(vstrm->codec->width));
		array_vobject->set("height", Poco::NumberFormatter::format(vstrm->codec->height));
		array_vobject->set("pixelformat", av_get_pix_fmt_name(vstrm->codec->pix_fmt));
		array->set(index, array_vobject);
		index++;
	}
	else
	{
		root->set("video", "notfound");
	}



	const AVStream *astrm = con.find_stream(AVMEDIA_TYPE_AUDIO);
	if(astrm)
	{

		array_aobject->set("mediatype", std::string(av_get_media_type_string(astrm->codec->codec_type)));
		array_aobject->set("codectype", std::string(avcodec_get_name(astrm->codec->codec_id)));
		array_aobject->set("channel", Poco::NumberFormatter::format(astrm->codec->channels));
		array_aobject->set("samplingrate", Poco::NumberFormatter::format(astrm->codec->sample_rate));
		array_aobject->set("sampleformat", av_get_sample_fmt_name(astrm->codec->sample_fmt));
		array->set(index, array_aobject);
		index++;
	}
	else
	{
		root->set("audio", "notfound");
	}

	root->set("streams", array);

	std::ostringstream os;
	root->stringify(os, 2);

	response.send() << os.str();
}

void information_requesthandler::handle_message_proxy(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	std::string sdp;
	if(std::get<streaming_list::index::type>(_target) != source_type_rtspclient)
	{
		return;
	}

	sdp = live5rtspclient::targetserver_alive(5000, std::get<streaming_list::index::fullpath>(_target), "","");
	if(!sdp.empty())
	{
		Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
		root->set("name", std::get<streaming_list::index::name>(_target).c_str());
		root->set("sdp", sdp);
		std::ostringstream os;
		root->stringify(os, 2);
		response.send() << os.str();
	}
	else
	{
		/*can't get sdp*/
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_ACCEPTABLE,
				Poco::Net::HTTPResponse::HTTP_REASON_NOT_ACCEPTABLE);
	}
}
void information_requesthandler::handle_message_uvc(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	if(std::get<streaming_list::index::type>(_target) != source_type_uvc)
	{
		return;
	}
	uvc cam(std::get<streaming_list::index::fullpath>(_target).c_str());
	Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
	root->set("name", std::get<streaming_list::index::name>(_target));
	root->set("width", Poco::NumberFormatter::format(cam.video_width()));
	root->set("height", Poco::NumberFormatter::format(cam.video_height()));
	root->set("fps", Poco::NumberFormatter::format(cam.video_fps()));
	root->set("pixelformat", av_get_pix_fmt_name(cam.video_format()));
	std::ostringstream os;
	root->stringify(os, 2);
	response.send() << os.str();
}
thumbnail_requesthandler::thumbnail_requesthandler(const streaming_list::stream_values &target) : _target(target){}
triple_int thumbnail_requesthandler::getvalue(Poco::Net::HTTPServerRequest& request, bool &res)
{
	res = false;
	int width = -1;
	int height = -1;
	int pts = -1;
	unsigned length = request.getContentLength();

	std::string url = request.getURI();
	std::string::size_type n;
	n = url.find("?");

	if( n == std::string::npos)
	{
		/*can't find parameter*/

		return std::make_tuple(-1, -1, -1);
	}
	std::string parameter = url.substr(n + 1);

	std::vector<std::string> param_split;
	std::string token;
	std::stringstream ss(parameter);
	char split = '&';
	while(std::getline(ss, token, split))
	{
		param_split.push_back(token);
	}

	if(param_split.size() != 3)
	{
		/*invalid parameter*/
		return std::make_tuple(-1, -1, -1);
	}

	for(int i = 0; i < param_split.size(); i++)
	{
		std::string key, value;
		n = param_split[i].find("=");

		if( n == std::string::npos)
		{
			/*invalid parameter*/
			return std::make_tuple(-1, -1, -1);
		}

		key = param_split[i].substr(0, n);
		value = param_split[i].substr(n+1);
		if(key == "width") width = std::stoi(value);
		if(key == "height") height = std::stoi(value);
		if(key == "pts") pts = std::stoi(value);
	}

	res = true;
	return std::make_tuple(width,
			height,
			pts);

}
void thumbnail_requesthandler::handle_message(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	bool res = false;
	triple_int value = getvalue(request, res);


	if(!res)
	{
				response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST,
				Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST);
		return;
	}

	Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
	lm_liveserver_log(dlog::normal,"handle message 200 ok thumbnail\n");

	if(std::get<streaming_list::index::type>(_target) != source_type_file)
	{
		root->set("type", "not found");
		root->set("pixelformat", "unknown");
		root->set("width", Poco::NumberFormatter::format(std::get<0>(value)));
		root->set("height", Poco::NumberFormatter::format(std::get<1>(value)));
		root->set("data", "not found");
		std::ostringstream os;
		root->stringify(os, 2);
		response.send() << os.str();
		return;

	}
	mediacontainer con(std::get<streaming_list::index::fullpath>(_target).c_str());
	avattr attr;
	attr.set(avattr::frame_video, "frame_video", 0, 0.0);
	attr.set(avattr::width, "width", std::get<0>(value), 0.0);
	attr.set(avattr::height, "height", std::get<1>(value), 0.0);
	attr.set(avattr::pixel_format,"pix fmt", AV_PIX_FMT_RGB24,0.0);
	pixel pix = con.keyframe_thumbnail((double)std::get<2>(value), attr);
	if(!pix)
	{
		root->set("type", "not found");
		root->set("pixelformat", "unknown");
		root->set("width", Poco::NumberFormatter::format(std::get<0>(value)));
		root->set("height", Poco::NumberFormatter::format(std::get<1>(value)));
		root->set("data", "not found");
		std::ostringstream os;
		root->stringify(os, 2);
		response.send() << os.str();
		return;
	}

	root->set("type", "base64");
	root->set("pixelformat", av_get_pix_fmt_name(pix.format()));
	root->set("width", Poco::NumberFormatter::format(pix.width()));
	root->set("height", Poco::NumberFormatter::format(pix.height()));
	base64_encoder base64encoder(pix.take<uint8_t *>(), pix.take<unsigned>());
	root->set("data", base64encoder.get());

	std::ostringstream os;
	root->stringify(os, 2);

	response.send() << os.str();
}


play_requesthandler::	play_requesthandler(const streaming_list::stream_values &target) :
	_target(target){}
std::string play_requesthandler::make_url()
{
	std::string url;
    url += "rtsp://";
    url += get_our_server_ip();
//    url += ":";
//    url += Poco::NumberFormatter::format(get_port());
    url += "/";
    url += get_session_name();
    return url;
}
std::string play_requesthandler::get_session_name()
{
	return std::string("livestream/") + std::get<streaming_list::index::name>(_target);
}
int play_requesthandler::get_port()
{
	return 554;
}
std::string play_requesthandler::get_our_server_ip()
{
    struct ifreq ifr;
    char ipstr[40];
    memset(ipstr, 0, 40);
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);

    if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {
       return std::string("");/*empty return*/
    }
    inet_ntop(AF_INET, ifr.ifr_addr.sa_data+2,
            ipstr,sizeof(struct sockaddr));



    return std::string(ipstr);
}
std::string play_requesthandler::get_auth_id()
{
	//return std::string("lm_liveserver");
	return "none";
}
std::string play_requesthandler::get_auth_pwd()
{
	return "none";
	return Poco::NumberFormatter::format(78910);
}
void play_requesthandler::start_server_process()
{
	std::list<live5livemediapp_server_session_parameters::session_parameters_pair> paramlist;
	paramlist.push_back(std::make_pair(
			live5livemediapp_server_session_parameters_in(std::get<streaming_list::index::type>(_target),
			std::get<streaming_list::index::fullpath>(_target).c_str(),
			make_sessionsource_functor(),
			get_session_name())
			,
			live5livemediapp_server_session_parameters_out(live5livemediapp_server_session_parameters_out::streamclient_functor())));

	live5livemediapp_server_session_parameters param(paramlist,
			live5livemediapp_server_session_parameters::playingclient_functor(),
			live5livemediapp_server_session_parameters::teardownclient_functor(),
			get_port(),
			get_auth_id().c_str(),
			get_auth_pwd().c_str());
	for(auto &it : param._sessionparameters_pairs)
	{
		char linebuffer[512] = {0, };
		snprintf(linebuffer, 500, "./live_unit_rtspserver -o server -t %s -s '%s' -i '%s' -p %d -u %s -w %s &",
				session_in_par(it)._type == source_type_uvc ? "uvc" : session_in_par(it)._type == source_type_file ? "file" : "proxy",
				session_in_par(it)._session_name.c_str(),
				session_in_par(it)._target.c_str(),
				get_port(),
				get_auth_id().c_str(),
				get_auth_pwd().c_str());

		system(linebuffer);
	}



}
void play_requesthandler::handle_message(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
{
	int clientid = -1;
	Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

	start_server_process();
	lm_liveserver_log(dlog::normal,"handle message 200 ok play\n");
	Time_sleep(1000);
	root->set("result", "ok");
	root->set("sessionname", make_url());
	root->set("auth-id", get_auth_id());
	root->set("auth-password", get_auth_pwd());
	root->set("clientid", Poco::NumberFormatter::format(clientid));

	std::ostringstream os;
	root->stringify(os, 2);
	response.send() << os.str();
}


