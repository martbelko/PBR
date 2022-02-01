#pragma once

#include "Core/Base.h"

#include <vector>

enum class FramebufferTextureFormat
{
	None = 0,

	// Color
	RGBA8,
	RED_INTEGER,

	// Depth/stencil
	DEPTH24STENCIL8,

	// Custom
	Float32,
	Vec3,
	Vec4,

	// Defaults
	Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat format)
		: textureFormat(format) {}

	FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
		: attachments(attachments) {}

	std::vector<FramebufferTextureSpecification> attachments;
};

struct FramebufferSpecification
{
	uint32_t width = 0, height = 0;
	FramebufferAttachmentSpecification attachments;
	uint32_t samples = 1;
};

class Framebuffer
{
public:
	Framebuffer(const FramebufferSpecification& specification);
	~Framebuffer();
public:
	void Bind();
	void Unbind();

	void Invalidate();

	void Resize(uint32_t width, uint32_t height);

	int ReadPixel(uint32_t attachmentIndex, int x, int y);

	void ClearAttachment(uint32_t attachmentIndex, int value);

	const FramebufferSpecification& GetSpecification() const { return m_Specification; }
private:
	uint32_t m_RendererID = 0;
	FramebufferSpecification m_Specification;

	std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
	FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment = 0;
};
