/*
  OSMScout for SFOS
  Copyright (C) 2018 Lukas Karas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef OSMSCOUT_SAILFISH_COLLECTIONMAPBRIDGE_H
#define OSMSCOUT_SAILFISH_COLLECTIONMAPBRIDGE_H

#include "Storage.h"

#include <osmscout/MapWidget.h>

#include <QObject>
#include <QtCore/QSet>

class CollectionMapBridge : public QObject {

  Q_OBJECT
  Q_PROPERTY(QObject* map READ getMap WRITE setMap)
  Q_PROPERTY(QString waypointType READ getWaypointType WRITE setWaypointType)
  Q_PROPERTY(QString trackType READ getTrackType WRITE setTrackType)

signals:
  void collectionLoadRequest();
  void collectionDetailRequest(Collection);
  void trackDataRequest(Track track);
  void error(QString message);

public slots:
  void init();
  void storageInitialisationError(QString);
  void onCollectionsLoaded(std::vector<Collection> collections, bool ok);
  void onCollectionDetailsLoaded(Collection collection, bool ok);
  void onTrackDataLoaded(Track track, bool complete, bool ok);

public:
  CollectionMapBridge(QObject *parent = nullptr);

  ~CollectionMapBridge(){}

  inline QObject *getMap() const
  {
    return delegatedMap;
  }

  void setMap(QObject *map);

  inline QString getWaypointType() const
  {
    return waypointTypeName;
  }

  void setWaypointType(QString name);

  inline QString getTrackType() const
  {
    return trackTypeName;
  }

  void setTrackType(QString type);

private:
  osmscout::MapWidget *delegatedMap{nullptr};
  QString waypointTypeName{"_waypoint"};
  QString trackTypeName{"_track"};
  qint64 overlayWptIdBase{10000};
  qint64 overlayTrkIdBase{50000};

  QMap<qint64, QMap<qint64, QDateTime>> displayedWaypoints;
  QMap<qint64, QMap<qint64, QDateTime>> displayedTracks;
};

#endif //OSMSCOUT_SAILFISH_COLLECTIONMAPBRIDGE_H
