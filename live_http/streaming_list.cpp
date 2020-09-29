#include "streaming_list.hpp"

streaming_list::streaming_list() : _contentsindex(0)
{
	/*
		 first find local device
	 */
	for(Poco::DirectoryIterator it(std::string("/dev"));
			it != Poco::DirectoryIterator();
			++it)
	{
		if(contain_string(it.name().c_str(), "video"))
		{
			add(source_type_uvc,
					(*it).path(),
					it.name());
		}
	}
	/*
		 next find mp4
	 */
	for(Poco::DirectoryIterator it(Poco::Path::current());
			it != Poco::DirectoryIterator();
			++it)
	{
		if(contain_string(it.name().c_str(), ".mp4"))
		{
			add(source_type_file,
					(*it).path(),
					it.name());
		}
	}
	/*
		 next proxy
	 */
	char const *proxys[] =
	{
			"rtsp://184.72.239.149/vod/mp4:BigBuckBunny_175k.mov"
	};
	for(int i = 0;
			i < ((int)sizeof(proxys) / (int)sizeof(proxys[0]));
			i++)
	{
		add(source_type_rtspclient,
				proxys[i],
				proxys[i]);
	}
}
const std::list<streaming_list::stream_values> &streaming_list::get() {
	return _list;
}
void streaming_list::add(enum source_type t,
		const std::string &f,
		const std::string &n)
{
	std::string contents = "contents";
	_list.push_back(std::make_tuple(t,
			f,
			n,
			std::string(contents + Poco::NumberFormatter::format(_contentsindex++))));
}
