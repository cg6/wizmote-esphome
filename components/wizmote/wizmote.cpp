#include "wizmote.h"

namespace esphome {
namespace wizmote {
void WizMoteListener::on_esp_now_message(esp_now::ESPNowPacket packet) {
  WizMotePacket wizmote = WizMotePacket::build(packet);
  if (update_wizmote_history(&wizmote))
    return;

  this->on_button_->trigger(wizmote);
}

bool WizMoteListener::update_wizmote_history(WizMotePacket *packet)
{
  uint8_t index = 0;
  if (find_bssid_index(packet, &index))
  {
    if (this->history[index].sequence == packet->sequence)
      return true;
    
    // We had a matching MAC, but not a matching sequence, 
    this->history[index].sequence = packet->sequence;
  }
  
  for (uint8_t i = (WIZMOTEHISTORYSIZE - 1); i > 0; i--)
    memcpy(&(this->history[i]), &(this->history[i-1]), sizeof(WizMoteHistory));
  this->history[0].sequence = packet->sequence;
  memcpy(this->history[0].bssid, packet->bssid, 6);
  
  return false;
}

bool WizMoteListener::find_bssid_index(WizMotePacket *packet, uint8_t *index)
{
  for (uint8_t i = 0; i < WIZMOTEHISTORYSIZE; i++)
  {
    if (is_bssid_equal(packet, &(this->history[i])))
    {
      *index = i;
      return true;
    }
  }
  return false;
}

bool WizMoteListener::is_bssid_equal(WizMotePacket *packet, WizMoteHistory *history)
{
  for (uint8_t i = 0; i < 6; i++)
  {
    if (packet->bssid[i] != history->bssid[i])
      return false;
  }
  return true;
}
  
}  // namespace wizmote
}  // namespace esphome
