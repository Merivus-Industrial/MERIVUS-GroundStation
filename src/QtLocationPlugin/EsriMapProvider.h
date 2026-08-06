/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "MapProvider.h"

class EsriWorldSatelliteMapProvider : public MapProvider {
    Q_OBJECT

  public:
    explicit EsriWorldSatelliteMapProvider(QObject* parent = nullptr);

    QNetworkRequest getTileURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
    QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};
