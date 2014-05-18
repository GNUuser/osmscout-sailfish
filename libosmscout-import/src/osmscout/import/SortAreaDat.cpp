/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/import/SortAreaDat.h>

namespace osmscout {

  class AreaLocationProcessorFilter : public SortDataGenerator<Area>::ProcessingFilter
  {
  private:
    FileWriter               writer;
    uint32_t                 overallDataCount;
    OSMSCOUT_HASHSET<TypeId> poiTypes;

  public:
    bool BeforeProcessingStart(const ImportParameter& parameter,
                               Progress& progress,
                               const TypeConfig& typeConfig);
    bool Process(const FileOffset& offset,
                 Area& area);
    bool AfterProcessingEnd();
  };

  bool AreaLocationProcessorFilter::BeforeProcessingStart(const ImportParameter& parameter,
                                                         Progress& progress,
                                                         const TypeConfig& typeConfig)
  {
    overallDataCount=0;

    typeConfig.GetIndexAsPOITypes(poiTypes);

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "areaaddress.dat"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");

      return false;
    }

    writer.Write(overallDataCount);

    return true;
  }

  bool AreaLocationProcessorFilter::Process(const FileOffset& offset,
                                           Area& area)
  {
    for (std::vector<Area::Ring>::const_iterator ring=area.rings.begin();
        ring!=area.rings.end();
        ++ring) {
      bool isAddress=ring->GetType()!=typeIgnore &&
                     !ring->GetAttributes().GetLocation().empty() &&
                     !ring->GetAttributes().GetAddress().empty();

      bool isPoi=!ring->GetName().empty() && poiTypes.find(ring->GetType())!=poiTypes.end();

      if (!isAddress && !isPoi) {
        continue;
      }

      if (ring->ring==Area::masterRingId &&
          ring->nodes.empty()) {
        for (std::vector<Area::Ring>::const_iterator r=area.rings.begin();
            r!=area.rings.end();
            ++r) {
          if (r->ring==Area::outerRingId) {
            if (!writer.WriteFileOffset(offset)) {
              return false;
            }

            if (!writer.WriteNumber(ring->GetType())) {
              return false;
            }

            if (!writer.Write(ring->GetAttributes().GetName())) {
              return false;
            }

            if (!writer.Write(ring->GetAttributes().GetLocation())) {
              return false;
            }

            if (!writer.Write(ring->GetAttributes().GetAddress())) {
              return false;
            }

            if (!writer.Write(r->nodes)) {
              return false;
            }

            overallDataCount++;
          }
        }
      }
      else {
        if (!writer.WriteFileOffset(offset)) {
          return false;
        }

        if (!writer.WriteNumber(ring->GetType())) {
          return false;
        }

        if (!writer.Write(ring->GetAttributes().GetName())) {
          return false;
        }

        if (!writer.Write(ring->GetAttributes().GetLocation())) {
          return false;
        }

        if (!writer.Write(ring->GetAttributes().GetAddress())) {
          return false;
        }

        if (!writer.Write(ring->nodes)) {
          return false;
        }

        overallDataCount++;
      }

    }

    return true;
  }

  bool AreaLocationProcessorFilter::AfterProcessingEnd()
  {
    writer.SetPos(0);
    writer.Write(overallDataCount);

    return writer.Close();
  }

  void SortAreaDataGenerator::GetTopLeftCoordinate(const Area& data,
                                                   double& maxLat,
                                                   double& minLon)
  {
    bool start=true;
    for (size_t r=0; r<data.rings.size(); r++) {
      if (data.rings[r].ring==Area::outerRingId) {
        for (size_t n=1; n<data.rings[r].nodes.size(); n++) {
          if (start) {
            maxLat=data.rings[r].nodes[n].GetLat();
            minLon=data.rings[r].nodes[n].GetLon();

            start=false;
          }
          else {
            maxLat=std::max(maxLat,data.rings[r].nodes[n].GetLat());
            minLon=std::min(minLon,data.rings[r].nodes[n].GetLon());
          }
        }
      }
    }
  }

  SortAreaDataGenerator::SortAreaDataGenerator()
  : SortDataGenerator<Area>("areas.dat","areas.idmap")
  {
    AddSource(osmRefWay,"wayarea.dat");
    AddSource(osmRefRelation,"relarea.dat");

    AddFilter(new AreaLocationProcessorFilter());
  }

  std::string SortAreaDataGenerator::GetDescription() const
  {
    return "Sort/copy areas";
  }
}
