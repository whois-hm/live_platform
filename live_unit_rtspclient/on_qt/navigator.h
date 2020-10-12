#pragma once
#include "headers.h"
class videoreader_thread : public QThread
{
    bool &_readflag;
    playback &_playback;
public:
    videoreader_thread(bool &readflag,
                       playback &play,
                       QObject *obj) :
        QThread(obj),
        _readflag(readflag),
        _playback(play) { }
    virtual void run()
    {
        while(!_readflag)
        {
            pixel pix;
            int res = _playback.take(pix, std::string(avattr::frame_video));
            if(res < 0)
            {
                (new endpixel_event())->send();
            }
            if(res == 0)
            {
                continue;
            }
            (new pixel_event(pix))->send();
        }
    }
};

class navigator : public QLabel
{
    QString _targetcontents;
    QString _res;
    QString _url;
    QString _id;
    QString _pwd;
    QString _cid;
    playback *_playback;
    QSlider *_duration;
    QLabel *_state;
    QLabel *_preseek;
    QLabel *_nextseek;
    bool _bvideo_endofstream;
    videoreader_thread *_videoreader;
private:
    void mousePressEvent(QMouseEvent *ev)
    {
        if(_playback)
        {
            if(_playback->isplaying())
            {
                _state->setText("|>");
                _playback->pause();
            }
            else
            {
                _state->setText("||");
                _playback->play();
            }
        }
    }
    void leaveEvent(QEvent *event)
    {
        if(_playback)
        {
            _duration->hide();
            _state->hide();
            _nextseek->hide();
            _preseek->hide();
        }

    }
    void enterEvent(QEvent *event)
    {
        if(_playback)
        {
            _duration->show();
            _state->show();
            if(!_playback->isplaying())
            {
                _state->setText("|>");
            }
            else
            {
                _state->setText("||");
            }
            _nextseek->show();
            _preseek->show();
        }
    }
    void mouseDoubleClickEvent(QMouseEvent *ev)
    {

        if(!_playback)
        {
            return;
        }
        int half = width() / 2;
        if(ev->x() >= half)
        {
            _playback->seek(10.0);
        }
        else
        {
            _playback->seek(-10.0);
        }
    }


public:
    navigator(QWidget *parent) :
        QLabel(parent),
      _targetcontents(""),
      _res(""),
      _url(""),
      _id(""),
      _pwd(""),
      _cid(""),
      _playback(NULL),
      _duration(NULL),
      _state(NULL),
      _preseek(NULL),
      _nextseek(NULL),
      _bvideo_endofstream(true),
      _videoreader(NULL) { }
    void create(const QRect &r)
    {
        setGeometry(r);
        setMouseTracking(true);
        QFont font;
        font.setBold(true);
        font.setPixelSize(30);
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::blue);
        _duration = new QSlider(Qt::Horizontal, this);
        _duration->setGeometry(50 , height() - 50, width() - 100, 30);
        _duration->hide();
        _state = new QLabel(this);
        _state->setFont(font);
        _state->setText("|>");
        _state->setAttribute(Qt::WA_TransparentForMouseEvents);
        _state->setPalette(pal);
        _state->setAlignment(Qt::AlignCenter);
        _state->setGeometry(0, 0, width(), height());
        _state->hide();

        _preseek = new QLabel(this);
        _preseek->setFont(font);
        _preseek->setText("<<");
        _preseek->setAttribute(Qt::WA_TransparentForMouseEvents);
        _preseek->setPalette(pal);
        _preseek->setAlignment(Qt::AlignCenter);
        _preseek->setGeometry(0, 0, width() / 2, height());
        _preseek->hide();

        _nextseek = new QLabel(this);
        _nextseek->setAttribute(Qt::WA_TransparentForMouseEvents);
        _nextseek->setText(">>");
        _nextseek->setFont(font);
        _nextseek->setPalette(pal);
        _nextseek->setAlignment(Qt::AlignCenter);
        _nextseek->setGeometry(width() / 2, 0, width() / 2, height());
        _nextseek->hide();
        close();
    }

    virtual ~navigator()
    {
        close();
    }
    void draw_pixel(pixel &pix)
    {
        do
        {
            if(!_playback)
            {
                break;
            }
            if(!pix)
            {
                break;
            }
            QImage im(pix.take<raw_media_data::type_ptr>(),
                      pix.width(),
                      pix.height(),
                      QImage::Format_RGB888);
            QPixmap map;
            if(!map.convertFromImage(im))
            {
                break;
            }
            setPixmap(map);
            return;
        }while(0);
        QPixmap map(width(), height());
        map.fill(QColor(0,0,0));
        setPixmap(map);
        stop_videothread();
        close_if_complete_playback();
    }
    void play_pcm(pcm &pc)
    {
        close_if_complete_playback();
    }
    void close_if_complete_playback()
    {
        if(!videothread_is_run() &&
               !audio_is_run() )
        {
            close();
        }
    }

    void error()
    {
        close();
        QMessageBox msg(NULL);
        msg.setText("can't response server message");
        msg.exec();
    }

    bool open(const QString &targetcontents,
              const QString &res,
              const QString &url,
              const QString &id,
              const QString &pwd,
              const QString &cid,
              const QPixmap &pix)
    {
        close();
        if(res != "ok")
        {
            return false;
        }
        setPixmap(pix.scaled(width(), height()));
        _targetcontents = targetcontents;
        _res = res;
        _url = url;
        _id = id;
        _pwd = pwd;
        _cid = cid;
        avattr attr;
        attr.set(avattr::frame_video, avattr::frame_video, 0, 0.0);
        attr.set(avattr::width, avattr::width, width(), 0.0);
        attr.set(avattr::height, avattr::height, height(), 0.0);
        attr.set(avattr::pixel_format, avattr::pixel_format, AV_PIX_FMT_RGB24, 0);
        _playback = new playback(attr,
                                 strDup(_url.toLocal8Bit().data()),
                                 5000,
                                 _id == "none" ? nullptr : _id.toLocal8Bit().data(),
                                 _pwd == "none" ? nullptr : _pwd.toLocal8Bit().data());
        if(_playback->has(avattr::frame_video))
        {
            _bvideo_endofstream = false;
               _videoreader = new videoreader_thread(  _bvideo_endofstream, *_playback, this);
               _videoreader->start();
            return true;
        }
        return false;
    }
    void stop_videothread()
    {
        _bvideo_endofstream = true;
        if(_videoreader)
        {
            _videoreader->quit();
            _videoreader->wait();
            delete  _videoreader;
            _videoreader = nullptr;
        }
    }
    bool videothread_is_run()
    {
        return _bvideo_endofstream;
    }
    void stop_audio()
    {

    }
    bool audio_is_run()
    {
        return false;
    }

    void close()
    {
        stop_videothread();
        stop_audio();
        if(_playback)
        {
            delete _playback;
            _playback = nullptr;
        }
        _targetcontents = _res = _url = _id = _pwd = _cid = "";
        QPixmap map(width() , height());
        map.fill(QColor(0,0,0));
        this->setPixmap(map);
        if(_duration)_duration->hide();
        if(_state)_state->hide();
        if(_nextseek)_nextseek->hide();
        if(_preseek)_preseek->hide();

    }
};





