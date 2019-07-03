#include "FEDChannel.h"

FEDChannel::FEDChannel(std::ifstream& file)
{
  file.read((char*)&fedid_, sizeof(fedid_));
  file.read((char*)&fedch_, sizeof(fedch_));
  file.read((char*)&ipair_, sizeof(ipair_));
  file.read((char*)&offset_, sizeof(offset_));
  file.read((char*)&length_, sizeof(length_));
  data_.reset(new uint8_t[length_+8]);
  file.read((char*)data_.get(), length_+8);
}