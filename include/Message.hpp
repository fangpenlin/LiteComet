#ifndef __LITE_COMET_MESSAGE__HEADER__
#define __LITE_COMET_MESSAGE__HEADER__

#include <list>
#include <string>

namespace lite_comet {

class Message {
private:
    long m_offset;
    std::string m_data;
public:
    Message(long offset, const std::string &data) 
        : m_offset(offset), m_data(data)
    {}

    Message(const Message& other) {
        setOffset(other.getOffset());
        setData(other.getData());
    }

    /**
        @brief get offset of this message
        @return offset
    **/
    inline long getOffset() const {
        return m_offset;
    }

    /**
        @brief set offset of this message
        @param offset: offset to set
    **/
    inline void setOffset(long offset) {
        m_offset = offset;
    }

    /**
        @brief get data of this message
        @return data
    **/
    inline const std::string& getData() const {
        return m_data;
    }

    /**
        @brief set data of this message
        @param data: data to set
    **/
    inline void setData(const std::string& data) {
        m_data = data;
    }
  
};
}   // end namespace lite_comet

#endif

