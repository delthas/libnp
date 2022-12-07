#include "np.h"
#include "np_util.h"

#include <stdbool.h>
#include <stdlib.h>

#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-bus.h>
#elif defined(HAVE_LIBELOGIND)
#include <elogind/sd-bus.h>
#elif defined(HAVE_BASU)
#include <basu/sd-bus.h>
#endif

#include <stddef.h>
#include <string.h>

bool msg_read(sd_bus_message *m, char *type, void *v) {
	if (sd_bus_message_read(m, "v", type, v) < 0) {
		return false;
	}
	return true;
}

bool msg_read_i(sd_bus_message *m, int *i) {
	int32_t v;
	if (sd_bus_message_read(m, "v", "i", &v) < 0) {
		return false;
	}
	*i = v;
	return true;
}

bool msg_read_s(sd_bus_message *m, char **buf) {
	const char *b;
	if (sd_bus_message_read(m, "v", "s", &b) < 0) {
		return false;
	}
	if (b && *b) {
		*buf = strdup(b);
	}
	return true;
}

bool msg_read_as(sd_bus_message *m, char ***buf, size_t *size) {
	if (sd_bus_message_enter_container(m, 'v', "as") < 0) {
		return false;
	}
	if (sd_bus_message_enter_container(m, 'a', "s") < 0) {
		return false;
	}
	char **b = NULL;
	size_t i;
	for (i = 0;; ++i) {
		char *v;
		int r = sd_bus_message_read(m, "s", &v);
		if (r < 0) {
			goto exit;
		}
		if (r == 0) {
			break;
		}
		b = realloc(b, (i + 1) * sizeof(*b));
		b[i] = strdup(v);
	}
	if (sd_bus_message_exit_container(m) < 0) {
		goto exit;
	}
	if (sd_bus_message_exit_container(m) < 0) {
		goto exit;
	}
	if (b) {
		*buf = b;
		*size = i;
		b = NULL;
	}
exit:;
	if (b) {
		for (int j = 0; j < i; ++j) {
			free(b[j]);
		}
		free(b);
	}
	return b == NULL;
}

sd_bus *bus = NULL;

void np_init() {
	sd_bus_default_user(&bus);
}

void np_destroy() {
	if (bus) {
		sd_bus_unref(bus);
	}
}

struct np_info *np_info_get() {
	if (!bus) {
		return NULL;
	}

	struct np_info *r = NULL;

	char **names;
	if (sd_bus_list_names(bus, &names, NULL) < 0) {
		goto exit0;
	}

	const char *prefix = "org.mpris.MediaPlayer2.";
	for (size_t i = 0; names[i]; ++i) {
		const char *name = names[i];
		if (strncmp(prefix, name, strlen(prefix)) != 0) {
			continue;
		}
		char *status;
		if (sd_bus_get_property_string(bus, name, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", "PlaybackStatus", NULL, &status) < 0) {
			continue;
		}
		bool success = false;
		bool playing;
		if (!strcmp(status, "Playing")) {
			playing = true;
		} else if (!strcmp(status, "Paused")) {
			playing = false;
		} else {
			goto player0;
		}
		struct np_info *info = calloc(1, sizeof(struct np_info));

		sd_bus_message *metadata_msg = NULL;
		if (sd_bus_get_property(bus, name, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", "Metadata", NULL, &metadata_msg, "a{sv}") < 0) {
			goto player0;
		}

		if (sd_bus_message_enter_container(metadata_msg, 'a', "{sv}") < 0) {
			goto player1;
		}
		while (1) {
			int c = sd_bus_message_enter_container(metadata_msg, 'e', "sv");
			if (c < 0) {
				goto player1;
			}
			if (c == 0) {
				break;
			}
			const char *key = NULL;
			if (sd_bus_message_read(metadata_msg, "s", &key) < 0) {
				goto player1;
			}
			if (!strcmp(key, "mpris:length")) {
				if (!msg_read(metadata_msg, "t", &info->length)) {
					goto player1;
				}
			} else if (!strcmp(key, "mpris:artUrl")) {
				if (!msg_read_s(metadata_msg, &info->art_url)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:album")) {
				if (!msg_read_s(metadata_msg, &info->album)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:albumArtist")) {
				if (!msg_read_as(metadata_msg, &info->album_artists, &info->album_artists_size)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:artist")) {
				if (!msg_read_as(metadata_msg, &info->artists, &info->artists_size)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:audioBPM")) {
				if (!msg_read_i(metadata_msg, &info->bpm)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:composer")) {
				if (!msg_read_as(metadata_msg, &info->composers, &info->composers_size)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:contentCreated")) {
				if (!msg_read_s(metadata_msg, &info->created)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:discNumber")) {
				if (!msg_read_i(metadata_msg, &info->disc_number)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:firstUsed")) {
				if (!msg_read_s(metadata_msg, &info->first_played)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:genre")) {
				if (!msg_read_as(metadata_msg, &info->genres, &info->genres_size)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:lastUsed")) {
				if (!msg_read_s(metadata_msg, &info->last_played)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:lyricist")) {
				if (!msg_read_as(metadata_msg, &info->lyricists, &info->lyricists_size)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:title")) {
				if (!msg_read_s(metadata_msg, &info->title)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:trackNumber")) {
				if (!msg_read_i(metadata_msg, &info->track_number)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:url")) {
				if (!msg_read_s(metadata_msg, &info->url)) {
					goto player1;
				}
			} else if (!strcmp(key, "xesam:useCount")) {
				if (!msg_read_i(metadata_msg, &info->play_count)) {
					goto player1;
				}
			} else {
				if (sd_bus_message_skip(metadata_msg, "v") < 0) {
					goto player1;
				}
			}
			if (sd_bus_message_exit_container(metadata_msg) < 0) {
				goto player1;
			}
		}
		success = true;
	player1:;
		if (success) {
			if (r) {
				np_info_destroy(r);
			}
			r = info;
		} else {
			np_info_destroy(info);
		}
		sd_bus_message_unref(metadata_msg);
	player0:;
		free(status);

		if (success && playing) {
			break;
		}
	}

	for (size_t i = 0; names[i]; ++i) {
		free(names[i]);
	}
	free(names);
exit0:;
	return r;
}

void np_info_destroy(struct np_info *info) {
	free_info(info);
}
