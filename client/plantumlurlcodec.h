#ifndef PLANTUMLURLCODEC_H
#define PLANTUMLURLCODEC_H

#include <string>

class PlantUMLUrlCodec
{
public:
    PlantUMLUrlCodec();
    std::string Encode(const std::string& data);
    std::string Decode(const std::string& data);
private:
    std::string compressDeflate(const std::string& str, int compressionlevel = 9);
    std::string uncompressInflate(const std::string& str);
    std::string append3bytes(unsigned char b1, unsigned char b2, unsigned char b3);
    void decode3bytes(unsigned char * r, int pos, unsigned char cc1, unsigned char cc2, unsigned char cc3, unsigned char cc4);
    unsigned char decode6bit(unsigned char b);
    unsigned char encode6bit(unsigned char b);
    std::string encode64(const std::string& data);
    std::string decode64(const std::string& data);
    void initialize();
    unsigned char m_decode6bit[128];
};

#endif // PLANTUMLURLCODEC_H
