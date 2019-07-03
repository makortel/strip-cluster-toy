#include <fstream>
#include <iostream>
#include <algorithm>

#include "Clusterizer.h"
#include "FEDChannel.h"
#include "FEDZSChannelUnpacker.h"

class StripByStripAdder {
public:
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  StripByStripAdder(Clusterizer& clusterizer,
                    Clusterizer::State& state,
                    std::vector<SiStripCluster>& record)
    : clusterizer_(clusterizer), state_(state), record_(record) {}

  StripByStripAdder& operator= ( SiStripDigi digi )
  {
    clusterizer_.stripByStripAdd(state_, digi.strip(), digi.adc(), record_);
    return *this;
  }

  StripByStripAdder& operator*  ()    { return *this; }
  StripByStripAdder& operator++ ()    { return *this; }
  StripByStripAdder& operator++ (int) { return *this; }
private:
  Clusterizer& clusterizer_;
  Clusterizer::State& state_;
  std::vector<SiStripCluster>& record_;
};

template<typename OUT>
OUT unpackZS(const FEDChannel& chan, uint16_t stripOffset, OUT out, detId_t idet)
{
  auto unpacker = FEDZSChannelUnpacker::zeroSuppressedModeUnpacker(chan);
  while (unpacker.hasData()) {
    auto digi = SiStripDigi(stripOffset+unpacker.sampleNumber(), unpacker.adc());
    //std::cout << "unpackZS det " << idet << " digi " << digi.strip() << " sample " << (unsigned int) unpacker.sampleNumber() << " adc " << (unsigned int) unpacker.adc() << std::endl;
    if (digi.strip() != 0) {
      *out++ = digi;
    }
    unpacker++;
  }
  return out;
}

FEDSet fillFeds()
{
  std::ifstream fedfile("stripdata.bin", std::ios::in | std::ios::binary);

  FEDSet feds;
  detId_t detid;  

  while (fedfile.read((char*)&detid, sizeof(detid)).gcount() == sizeof(detid)) {
    FEDChannel fed(fedfile);
    //std::cout << "Det " << detid << " fed " << fed.fedId() << " channel " << (int) fed.fedCh() << " length " << fed.length() << std::endl;
    feds[detid].push_back(std::move(fed));
  }
  return feds;
}

std::vector<SiStripCluster>
fillClusters(detId_t idet, Clusterizer& clusterizer, Clusterizer::State& state, const std::vector<FEDChannel>& channels)
{
  std::vector<SiStripCluster> out;

  auto const & det = clusterizer.stripByStripBegin(idet);
  state.reset(det);

  for (auto const& chan : channels) {
    //std::cout << "Processing channel for detid " << idet << " fed " << chan.fedId() << " channel " << (int) chan.fedCh() << " len:off " << chan.length() << ":" << chan.offset() << " ipair " << chan.iPair() << std::endl;
    auto perStripAdder = StripByStripAdder(clusterizer, state, out);
    unpackZS(chan, chan.iPair()*256, perStripAdder, idet);
  }
  clusterizer.stripByStripEnd(state, out);

  return out;
}

int main()
{
  Clusterizer clusterizer;
  Clusterizer::State state;

  FEDSet feds(fillFeds());
  for (auto idet : clusterizer.allDetIds()) {
    if (feds.find(idet) != feds.end()) {
      auto out = fillClusters(idet, clusterizer, state, feds[idet]);
    }
  }
}
