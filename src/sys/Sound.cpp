#include "Pch.h"
#include "Common.h"

struct Sound
{
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice* voice;
	uint8_t* data;

	Sound() : wfx(), buffer(), voice(), data() { }
	~Sound() { delete [] data; }
};

IXAudio2* gXAudio;
Sound gSound[kSid_Max];

// riff loader

#define RIFF_FOURCC_RIFF	MAKEFOURCC('R', 'I', 'F', 'F')
#define RIFF_FOURCC_DATA	MAKEFOURCC('d', 'a', 't', 'a')
#define RIFF_FOURCC_FMT		MAKEFOURCC('f', 'm', 't', ' ')
#define RIFF_FOURCC_WAVE	MAKEFOURCC('W', 'A', 'V', 'E')

struct riff_chunk {
	const uint8_t* data;
	uint32_t size;

	riff_chunk() : data(), size() { }
	riff_chunk(const uint8_t* data_, uint32_t size_) : data(data_), size(size_) { }
	operator bool() const { return data != 0; }
};

riff_chunk riff_find_chunk(const uint8_t* riff_data, uint32_t riff_size, DWORD fourcc) {
	uint32_t offset = 4;

	while(offset < riff_size) {
		uint32_t code = *(const uint32_t*)(riff_data + offset);
		uint32_t size = *(const uint32_t*)(riff_data + offset + 4);

		if (code == fourcc)
			return riff_chunk(riff_data + offset + 8, size);

		offset += 8 + size;
	}

	return riff_chunk();
}

bool load_riff(IXAudio2* xAudio, Sound* sound, const char* path) {
	file_buf fb;

	if (!load_file(&fb, path))
		return false;

	uint32_t* p = (uint32_t*)fb.data;

	if (fb.size <= 12 || p[0] != RIFF_FOURCC_RIFF || p[2] != RIFF_FOURCC_WAVE)
		return false;

	uint32_t riff_size = p[1];

	if (riff_size > ((uint32_t)fb.size - 8))
		return false;

	riff_chunk ch_fmt = riff_find_chunk(fb.data + 8, riff_size, RIFF_FOURCC_FMT);
	riff_chunk ch_data = riff_find_chunk(fb.data + 8, riff_size, RIFF_FOURCC_DATA);

	if (!ch_fmt || !ch_data || ch_fmt.size < 16 || ch_data.size > riff_size)
		return false;

	if ((sound->data = new uint8_t[ch_data.size]) == 0)
		return false;

	memcpy(&sound->wfx, ch_fmt.data, min(sizeof(sound->wfx), ch_fmt.size));
	memcpy(sound->data, ch_data.data, ch_data.size);

	sound->buffer.AudioBytes	= ch_data.size;
	sound->buffer.pAudioData	= sound->data;
	sound->buffer.Flags			= XAUDIO2_END_OF_STREAM;

	if (FAILED(xAudio->CreateSourceVoice(&sound->voice, (WAVEFORMATEX*)&sound->wfx))) {
		delete [] sound->data;
		sound->data = 0;
		return false;
	}

	return true;
}

// sound sys

void SoundInit()
{
	ZeroMemory(gSound, sizeof(gSound));

	if (FAILED(XAudio2Create(&gXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		DebugLn("XAudioCreate failed");
		return;
	}

	IXAudio2MasteringVoice* masterVoice = 0;

	if (FAILED(gXAudio->CreateMasteringVoice(&masterVoice)))
	{
		DebugLn("CreateMasterVoice failed");
		return;
	}

	load_riff(gXAudio, &gSound[kSid_Dit],		"data\\dit.wav");
	load_riff(gXAudio, &gSound[kSid_Buzz],		"data\\buzz.wav");

	load_riff(gXAudio, &gSound[kSid_GemCollect],	"data\\gem_collect.wav");
	load_riff(gXAudio, &gSound[kSid_BugDies],		"data\\bug_dies.wav");
	load_riff(gXAudio, &gSound[kSid_BugHurt],		"data\\bug_hurt.wav");
	load_riff(gXAudio, &gSound[kSid_TurretDies],	"data\\turret_dies.wav");
	load_riff(gXAudio, &gSound[kSid_TurretFire],	"data\\turret_fire.wav");
	load_riff(gXAudio, &gSound[kSid_TurretHurt],	"data\\turret_hurt.wav");
	load_riff(gXAudio, &gSound[kSid_TurretPlace],	"data\\turret_place.wav");
	load_riff(gXAudio, &gSound[kSid_PlayerHurt],	"data\\player_hurt.wav");
}

void SoundShutdown()
{
	if (!gXAudio)
		return;

	for(int i = 0; i < kSid_Max; i++)
	{
		Sound& sound = gSound[i];

		if (sound.voice)
		{
			sound.voice->Stop();
			sound.voice->FlushSourceBuffers();
			sound.voice->DestroyVoice();
			sound.voice = 0;
		}

		delete [] sound.data;
		sound.data = 0;
	}

	gXAudio->Release();
}

void SoundPlay(SoundId sid, float freq, float volume)
{
	if (!gXAudio)
		return;

	Sound& sound = gSound[sid];

	if (sound.voice) {
		sound.voice->Stop();
		sound.voice->FlushSourceBuffers();
		sound.voice->SubmitSourceBuffer(&sound.buffer);
		sound.voice->SetFrequencyRatio(freq);
		sound.voice->SetVolume(volume);
		sound.voice->Start();
	}
}