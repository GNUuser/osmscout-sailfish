/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2017 Lukas Karas

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

#include "AppSettings.h"

#include <osmscout/OSMScoutQt.h>

#include <QDebug>

using namespace osmscout;

AppSettings::AppSettings(): view(NULL)
{
}

MapView *AppSettings::GetMapView()
{
  if (view == NULL){
    double lat   = settings.value("settings/map/lat",   0).toDouble();
    double lon   = settings.value("settings/map/lon",   0).toDouble();
    double angle = settings.value("settings/map/angle", 0).toDouble();
    double mag   = settings.value("settings/map/mag",   osmscout::Magnification(osmscout::Magnification::magContinent).GetMagnification()).toDouble();
    view = new MapView(this,
                       osmscout::GeoCoord(lat, lon),
                       angle,
                       osmscout::Magnification(mag),
                       OSMScoutQt::GetInstance().GetSettings()->GetMapDPI());
  }
  return view;
}

void AppSettings::SetMapView(QObject *o)
{
  MapView *updated = dynamic_cast<MapView*>(o);
  if (updated == NULL){
    qWarning() << "Failed to cast " << o << " to MapView*.";
    return;
  }
  bool changed = false;
  if (view == NULL){
    view = new MapView(this,
                       osmscout::GeoCoord(updated->GetLat(), updated->GetLon()),
                       updated->GetAngle(),
                       osmscout::Magnification(updated->GetMag()),
                       updated->GetMapDpi());
    changed = true;
  }else{
    changed = *view != *updated;
    if (changed){
        view->operator =( *updated );
    }
  }
  if (changed){
    settings.setValue("settings/map/lat", view->GetLat());
    settings.setValue("settings/map/lon", view->GetLon());
    settings.setValue("settings/map/angle", view->GetAngle());
    settings.setValue("settings/map/mag", view->GetMag());
    emit MapViewChanged(view);
  }
}

const QString AppSettings::GetGpsFormat() const
{
  return settings.value("gpsFormat", "degrees").toString();
}

void AppSettings::SetGpsFormat(const QString formatId)
{
  if (GetGpsFormat() != formatId){
    settings.setValue("gpsFormat", formatId);
    emit GpsFormatChanged(formatId);
  }
}

bool AppSettings::GetHillShades() const
{
  return settings.value("hillShades", false).toBool();
}

void AppSettings::SetHillShades(bool b)
{
  if (b!=GetHillShades()) {
    settings.setValue("hillShades", b);
    emit HillShadesChanged(b);
  }
}

double AppSettings::GetHillShadesOpacity() const
{
  return settings.value("hillShadesOpacity", 0.6).toDouble();
}

void AppSettings::SetHillShadesOpacity(double d)
{
  if (d!=GetHillShadesOpacity()) {
    settings.setValue("hillShadesOpacity", d);
    emit HillShadesOpacityChanged(d);
  }
}
