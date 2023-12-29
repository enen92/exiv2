// SPDX-License-Identifier: GPL-2.0-or-later
// Sample program to dump the XMP packet of an image

#include <exiv2/exiv2.hpp>
#include <exiv2/image_types.hpp>
#include <iostream>

int main(int argc, char* const argv[]) {
  Exiv2::XmpParser::initialize();
  ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
  Exiv2::enableBMFF();
#endif

  try {
    if (argc != 2) {
      std::cout << "Usage: " << argv[0] << " file\n";
      return EXIT_FAILURE;
    }

    auto image = Exiv2::ImageFactory::open(argv[1]);
    if (image->imageType() == Exiv2::ImageType::jpeg) {
        Exiv2::JpegImage* jpegImage = dynamic_cast<Exiv2::JpegImage*>(image.get());
        jpegImage->readMetadata();
        std::cout << "Number of color components: " << jpegImage->numColorComponents() << "\n";
        std::cout << "Encoding process: " << jpegImage->encodingProcess() << "\n";
    }

    return EXIT_SUCCESS;
  } catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e << "'\n";
    return EXIT_FAILURE;
  }
}
