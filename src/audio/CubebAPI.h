#pragma once

#include "IAudioAPI.h"

#include <cubeb/cubeb.h>

#include <memory>

class CubebAPI : public IAudioAPI
{
public:
	class CubebDeviceDescription : public DeviceDescription
	{
	public:
		CubebDeviceDescription(cubeb_devid devid, std::string device_id, const std::wstring& name)
			: DeviceDescription(name), m_devid(devid), m_device_id(std::move(device_id)) { }

		std::wstring GetIdentifier() const override { return  boost::nowide::widen(m_device_id); }
		cubeb_devid GetDeviceId() const { return m_devid; }

	private:
		cubeb_devid m_devid;
		std::string m_device_id;
	};

	using CubebDeviceDescriptionPtr = std::shared_ptr<CubebDeviceDescription>;

	CubebAPI(cubeb_devid devid, uint32 samplerate, uint32 channels, uint32 samples_per_block, uint32 bits_per_sample);
	~CubebAPI();

	AudioAPI GetType() const override { return Cubeb; }
	bool NeedAdditionalBlocks() const override;
	bool FeedBlock(sint16* data) override;
	bool Play() override;
	bool Stop() override;
	void SetVolume(sint32 volume) override;

	static std::vector<DeviceDescriptionPtr> GetDevices();

	static bool InitializeStatic();
	static void Destroy();

private:
	inline static bool s_com_initialized = false;
	inline static cubeb* s_context = nullptr;

	cubeb_stream* m_stream = nullptr;
	bool m_is_playing = false;

	mutable std::shared_mutex m_mutex;
	std::vector<uint8> m_buffer;
	static long data_cb(cubeb_stream* stream, void* user, const void* inputbuffer, void* outputbuffer, long nframes);
};
