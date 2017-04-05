#ifndef IEEE802_1X_H
#define IEEE802_1X_H

struct bss_data;
struct sta_info;
struct eapol_state_machine;


void ieee802_1x_receive(struct bss_data *hapd, struct sta_info *sta, const uint8_t *buf,
			size_t len);
void ieee802_1x_free_station(struct sta_info *sta);

int ieee802_1x_init(struct bss_data *hapd);
void ieee802_1x_deinit(struct bss_data *hapd);
uint8_t * ieee802_1x_get_identity(struct eapol_state_machine *sm, size_t *len);
uint8_t * ieee802_1x_get_radius_class(struct eapol_state_machine *sm, size_t *len,
				 int idx);

#endif /* IEEE802_1X_H */
