#include <array>
#include <cassert>
#include <cstring>
#include <sstream>
#include <stdexcept>

#include "plantumlurlcodec.h"
#include "zlib.h"

PlantUMLUrlCodec::PlantUMLUrlCodec()
{
    initialize();
}

std::string PlantUMLUrlCodec::Encode(const std::string &data)
{
    std::string zStr = compressDeflate(data);
    return encode64(zStr);
}

std::string PlantUMLUrlCodec::Decode(const std::string &data)
{
    std::string zStr = decode64(data);
    return uncompressInflate(zStr);
}

std::string PlantUMLUrlCodec::uncompressInflate(const std::string &str)
{
    z_stream zs; // z_stream is zlib's control structure
    std::memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
    {
        throw(std::runtime_error("deflateInit failed while uncompressing."));
    }

    zs.next_in  = (Bytef *)str.data();
    zs.avail_in = str.size(); // set the z_stream's input

    int                             ret           = 0;
    const size_t                    outBufferSize = 32768;
    std::array<char, outBufferSize> outbuffer {};
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do
    {
        zs.next_out  = reinterpret_cast<Bytef *>(outbuffer.data());
        zs.avail_out = outBufferSize;

        ret = inflate(&zs, Z_NO_FLUSH);

        if (outstring.size() < zs.total_out)
        {
            // append the block to the output string
            outstring.append(outbuffer.data(), zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib uncompression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

unsigned char PlantUMLUrlCodec::decode6bit(unsigned char b)
{
    return m_decode6bit[b];
}

unsigned char PlantUMLUrlCodec::encode6bit(unsigned char b)
{
    if (b < 10)
    {
        return 48 + b;
    }
    b -= 10;
    if (b < 26)
    {
        return 65 + b;
    }
    b -= 26;
    if (b < 26)
    {
        return 97 + b;
    }
    b -= 26;
    if (b == 0)
    {
        return '-';
    }
    if (b == 1)
    {
        return '_';
    }
    assert(0);
    return '?';
}

void PlantUMLUrlCodec::decode3bytes(unsigned char *r, int pos, unsigned char cc1, unsigned char cc2, unsigned char cc3, unsigned char cc4)
{
    int c1     = m_decode6bit[cc1];
    int c2     = m_decode6bit[cc2];
    int c3     = m_decode6bit[cc3];
    int c4     = m_decode6bit[cc4];
    r[pos]     = (unsigned char)((c1 << 2) | (c2 >> 4));
    r[pos + 1] = (unsigned char)(((c2 & 0x0F) << 4) | (c3 >> 2));
    r[pos + 2] = (unsigned char)(((c3 & 0x3) << 6) | c4);
}

std::string PlantUMLUrlCodec::decode64(const std::string &s)
{
    size_t length = s.size();
    size_t r      = length % 4;
    if (r != 0)
    {
        length += 4 - r;
    }
    assert(length % 4 == 0);
    r = (length * 3 + 3) / 4;

    auto *data = new unsigned char[r];
    int   pos  = 0;
    for (size_t i = 0; i < s.length(); i += 4)
    {
        decode3bytes(data, pos, s.at(i), s.at(i + 1), s.at(i + 2), s.at(i + 3));
        pos += 3;
    }

    std::string res((const char *)data, r);
    delete[] data;
    return res;
}

void PlantUMLUrlCodec::initialize()
{
    unsigned char encode6bitTable[64] = {0};
    std::memset(m_decode6bit, 0, sizeof(m_decode6bit));
    for (unsigned char b = 0; b < 64; b++)
    {
        encode6bitTable[b]               = encode6bit(b);
        m_decode6bit[encode6bitTable[b]] = b;
    }
}

std::string PlantUMLUrlCodec::compressDeflate(const std::string &str, int compressionlevel)
{
    z_stream zs; // z_stream is zlib's control structure
    std::memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK)
    {
        throw(std::runtime_error("deflateInit failed while compressing."));
    }

    zs.next_in  = (Bytef *)str.data();
    zs.avail_in = str.size(); // set the z_stream's input

    int                             ret           = 0;
    const size_t                    outBufferSize = 32768;
    std::array<char, outBufferSize> outbuffer {};
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do
    {
        zs.next_out  = reinterpret_cast<Bytef *>(outbuffer.data());
        zs.avail_out = outBufferSize;

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out)
        {
            // append the block to the output string
            outstring.append(outbuffer.data(), zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END)
    { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

std::string PlantUMLUrlCodec::append3bytes(unsigned char b1, unsigned char b2, unsigned char b3)
{
    auto        c1 = b1 >> 2;
    auto        c2 = ((b1 & 0x3) << 4) | (b2 >> 4);
    auto        c3 = ((b2 & 0xF) << 2) | (b3 >> 6);
    auto        c4 = b3 & 0x3F;
    std::string r;
    r += encode6bit(c1 & 0x3F);
    r += encode6bit(c2 & 0x3F);
    r += encode6bit(c3 & 0x3F);
    r += encode6bit(c4 & 0x3F);
    return r;
}

std::string PlantUMLUrlCodec::encode64(const std::string &data)
{
    std::string r;
    for (size_t i = 0; i < data.length(); i += 3)
    {
        if (i + 2 == data.length())
        {
            r.append(append3bytes(data.at(i), data.at(i + 1), 0));
        }
        else if (i + 1 == data.length())
        {
            r.append(append3bytes(data.at(i), 0, 0));
        }
        else
        {
            r.append(append3bytes(data.at(i), data.at(i + 1), data.at(i + 2)));
        }
    }
    return r;
}
