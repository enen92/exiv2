// SPDX-License-Identifier: GPL-2.0-or-later

#include "photoshop.hpp"

#include "error.hpp"
#include "iptc.hpp"

#include <gtest/gtest.h>

using namespace Exiv2;

namespace {
constexpr std::array validMarkers{"8BIM", "AgHg", "DCSR", "PHUT"};
}  // namespace

TEST(PhotoshopIsIrb, returnsTrueWithValidMarkers) {
  for (const auto& marker : validMarkers) {
    ASSERT_TRUE(Photoshop::isIrb(reinterpret_cast<const byte*>(marker)));
  }
}

TEST(PhotoshopIsIrb, returnsFalseWithInvalidMarkers) {
  ASSERT_FALSE(Photoshop::isIrb(reinterpret_cast<const byte*>("7BIM")));
  ASSERT_FALSE(Photoshop::isIrb(reinterpret_cast<const byte*>("AGHg")));
  ASSERT_FALSE(Photoshop::isIrb(reinterpret_cast<const byte*>("dcsr")));
  ASSERT_FALSE(Photoshop::isIrb(reinterpret_cast<const byte*>("LUIS")));
}

TEST(PhotoshopIsIrb, returnsFalseWithNullPointer) {
  ASSERT_FALSE(Photoshop::isIrb(nullptr));
}

/// \note probably this is not safe and we need to remove it
TEST(PhotoshopIsIrb, returnsFalseWithShorterMarker) {
  ASSERT_FALSE(Photoshop::isIrb(reinterpret_cast<const byte*>("8BI")));
}

TEST(PhotoshopLocateIrb, returnsMinus2withInvalidPhotoshopIRB) {
  const std::string data{"8BIMlalalalalalala"};
  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;
  ASSERT_EQ(-2, Photoshop::locateIrb(reinterpret_cast<const byte*>(data.c_str()), data.size(), Photoshop::iptc_,
                                     &record, sizeHdr, sizeData));
}

TEST(PhotoshopLocateIrb, returnsMinus2WithMarkerNotStartingWith8BIM) {
  const std::string data{"7BIMlalalalalalalala"};
  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;
  ASSERT_EQ(-2, Photoshop::locateIrb(reinterpret_cast<const byte*>(data.c_str()), data.size(), Photoshop::iptc_,
                                     &record, sizeHdr, sizeData));
}

TEST(PhotoshopLocateIrb, returns3withNotLongEnoughData) {
  const std::string data{"8BIMlala"};
  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;
  ASSERT_EQ(3, Photoshop::locateIrb(reinterpret_cast<const byte*>(data.c_str()), data.size(), Photoshop::iptc_, &record,
                                    sizeHdr, sizeData));
}

TEST(PhotoshopLocateIrb, returns0withGoodIptcIrb) {
  // Data taken from file test/data/DSC_3079.jpg
  // The IPTC marker is 0x04 0x04
  const std::array<byte, 68> data{0x38, 0x42, 0x49, 0x4d, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x1c, 0x01,
                                  0x5a, 0x00, 0x03, 0x1b, 0x25, 0x47, 0x1c, 0x02, 0x00, 0x00, 0x02, 0x00, 0x04, 0x1c,
                                  0x02, 0x19, 0x00, 0x07, 0x41, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x61, 0x00, 0x38, 0x42,
                                  0x49, 0x4d, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3f, 0x65, 0x16, 0xda,
                                  0x51, 0x3f, 0xfe, 0x5c, 0xbb, 0x52, 0xf3, 0x2e, 0x36, 0x7b, 0x97, 0x3d};

  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;

  ASSERT_EQ(0, Photoshop::locateIrb(data.data(), data.size(), Photoshop::iptc_, &record, sizeHdr, sizeData));
  ASSERT_EQ(12, sizeHdr);
  ASSERT_EQ(27, sizeData);
}

TEST(PhotoshopLocateIptcIrb, returns0withGoodIptcIrb) {
  // Data taken from file test/data/DSC_3079.jpg
  // The IPTC marker is 0x04 0x04
  const std::array<byte, 68> data{0x38, 0x42, 0x49, 0x4d, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x1c, 0x01,
                                  0x5a, 0x00, 0x03, 0x1b, 0x25, 0x47, 0x1c, 0x02, 0x00, 0x00, 0x02, 0x00, 0x04, 0x1c,
                                  0x02, 0x19, 0x00, 0x07, 0x41, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x61, 0x00, 0x38, 0x42,
                                  0x49, 0x4d, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3f, 0x65, 0x16, 0xda,
                                  0x51, 0x3f, 0xfe, 0x5c, 0xbb, 0x52, 0xf3, 0x2e, 0x36, 0x7b, 0x97, 0x3d};

  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;

  ASSERT_EQ(0, Photoshop::locateIptcIrb(data.data(), data.size(), &record, sizeHdr, sizeData));
  ASSERT_EQ(12, sizeHdr);
  ASSERT_EQ(27, sizeData);
}

