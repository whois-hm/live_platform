#include "rootheader.hpp"
#include "streaming_list.hpp"
#include "requesthandlerfactory.hpp"
#include "requesthandler.hpp"




/*----------------------------------------------------------------------------------------------------------------
 class httpserver
 *----------------------------------------------------------------------------------------------------------------*/
class httpserver : public Poco::Util::ServerApplication
{
	streaming_list _streamlist;
	int main(const std::vector<std::string > &args)
	{
		const std::list<streaming_list::stream_values> &oursource_list = _streamlist.get();

		lm_liveserver_log(dlog::normal,"<server open>\n");

		if(oursource_list.size() <= 0)
		{
			lm_liveserver_log(dlog::warnning,"source can't find, close server.\n");
			return -1;
		}

		for(auto &it : oursource_list)
		{
			char const *stype = std::get<streaming_list::index::type>(it) == source_type_uvc ? "uvc" :
					std::get<streaming_list::index::type>(it) == source_type_file ? "file" :
							"proxy";

			lm_liveserver_log(dlog::normal,"source found that %s/%s/%s\n", stype,
					std::get<streaming_list::index::fullpath>(it).c_str(),
					std::get<streaming_list::index::name>(it).c_str());
		}
		lm_liveserver_log(dlog::normal,"now start server\n");

		Poco::Net::HTTPServer sv(new reqeusthandlerfactory(oursource_list));
		sv.start();
		waitForTerminationRequest();
		sv.stop();

		return Application::EXIT_OK;
	}
};




int main(int argc, char **argv)
{


	livemedia_pp::ref();

	lm_liveserver_log.log_enable();
	lm_liveserver_log.console_writer_install();
#if defined(armv7l)
	struct _armv7loutlog
	{
		void operator () (std::string &str)
		{
			std::string o = "echo -e \x1b[31m";
			o += str;
			o += " > /dev/tty0 &";

			system(o.c_str());
		}
	};
	lm_liveserver_log.userout_writer_install(_armv7loutlog());
#endif


	lm_liveserver_log.level_install(dlog::normal);
	lm_liveserver_log.outbuffer_increase(1024);
	lm_liveserver_log.prefix_install("[live_http] ");
	lm_liveserver_log.color(dlog::CYAN);


	httpserver ourserver;

	ourserver.run(argc, argv);


	livemedia_pp::ref(false);
	return 0;
}
