#include "streaming_list.hpp"

streaming_list::streaming_list() : _contentsindex(0)
{

	std::ifstream f;
	f.open("streamlist.txt");
	if(!f.is_open())
	{
		return;
	}
	while(!f.eof())
	{
		int type = -1;

		char arr[256] = {0, };
		f.getline(arr, 256);
		std::string source(arr);
		std::vector <std::string> list;
		std::stringstream ss(source);
		char splitvalue = '^';
		std::string token;
		while(std::getline(ss, token, splitvalue))
		{
			list.push_back(token);
		}
		if(list.size() == 3)
		{
			if(list[0] == "f")  	type = (int)source_type_file;
			else if(list[0] == "d") type = (int)source_type_uvc;
			else if(list[0] == "p")	type = (int)source_type_rtspclient;

			if(type != -1)
			{
				if(exist((enum source_type) type,
						list[1],
						list[2]))
				{
					add((enum source_type) type,
						list[1],
						list[2]);
				}
			}
		}
	}
	f.close();
	
}
bool streaming_list::exist(enum source_type stype, 
	const std::string &path,
	const std::string &name)
{
	bool res = false;
	lm_liveserver_log(dlog::normal,"cheking source can be streamming... %s\n", path.c_str());
	if(stype == source_type_file)
	{
		Poco::File f(path);
		res = f.exists();
	}
	if(stype == source_type_uvc)
	{
		Poco::File f(path);
		res = f.exists();
	}
	if(stype == source_type_rtspclient)
	{
		res = !live5rtspclient::targetserver_alive(3000, path, "", "").empty();
	}
	if(res) lm_liveserver_log(dlog::normal,"cheking source can be streamming... %s... ok!\n", path.c_str());
	else lm_liveserver_log(dlog::normal,"cheking source can be streamming... %s... fail!\n", path.c_str());
	return res;
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

