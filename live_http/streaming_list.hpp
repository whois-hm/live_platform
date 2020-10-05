#pragma once
/*
 	 livemedia, poco reference
 */
#include "rootheader.hpp"

class streaming_list
/*
 	 class. container for our stream contents list
 	 -finding the contents in the process folder
 */
{
	/*
	 	 our main contents index
	 */
	unsigned _contentsindex;
public:
	typedef std::tuple<enum source_type, /*source type*/
			std::string,/*full path*/
			std::string, /*source name*/
			std::string/*contents index*/>
	stream_values;

	struct index
	{
		/*
		 	 get index for 'stream_values'
		 */
		static const int type = 0;
		static const int fullpath = 1;
		static const int name = 2;
		static const int contentsindex =3;
	};

	/*
	 	 get the our stream contents list
	 */
	const std::list<streaming_list::stream_values> &get() ;
	/*
	 	 add stream contents if you need
	 */
	void add(enum source_type t,
			const std::string &f,
			const std::string &n);
	/*
	 	 constructor / we making stream list this function using 'add()'
	 */
	streaming_list();
private:
	std::list<streaming_list::stream_values> _list;
};

