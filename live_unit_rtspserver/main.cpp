#include "core.hpp"
static live5scheduler<live5rtspserver> *_server = nullptr;
static ui *_clientgui = nullptr;
namespace server
{
	struct make_session_default_uvc
	{
		live5livemediapp_serversession_source *operator()(const std::string &target)
		{
			avattr codec;
			codec.set(avattr::frame_video, avattr::frame_video, 0, 0.0);
			codec.set(avattr::width, avattr::width, 160, 0.0);
			codec.set(avattr::height, avattr::height, 120, 0.0);
			codec.set(avattr::fps, avattr::fps, 30, 0.0);
			codec.set(avattr::bitrate, avattr::bitrate, 400000, 0.0);
			codec.set(avattr::gop, avattr::gop, 1, 0.0);
			codec.set(avattr::max_bframe, avattr::max_bframe, 0, 0.0);
			codec.set(avattr::video_encoderid, avattr::video_encoderid, (int)AV_CODEC_ID_H264, 0.0);
			codec.set(avattr::pixel_format, avattr::pixel_format, (int)AV_PIX_FMT_YUV420P, 0.0);
			return new live5livemediapp_serversession_source_uvc(target.c_str(), codec);
		}
	};
	struct make_session_mp4
	{
		live5livemediapp_serversession_source *operator()(const std::string &target)
		{
			return new live5livemediapp_serversession_source_mp4(target.c_str());
		}
	};
	struct make_session_rtspclient
	{
		live5livemediapp_serversession_source *operator()(const std::string &target)
		{
			return new live5livemediapp_serversession_source_rtspclient();
		}
	};

	struct stream_dout
	{
		void operator()(unsigned clientsessionid,
				enum AVMediaType mediatype,
				enum AVCodecID codecid,
				const unsigned char *streamdata,
				unsigned streamdata_size)
		{

		}
	};
	struct client_incoming
	{
		void operator()(std::string clientip,
				unsigned clientsessionid)
		{

		}
	};
	struct client_teardown
	{
		void operator()(std::string clientip,
				unsigned clientsessionid)
		{
			_server->trigger(live5scheduler<live5rtspserver>::live5scheduler_trigger_id_close);
		}
	};

	static live5livemediapp_server_session_parameters_in get_server_in_parameter(const avattr &attr)
	{
		enum source_type  t =
				attr.get<avattr::avattr_type_string>("type") == "uvc" ?   source_type_uvc :
				attr.get<avattr::avattr_type_string>("type") == "file" ?  source_type_file :
														 source_type_rtspclient;

		if(t == source_type_uvc)
		{
			return live5livemediapp_server_session_parameters_in(t,
					 attr.get<avattr::avattr_type_string>("input").c_str(),
					 make_session_default_uvc(),
					 attr.get<avattr::avattr_type_string>("sessionname").c_str());
		}
		if(t == source_type_file)
		{
			return live5livemediapp_server_session_parameters_in(t,
					 attr.get<avattr::avattr_type_string>("input").c_str(),
					 make_session_mp4(),
					 attr.get<avattr::avattr_type_string>("sessionname").c_str());
		}
		return live5livemediapp_server_session_parameters_in(t,
				 attr.get<avattr::avattr_type_string>("input").c_str(),
				 make_session_rtspclient(),
				 attr.get<avattr::avattr_type_string>("sessionname").c_str());
	}
	static live5livemediapp_server_session_parameters_out get_server_out_parameter(const avattr &attr)
	{
		return live5livemediapp_server_session_parameters_out(stream_dout());
	}
	static live5livemediapp_server_session_parameters get_server_parameter(const avattr &attr)
	{
		std::list<live5livemediapp_server_session_parameters::session_parameters_pair> paramlist;
		paramlist.push_back(std::make_pair(get_server_in_parameter(attr),
				get_server_out_parameter(attr)));


		return 	live5livemediapp_server_session_parameters (paramlist,
					client_incoming(),
					client_teardown(),
					attr.get<avattr::avattr_type_int>("port") ,
					attr.get<avattr::avattr_type_string>("auth_id") == "none" ? nullptr : attr.get<avattr::avattr_type_string>("auth_id").c_str(),
					attr.get<avattr::avattr_type_string>("auth_pwd") == "none" ? nullptr : attr.get<avattr::avattr_type_string>("auth_pwd").c_str());
	}

	static void start_process(const avattr &attr)
	{
		if(!attr.notfound("type") &&
				!attr.notfound("sessionname") &&
				!attr.notfound("input") &&
				!attr.notfound("port") &&
				!attr.notfound("auth_id") &&
				!attr.notfound("auth_pwd"))
		{
			printf("lmp start width sessionname = %s inputsource = %s port = %d auth_id = %s auth_pwd = %s\n", attr.get<avattr::avattr_type_string>("sessionname").c_str(),
					attr.get<avattr::avattr_type_string>("input").c_str(),
					attr.get<avattr::avattr_type_int>("port"),
					attr.get<avattr::avattr_type_string>("auth_id").c_str(),
					attr.get<avattr::avattr_type_string>("auth_pwd").c_str());
			_server = new live5scheduler<live5rtspserver> ();
			_server->start(false, get_server_parameter(attr));
		}
	}

}
namespace client
{
	static void start_process(const avattr &attr)
	{

	}
}
static avattr open_process(int argc, char *argv[])
{
	livemedia_pp::ref();

	int opt;
	avattr attr;
	while((opt = getopt(argc, argv, "o:t:s:i:p:u:w:")) != -1)
	{
		if((char)opt == 'o') attr.set("operation", optarg, 0, 0.0);
		else if((char)opt == 't') attr.set("type", optarg, 0, 0.0);
		else if((char)opt == 's') attr.set("sessionname", optarg, 0, 0.0);
		else if((char)opt == 'i') attr.set("input", optarg, 0, 0.0);
		else if((char)opt == 'p') attr.set("port", optarg, atoi(optarg), 0.0);
		else if((char)opt == 'u') attr.set("auth_id", optarg, 0, 0.0);
		else if((char)opt == 'w') attr.set("auth_pwd", optarg, 0, 0.0);
		else if((char)opt == 'z') attr.set("width", optarg, atoi(optarg), 0.0);
		else if((char)opt == 'x') attr.set("height", optarg, atoi(optarg), 0.0);
	}
	return attr;
}
static void close_process()
{
	if(_server)delete _server;
	livemedia_pp::ref(false);
}
int main(int argc, char *argv[])
/* our main */
{
	avattr attr = open_process(argc, argv);
	if(attr.get<avattr::avattr_type_string>("operation") == "server")
	{
		server::start_process(attr);
	}
	if(attr.get<avattr::avattr_type_string>("operation") == "client")
	{
		client::start_process(attr);
	}

	close_process();
	return 0;
}






