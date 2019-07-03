#pragma once

class SiStripDigi {

 public:

  SiStripDigi( const uint16_t& strip, 
	     const uint16_t& adc ) : 
    strip_(strip), 
    adc_(adc) {;}
  
  SiStripDigi() : strip_(0), adc_(0) {;}
  ~SiStripDigi() {;}
  
  inline const uint16_t& strip() const;
  inline const uint16_t& adc() const;
  inline const uint16_t& channel() const;

  inline bool operator< ( const SiStripDigi& other ) const;
  
 private:
  
  uint16_t strip_;
  uint16_t adc_;
};

// inline methods
const uint16_t& SiStripDigi::strip() const { return strip_; }
const uint16_t& SiStripDigi::adc() const { return adc_; }
const uint16_t& SiStripDigi::channel() const { return strip(); }
bool SiStripDigi::operator< ( const SiStripDigi& other ) const { return strip() < other.strip(); }

class SiStripCluster  {
public:

  static const uint16_t stripIndexMask = 0x7FFF;  // The first strip index is in the low 15 bits of firstStrip_
  static const uint16_t mergedValueMask = 0x8000;  // The merged state is given by the high bit of firstStrip_

  SiStripCluster() {}

  template<typename Iter>
  SiStripCluster(const uint16_t& firstStrip, 
		 Iter begin, Iter end ):
	 amplitudes_(begin,end), firstStrip_(firstStrip) {}

  /** The number of the first strip in the cluster.
   *  The high bit of firstStrip_ indicates whether the cluster is a candidate for being merged.
   */
  uint16_t firstStrip() const {return firstStrip_ & stripIndexMask;}

  /** The amplitudes of the strips forming the cluster.
   *  The amplitudes are on consecutive strips; if a strip is missing
   *  the amplitude is set to zero.
   *  A strip may be missing in the middle of a cluster because of a
   *  clusterizer that accepts holes.
   *  A strip may also be missing anywhere in the cluster, including the 
   *  edge, to record a dead/noisy channel.
   *
   *  You can find the special meanings of values { 0, 254, 255} in section 3.4.1 of
   *  http://www.te.rl.ac.uk/esdg/cms-fed/firmware/Documents/FE_FPGA_Technical_Description.pdf
   */
  const std::vector<uint8_t>&  amplitudes() const {return amplitudes_;}

  /** total charge
   *
   */
   //int  charge() const { return std::accumulate(amplitudes().begin(), amplitudes().end(), int(0)); }

private:

  std::vector<uint8_t>   amplitudes_;
  uint16_t                firstStrip_ = 0;
};
