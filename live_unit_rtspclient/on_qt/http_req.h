#pragma once

#include "headers.h"
class http_req : public http_received_event
{
public:
    void set(int id){_id = id;}
    int get() const {return _id;}
    const QString &method(){return _method;}
    void notifyto(bool res,
                      QString &body)
    {
        this->_res = (res == false) ? true : false ;
        d() = body;
        send();
    }
    virtual QString getreqeust_url()
    {
        return QString("/") + _baseurl + QString("/") + _method;
    }
    operator bool ()
    {
        return _res;
    }

protected:
    QString _baseurl;
    QString _method;
    int _id;
    bool _res;
    http_req(const QString &method)    :
        _baseurl("livestreams"),
        _method(method),
        _id(-1),
    _res(false)
    {}
};
class http_req_list : public http_req
        /*request available playing list*/
{
public:
    http_req_list() :
        http_req("list"){}
};

class http_req_thumbnail : public http_req
        /*request thumnail*/
{
public:
    const QString _contentsid;
     const QString _name;
     const QString _type;
     unsigned _display_width;
     unsigned _display_height;
     unsigned _position_seconds;
public:
    http_req_thumbnail(const QString &contentsid,
                     const QString &name,
                     const QString &type,
                     unsigned display_width,
                     unsigned display_height,
                     unsigned position_seconds) :
        http_req("thumbnail"),
        _contentsid(contentsid),
        _name(name),
        _type(type),
        _display_width(display_width),
        _display_height(display_height),
        _position_seconds(position_seconds){ }
    virtual QString getreqeust_url()
    {
        return QString("/") +
                _baseurl +
                QString("/") +
                _contentsid +
                QString("/") +
                _method +
                QString("?") +
                QString("width=") +
                QString("%1").arg(_display_width) +
                QString("&")  +
                QString("height=") +
                QString("%1").arg(_display_height) +
                QString("&") +
                QString("pts=") +
                QString("%1").arg(_position_seconds);
    }
};
class http_req_information : public http_req
        /*request information*/
{
public:
    const QString _contentsid;
    http_req_information(const QString &contentsid) :
        http_req("information"),
        _contentsid(contentsid) {     }

    virtual QString getreqeust_url()
    {
          return QString("/") + _baseurl + QString("/") + _contentsid + QString("/") + _method;
    }
};



class http_req_play : public http_req
        /*request play*/
{

public:
    const QString _contentsid;

    QPixmap _savethumbnail;
    http_req_play(const QString &contentsid,
                  const QPixmap &pix) :
        http_req("play"),
        _contentsid(contentsid),
    _savethumbnail(pix){ }
    virtual QString getreqeust_url()
    {
          return QString("/") + _baseurl + QString("/") + _contentsid + QString("/") + _method;
    }
};



class http : public QHttp
{
        Q_OBJECT
private:
    QList<http_req *> _req_list;
private slots:
       void request_complete(int id, bool res)
       {
           for(int i = 0; i < _req_list.size(); i++)
           {
               if(_req_list.at(i)->get() == id)
               {

                    QString s = QString(readAll());
                   _req_list.takeAt(i)->notifyto(res, s);
                   break;
               }
           }
       }
public:
    http(const QString &host,
         quint16 port,
         QObject *parent = 0) :
        QHttp(host, port, parent)
    {
        connect(this,
                SIGNAL(requestFinished(int, bool)),
                this,
                SLOT(request_complete(int, bool)));
    }
    void send(http_req *req_object)
    {
        req_object->set(get(req_object->getreqeust_url()));
        _req_list.append(req_object);
    }

};