TEST(PhotoshopLocateIptcIrb, returns3withoutIptcMarker) {
  // Data taken from file test/data/DSC_3079.jpg
  // The IPTC marker (0x04 0x04) was manipulated to 0x03 0x04
  const std::array<byte, 68> data{0x38, 0x42, 0x49, 0x4d, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x1c, 0x01,
                                  0x5a, 0x00, 0x03, 0x1b, 0x25, 0x47, 0x1c, 0x02, 0x00, 0x00, 0x02, 0x00, 0x04, 0x1c,
                                  0x02, 0x19, 0x00, 0x07, 0x41, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x61, 0x00, 0x38, 0x42,
                                  0x49, 0x4d, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3f, 0x65, 0x16, 0xda,
                                  0x51, 0x3f, 0xfe, 0x5c, 0xbb, 0x52, 0xf3, 0x2e, 0x36, 0x7b, 0x97, 0x3d};

  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;

  ASSERT_EQ(3, Photoshop::locateIptcIrb(data.data(), data.size(), &record, sizeHdr, sizeData));
}

TEST(PhotoshopLocatePreviewIrb, returns0withGoodPreviewIrb) {
  // Data taken from file test/data/DSC_3079.jpg
  // The IPTC marker is 0x04 0x04 was transformed to a Preview one => (0x04 0x0c)
  const std::array<byte, 68> data{0x38, 0x42, 0x49, 0x4d, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x1c, 0x01,
                                  0x5a, 0x00, 0x03, 0x1b, 0x25, 0x47, 0x1c, 0x02, 0x00, 0x00, 0x02, 0x00, 0x04, 0x1c,
                                  0x02, 0x19, 0x00, 0x07, 0x41, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x61, 0x00, 0x38, 0x42,
                                  0x49, 0x4d, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3f, 0x65, 0x16, 0xda,
                                  0x51, 0x3f, 0xfe, 0x5c, 0xbb, 0x52, 0xf3, 0x2e, 0x36, 0x7b, 0x97, 0x3d};

  const Exiv2::byte* record;
  uint32_t sizeHdr = 0;
  uint32_t sizeData = 0;

  ASSERT_EQ(0, Photoshop::locatePreviewIrb(data.data(), data.size(), &record, sizeHdr, sizeData));
  ASSERT_EQ(12, sizeHdr);
  ASSERT_EQ(27, sizeData);
}

// --------------------------------

TEST(PhotoshopSetIptcIrb, withEmptyDataReturnsEmptyBuffer) {
  const IptcData iptc;
  DataBuf buf = Photoshop::setIptcIrb(nullptr, 0, iptc);
  ASSERT_TRUE(buf.empty());
}

TEST(PhotoshopSetIptcIrb, detectIntegerOverflowWithDataFromPOC2179) {
  const std::array<byte, 141> data{
      0x38, 0x42, 0x49, 0x4d, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x38, 0x42, 0x49, 0x4d, 0x04, 0x04,
      0x00, 0x20, 0x00, 0x00, 0x00, 0x75, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0x20, 0x20, 0x20,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd9, 0x20, 0xff, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0xff, 0xff, 0x20, 0x20, 0xff, 0x20, 0xff, 0xff, 0xff, 0xff, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0x20, 0x20, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0x20, 0xff, 0xff, 0xff, 0xff, 0x20, 0xff, 0xff, 0x20, 0xff, 0xff, 0xff};

  const IptcData iptc;

  ASSERT_THROW(Photoshop::setIptcIrb(data.data(), data.size(), iptc), Exiv2::Error);
}

TEST(PhotoshopSetIptcIrb, returnsEmptyBufferWhenDataDoesNotHave8BIM) {
  // First byte replaced from 0x38 to 0x37
  const std::array<byte, 181> data{
      0x37, 0x42, 0x49, 0x4d, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x38, 0x42, 0x49, 0x4d, 0x04, 0x04, 0x00,
      0x20, 0x00, 0x00, 0x00, 0x75, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0x20, 0x20, 0x20, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd9, 0x20, 0xff, 0x20, 0x20, 0xff, 0xed, 0x00, 0x15, 0x50, 0x68, 0x6f, 0x74,
      0x6f, 0x73, 0x68, 0x6f, 0x70, 0x20, 0x33, 0x2e, 0x30, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xed, 0x00, 0x54,
      0x50, 0x68, 0x6f, 0x74, 0x6f, 0x73, 0x68, 0x6f, 0x70, 0x20, 0x33, 0x2e, 0x30, 0x00, 0x20, 0x20, 0x20, 0x20, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff, 0xff, 0x20, 0x20, 0xff, 0x20, 0xff,
      0xff, 0xff, 0xff, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xff,
      0xff, 0x20, 0x20, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0xff, 0xff, 0xff, 0xff, 0x20,
      0xff, 0xff, 0x20, 0xff, 0xff, 0xff, 0xff, 0xd9, 0x0d, 0x0a};

  const IptcData iptc;

  DataBuf buf = Photoshop::setIptcIrb(data.data(), data.size(), iptc);
  ASSERT_TRUE(buf.empty());
}
