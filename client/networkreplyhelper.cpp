#include <QIODevice>
#include <QTimer>
#include <zlib.h>

#include "networkreplyhelper.h"

static QByteArray gUncompress(const QByteArray &data)
{
    if (data.size() <= 4)
    {
        qWarning("gUncompress: Input data is truncated");
        return data;
    }

    QByteArray result;

    int              ret;
    z_stream         strm;
    static const int CHUNK_SIZE = 1024;
    char             out[CHUNK_SIZE];

    /* allocate inflate state */
    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in  = (Bytef *)(data.data());

    ret = inflateInit2(&strm, 15 + 32); // gzip decoding
    if (ret != Z_OK)
        return data;

    // run inflate()
    do
    {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out  = (Bytef *)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR); // state not clobbered

        switch (ret)
        {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR; // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return data;
        }

        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    return result;
}

NetworkReplyHelper::NetworkReplyHelper(QNetworkReply *reply, QIODevice *storage, QObject *parent)
    : QObject(parent), m_reply(reply), m_storage(storage)
{
    Q_ASSERT(reply);
    connect(reply, &QNetworkReply::downloadProgress, this, &NetworkReplyHelper::downloadProgress);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::finished, this, &NetworkReplyHelper::finished);
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkReplyHelper::sslErrors);
    connect(reply, &QNetworkReply::uploadProgress, this, &NetworkReplyHelper::uploadProgress);
    connect(reply, &QNetworkReply::readyRead, this, &NetworkReplyHelper::readyRead);
}

NetworkReplyHelper::~NetworkReplyHelper()
{
    if (m_reply)
    {
        m_reply->disconnect(this);
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    if (m_timeoutTimer)
    {
        if (m_timeoutTimer->isActive())
            m_timeoutTimer->stop();
        delete m_timeoutTimer;
        m_timeoutTimer = nullptr;
    }
}

void NetworkReplyHelper::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__ << bytesReceived << bytesTotal;
#else
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
#endif
    if (m_timeoutTimer && m_timeoutTimer->isActive())
        m_timeoutTimer->start();
}

void NetworkReplyHelper::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    m_errMsg.append(reply->errorString() + "\n");

#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__ << m_errMsg;
#endif

    emit errorMessage(code, m_errMsg);
}

void NetworkReplyHelper::finished()
{
    if (m_timeoutTimer)
        m_timeoutTimer->stop();

    auto *reply           = qobject_cast<QNetworkReply *>(sender());
    auto  contentEncoding = reply->rawHeader("Content-Encoding");
#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__ << contentEncoding << m_content.length() << "bytes received";
#endif
    if (contentEncoding == "gzip" || contentEncoding == "deflate")
    {
        m_content = gUncompress(m_content);
        if (m_storage)
            m_storage->write(m_content);
    }

#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__ << m_content.length() << "bytes uncompressed: " << QString(m_content).left(256) << "\n";
#endif
    emit done();
}

void NetworkReplyHelper::sslErrors(const QList<QSslError> &errors)
{
    Q_FOREACH (const QSslError &e, errors)
    {
#if !defined(QT_NO_DEBUG)
        qDebug() << __FUNCTION__ << __LINE__ << e.errorString();
#endif
        m_errMsg.append(e.errorString() + "\n");
    }
}

void NetworkReplyHelper::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    Q_UNUSED(bytesSent);
    Q_UNUSED(bytesTotal);
    if (m_timeoutTimer && m_timeoutTimer->isActive())
        m_timeoutTimer->start();
}

void NetworkReplyHelper::readyRead()
{
    auto *reply      = qobject_cast<QNetworkReply *>(sender());
    int   statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__ << statusCode;
#endif
    if (statusCode >= 200 && statusCode < 300)
    {
        auto ba              = reply->readAll();
        auto contentEncoding = reply->rawHeader("Content-Encoding");
        if (contentEncoding != "gzip" && contentEncoding != "deflate")
        {
            m_storage->write(ba);
        }
#if !defined(QT_NO_DEBUG)
        qDebug() << __FUNCTION__ << __LINE__ << contentEncoding << ba.length();
#endif
        m_content.append(ba);
    }
}

void NetworkReplyHelper::timeout()
{
#if !defined(QT_NO_DEBUG)
    qDebug() << __FUNCTION__ << __LINE__;
#endif
    if (m_reply && m_reply->isRunning())
        m_reply->abort();
}

QVariant NetworkReplyHelper::data() const
{
    return m_data;
}

void NetworkReplyHelper::setData(const QVariant &data)
{
    m_data = data;
}

void NetworkReplyHelper::setTimeout(int milliseconds)
{
    if (!m_reply->isRunning())
        return;
    if (!m_timeoutTimer)
    {
        m_timeoutTimer = new QTimer;
        m_timeoutTimer->setSingleShot(true);
        connect(m_timeoutTimer, &QTimer::timeout, this, &NetworkReplyHelper::timeout);
    }
    m_timeoutTimer->start(milliseconds);
}
