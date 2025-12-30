#pragma once
#include <string>

class Message {
public:
    Message() = default;
    Message(const std::string& text, const std::string& playerName);

    std::string GetPlayerName() const;
    std::string GetTextFromPlayer() const;

private:
    std::string FilterNumbers(const std::string& text) const;

    std::string m_text;
    std::string m_playerName;
};
