#pragma once

#include <vector>
#include <map>
#include <fstream>

#include "Strip.h"

using detId_t = uint32_t;
using fedId_t = uint16_t;
using fedCh_t = uint8_t;

class Clusterizer {
public:
  Clusterizer();

  // state of detID
  class Det {
    struct DetStrip {
      uint16_t strip_;
      float noise_;
      float gain_;
      bool bad_;
    };
  public:
    Det() {}
    Det(detId_t id, std::ifstream& file);
    float noise(const uint16_t& strip) const { return strips_[strip-offset_].noise_; }
    float gain(const uint16_t& strip)  const { return strips_[strip-offset_].gain_; }
    bool bad(const uint16_t& strip)    const { return strips_[strip-offset_].bad_; }
    bool allBadBetween(uint16_t L, const uint16_t& R) const { while( ++L < R  &&  bad(L)) {}; return L == R; }
    detId_t id() const { return id_; }
  private:
    std::vector<DetStrip> strips_;
    std::vector<fedId_t> fedIDs_;
    detId_t id_ = 0;
    int offset_ = 0;
  };

  //state of the candidate cluster
  struct State {
    State(Det const & idet) : mp_det(&idet) { ADCs.reserve(128);}
    State() : mp_det(nullptr) { ADCs.reserve(128); }
    Det const& det() const { return *mp_det; }
    void reset(Det const& idet) {
      mp_det = &idet;
      ADCs.clear();
      lastStrip = 0; noiseSquared = 0; candidateLacksSeed = true;
    }
    std::vector<uint8_t> ADCs;  
    uint16_t lastStrip=0;
    float noiseSquared=0;
    bool candidateLacksSeed=true;
  private:
    Det const * mp_det;
  };

  Det stripByStripBegin(uint32_t id) const;

  void stripByStripEnd(State & state, std::vector<SiStripCluster>& out) const;
  void stripByStripAdd(State & state, uint16_t strip, uint8_t adc, std::vector<SiStripCluster>& out) const;

  std::vector<uint32_t> const & allDetIds() const { return detIds_;}
  Det findDetId(const uint32_t) const;

private:
  //constant methods with state information
  uint16_t firstStrip(State const & state) const {return state.lastStrip - state.ADCs.size() + 1;}
  bool candidateEnded(State const & state, const uint16_t&) const;
  bool candidateAccepted(State const & state) const;

  //state modification methods
  template<class T> void endCandidate(State & state, T&) const;
  void clearCandidate(State & state) const { state.candidateLacksSeed = true;  state.noiseSquared = 0;  state.ADCs.clear();}
  void addToCandidate(State & state, const SiStripDigi& digi) const { addToCandidate(state, digi.strip(),digi.adc());}
  void addToCandidate(State & state, uint16_t strip, uint8_t adc) const;
  void appendBadNeighbors(State & state) const;
  void applyGains(State & state) const;

  float ChannelThreshold = 2.0, SeedThreshold = 3.0, ClusterThresholdSquared = 25.0;
  uint8_t MaxSequentialHoles = 0, MaxSequentialBad = 1, MaxAdjacentBad = 0;
  bool RemoveApvShots = true;
  float minGoodCharge = 1620.0;

  using DetSet = std::map<detId_t, Det>;
  DetSet dets_;
  std::vector<detId_t> detIds_;
};


