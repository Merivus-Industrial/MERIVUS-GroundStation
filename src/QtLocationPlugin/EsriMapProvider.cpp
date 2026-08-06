/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "EsriMapProvider.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

namespace {
constexpr auto kWorldImageryUrl = "https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/%1/%2/%3";
}

EsriWorldSatelliteMapProvider::EsriWorldSatelliteMapProvider(QObject* parent)
    : MapProvider(QString(), QString(), AVERAGE_TILE_SIZE, QGeoMapType::SatelliteMapDay, parent)
{
}

QNetworkRequest EsriWorldSatelliteMapProvider::getTileURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager)
{
    QNetworkRequest request;
    const QString url = _getURL(x, y, zoom, networkManager);

    if (url.isEmpty()) {
        return request;
    }

    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("image/jpeg,image/*;q=0.8,*/*;q=0.5"));
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("MERIVUS Ground Station"));

    const QByteArray token = qgcApp()->toolbox()->settingsManager()->appSettings()->esriToken()->rawValue().toString().trimmed().toUtf8();

    if (!token.isEmpty()) {
        request.setRawHeader(QByteArrayLiteral("Authorization"), QByteArrayLiteral("Bearer ") + token);
    }

    return request;
}

QString EsriWorldSatelliteMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager)
{
    Q_UNUSED(networkManager)
    return QString::fromLatin1(kWorldImageryUrl).arg(zoom).arg(y).arg(x);
}
