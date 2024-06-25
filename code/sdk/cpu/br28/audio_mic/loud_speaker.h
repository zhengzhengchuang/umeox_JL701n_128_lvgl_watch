
#ifndef _AUDIO_SPEAKER_API_H_
#define _AUDIO_SPEAKER_API_H_

void stop_loud_speaker(void);
void start_loud_speaker(struct audio_fmt *fmt);
int speaker_if_working(void);
void loud_speaker_pause(void);
void loud_speaker_resume(void);

void switch_holwing_en(void);
void switch_echo_en(void);
void loundspeaker_set_echo_delay(u32 delay);
void loundspeaker_set_echo_decay(u32 decay);
void loundspeaker_set_pitch_para(u32 shiftv, u32 sr, u8 effect, u32 formant_shift);
#endif

