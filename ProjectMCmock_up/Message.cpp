#include "Message.h"
#include <regex>

Message::Message(const std::string& text, const std::string& playerName)
    : m_playerName(playerName)
{
    m_text = FilterNumbers(text);
}

std::string Message::GetPlayerName() const {
    return m_playerName;
}

std::string Message::GetTextFromPlayer() const {
    return m_text;
}

std::string Message::FilterNumbers(const std::string& text) const
{
    
    static const std::regex digitRegex(R"(\d)");

    
    return std::regex_replace(text, digitRegex, "#");
}
