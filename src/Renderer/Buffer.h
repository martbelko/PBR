#pragma once

#include <cassert>
#include <cstdint>

#include <string>
#include <vector>

#include "Base.h"

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat3:     return 4 * 3 * 3;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Int:      return 4;
	case ShaderDataType::Int2:     return 4 * 2;
	case ShaderDataType::Int3:     return 4 * 3;
	case ShaderDataType::Int4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	assert(false, "Unknown ShaderDataType!");
	return 0;
}

struct BufferElement
{
	ShaderDataType type;
	uint32_t size;
	size_t offset;
	bool normalized;

	BufferElement() = default;

	BufferElement(ShaderDataType type, bool normalized = false)
		: type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
	{
	}

	uint32_t getComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3; // 3* float3
		case ShaderDataType::Mat4:    return 4; // 4* float4
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 1;
		}

		assert(false, "Unknown ShaderDataType!");
		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(std::initializer_list<BufferElement> elements)
		: mElements(elements)
	{
		CalculateOffsetsAndStride();
	}

	uint32_t GetStride() const { return mStride; }
	const std::vector<BufferElement>& GetElements() const { return mElements; }

	std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
	std::vector<BufferElement>::iterator end() { return mElements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		mStride = 0;
		for (auto& element : mElements)
		{
			element.offset = offset;
			offset += element.size;
			mStride += element.size;
		}
	}
private:
	std::vector<BufferElement> mElements;
	uint32_t mStride = 0;
};

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size);
	VertexBuffer(const float* vertices, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	static void Unbind();

	void SetData(const void* data, uint32_t size);

	const BufferLayout& GetLayout() const { return mLayout; }
	void SetLayout(const BufferLayout& layout) { mLayout = layout; }
public:
	static Ref<VertexBuffer> Create(uint32_t size) { return CreateRef<VertexBuffer>(size); }
	static Ref<VertexBuffer> Create(const float* vertices, uint32_t size) { return CreateRef<VertexBuffer>(vertices, size); }
private:
	uint32_t mRendererID;
	BufferLayout mLayout;
};

class IndexBuffer
{
public:
	IndexBuffer(const uint32_t* indices, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	static void Unbind();

	uint32_t GetCount() const { return mCount; }
public:
	static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count) { return CreateRef<IndexBuffer>(indices, count); }
private:
	uint32_t mRendererID;
	uint32_t mCount;
};
