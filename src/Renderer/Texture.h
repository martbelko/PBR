#pragma once

#include <cstdint>
#include <string>

class Texture
{
public:
	Texture(uint32_t width, uint32_t height);
	Texture(const std::string& path);
	~Texture();

	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	uint32_t GetRendererID() const { return mRendererID; }

	void SetData(void* data, uint32_t size);

	void Bind(uint32_t slot = 0) const;

	bool IsLoaded() const { return mIsLoaded; }

	bool operator==(const Texture& other)
	{
		return mRendererID == other.mRendererID;
	}
private:
	std::string mPath;
	bool mIsLoaded = false;
	uint32_t mWidth, mHeight;
	uint32_t mRendererID;
	unsigned int mInternalFormat, mDataFormat;
};