#include "Message.h"

Message::Message()
{
	m_playerName ="";
	m_text ="";
}

Message::Message(std::string text, std::string playerName, std::chrono::system_clock::time_point time):
    m_text(text), m_playerName(playerName), m_time(time)
{
}

std::string Message::GetPlayerName() const
{
    return m_playerName;
}

std::string Message::GetTextFromPlayer() const
{
	return m_text;
}

std::chrono::system_clock::time_point Message::GetTimeStamp() const
{
    return m_time;
}


void Message::SetPlayerName(Player player)
{
    m_playerName = player.GetUsername();
}

void Message::SetTextFromPlayer(std::string text)
{
    bool contains_numbers = false;
    for (const char& c : text)
    {
        if (std::isdigit(c)) {
            contains_numbers = true;
            break;
        }
    }
    if (contains_numbers)
        m_text = "###";
    else
        m_text = text;
}

void Message::SetTimeStamp(std::chrono::system_clock::time_point time)
{
    m_time = time;
}
