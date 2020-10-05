#pragma once
extern const QEvent::Type custom_base_event_id;

enum custom_event_id
        /*our custom event ids*/
{
    custom_event_id_send_pixel, /*your video drawing  this class 'pixel'*/
    custom_event_id_send_pcm,   /*your audio playing this class 'pcm'*/
    custom_event_id_endof_pixel,    /* your video has been closed*/
    custom_event_id_endof_pcm,      /*your audio has been closed*/
    custom_event_id_http_received   /*http responsed */
};

class indexed_event : public QEvent
{
    enum custom_event_id _i;
public:
    custom_event_id i()const
    {
        return _i;
    }
    void send()
    {
        QCoreApplication::postEvent(QApplication::instance(),
                                    dynamic_cast<QEvent *>(this));
    }
protected:
    indexed_event(enum custom_event_id i ) :
        QEvent(custom_base_event_id),
        _i(i){}
};

template <typename T /*event for userdata*/ >
class custom_event : public indexed_event
{
private:
    T _t;
protected:
    custom_event(const T &&types,
                 enum custom_event_id i ) :
        indexed_event(i),
        _t(std::move(types)){}
    custom_event(const T &types,
                 enum custom_event_id i ) :
    indexed_event(i),
    _t(types){}
public:
    virtual ~custom_event(){}
    T &d() {return _t;}
};

class pixel_event :
        public custom_event<pixel>
        /*pixel drawing class*/
{
public:
    pixel_event(const pixel &pix) :
        custom_event<pixel>(pix,
                            custom_event_id_send_pixel){ }
};
class endpixel_event :
        public custom_event<pixel>
        /*end pixel drawing class*/
{
public:
    endpixel_event() :
        custom_event<pixel>(pixel(),
                            custom_event_id_endof_pixel){ }
};
class pcm_event :
        public custom_event<pcm>
        /*pcm play class*/
{
public:
    pcm_event(const pcm &pc) :
        custom_event<pcm>(pc,
                          custom_event_id_send_pcm){ }
};
class endpcm_event :
        public custom_event<pcm>
        /*end pcm play class*/
{
public:
    endpcm_event() :
        custom_event<pcm>(pcm(),
                          custom_event_id_endof_pcm){ }
};

class http_received_event :
        public custom_event<QString>
        /*http received class*/
{
protected:
    http_received_event() :
        custom_event<QString>(""/*change after*/,
                              custom_event_id_http_received){ }
};
