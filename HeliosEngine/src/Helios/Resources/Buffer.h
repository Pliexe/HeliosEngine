#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/DepricatedGraphics.h"

namespace Helios {

	enum class ShaderDataType
	{
		// Float 16-bit
		Float16, Float16_2, Float16_4, 
		// Float 32-bit
		Float32, Float32_2, Float32_3, Float32_4, 
		// Int 8-bit
		Int8, Int8_2, Int8_4, 
		// Int 16-bit
		Int16, Int16_2, Int16_4,
		// Int 32-bit
		Int32, Int32_2, Int32_3, Int32_4,
		// Unsigned Int 8-bit
		UInt8, UInt8_2, UInt8_4,
		// Unsigned Int 16-bit
		UInt16, UInt16_2, UInt16_4,
		// Unsigned Int 32-bit
		UInt32, UInt32_2, UInt32_3, UInt32_4,
		// Normalized Int 8-bit
		NormInt8, NormInt8_2, NormInt8_4,
		// Normalized Int 16-bit
		NormInt16, NormInt16_2, NormInt16_4,
		// Normalized unsigned Int 8-bit
		NormUInt8, NormUInt8_2, NormUInt8_4,
		// Normalized unsigned Int 16-bit
		NormUInt16, NormUInt16_2, NormUInt16_4,
		// Matrixs
		MatrixFloat2x2, MatrixFloat3x3, MatrixFloat4x4, 
		// Bool
		Bool,
		// Helper types
		MatrixFloat = ShaderDataType::MatrixFloat4x4,
	};

	static uint32_t ShaderDataTypeAlignment(ShaderDataType type)
	{
		switch (type)
		{
			// Bool (1 byte)
		case ShaderDataType::Bool: return 1;
			// 8-bit (1 byte)
		case ShaderDataType::Int8: return 1;
		case ShaderDataType::UInt8: return 1;
		case ShaderDataType::NormInt8: return 1;
		case ShaderDataType::NormUInt8: return 1;
			// 16-bit (2 bytes)
		case ShaderDataType::Int16: return 2;
		case ShaderDataType::Float16: return 2;
		case ShaderDataType::UInt16: return 2;
		case ShaderDataType::NormInt16: return 2;
		case ShaderDataType::NormUInt16: return 2;
			// 32-bit (4 bytes)
		case ShaderDataType::Int32: return 4;
		case ShaderDataType::Float32: return 4;
		case ShaderDataType::UInt32: return 4;
			// 8-bit x 2 (2 bytes) 1 * 2 = 2
		case ShaderDataType::Int8_2: return 2;
		case ShaderDataType::UInt8_2: return 2;
		case ShaderDataType::NormInt8_2: return 2;
		case ShaderDataType::NormUInt8_2: return 2;
			// 16-bit x 2 (4 bytes) 2 * 2 = 4
		case ShaderDataType::Int16_2: return 4;
		case ShaderDataType::UInt16_2: return 4;
		case ShaderDataType::NormInt16_2: return 4;
		case ShaderDataType::NormUInt16_2: return 4;
			// 32-bit x 2 (8 bytes) 4 * 2 = 8
		case ShaderDataType::Int32_2: return 8;
		case ShaderDataType::UInt32_2: return 8;
		case ShaderDataType::Float32_2: return 8;
			// 8-bit x 4 (2 bytes) 1 * 4 = 4
		case ShaderDataType::Int8_4: return 4;
		case ShaderDataType::UInt8_4: return 4;
		case ShaderDataType::NormInt8_4: return 4;
		case ShaderDataType::NormUInt8_4: return 4;
			// 16-bit x 4 (4 bytes) 2 * 4 = 8
		case ShaderDataType::Int16_4: return 8;
		case ShaderDataType::UInt16_4: return 8;
		case ShaderDataType::NormInt16_4: return 8;
		case ShaderDataType::NormUInt16_4: return 8;
			// 32-bit x 4 (8 bytes) 4 * 4 = 16
		case ShaderDataType::Int32_4: return 16;
		case ShaderDataType::UInt32_4: return 16;
			// Matrixs
		case ShaderDataType::MatrixFloat2x2: return 8;
		case ShaderDataType::MatrixFloat3x3: return 12;
		case ShaderDataType::MatrixFloat4x4: return 16;
		}
		HL_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			// Bool (1 byte)
			case ShaderDataType::Bool: return 1;
			// 8-bit (1 byte)
			case ShaderDataType::Int8: return 1;
			case ShaderDataType::UInt8: return 1;
			case ShaderDataType::NormInt8: return 1;
			case ShaderDataType::NormUInt8: return 1;
			// 16-bit (2 bytes)
			case ShaderDataType::Int16: return 2;
			case ShaderDataType::Float16: return 2;
			case ShaderDataType::UInt16: return 2;
			case ShaderDataType::NormInt16: return 2;
			case ShaderDataType::NormUInt16: return 2;
			// 32-bit (4 bytes)
			case ShaderDataType::Int32: return 4;
			case ShaderDataType::Float32: return 4;
			case ShaderDataType::UInt32: return 4;
			// 8-bit x 2 (2 bytes) 1 * 2 = 2
			case ShaderDataType::Int8_2: return 2;
			case ShaderDataType::UInt8_2: return 2;
			case ShaderDataType::NormInt8_2: return 2;
			case ShaderDataType::NormUInt8_2: return 2;
			// 16-bit x 2 (4 bytes) 2 * 2 = 4
			case ShaderDataType::Int16_2: return 4;
			case ShaderDataType::UInt16_2: return 4;
			case ShaderDataType::Float16_2: return 4;
			case ShaderDataType::NormInt16_2: return 4;
			case ShaderDataType::NormUInt16_2: return 4;
			// 32-bit x 2 (8 bytes) 4 * 2 = 8
			case ShaderDataType::Int32_2: return 8;
			case ShaderDataType::UInt32_2: return 8;
			case ShaderDataType::Float32_2: return 8;
			// 32-bit x 3 (12 bytes) 4 * 3 = 12
			case ShaderDataType::Int32_3: return 12;
			case ShaderDataType::UInt32_3: return 12;
			case ShaderDataType::Float32_3: return 12;
			// 8-bit x 4 (4 bytes) 1 * 4 = 4
			case ShaderDataType::Int8_4: return 4;
			case ShaderDataType::UInt8_4: return 4;
			case ShaderDataType::NormInt8_4: return 4;
			case ShaderDataType::NormUInt8_4: return 4;
			// 16-bit x 4 (8 bytes) 2 * 4 = 8
			case ShaderDataType::Int16_4: return 8;
			case ShaderDataType::UInt16_4: return 8;
			case ShaderDataType::Float16_4: return 8;
			case ShaderDataType::NormInt16_4: return 8;
			case ShaderDataType::NormUInt16_4: return 8;
			// 32-bit x 4 (16 bytes) 4 * 4 = 16
			case ShaderDataType::Int32_4: return 16;
			case ShaderDataType::UInt32_4: return 16;
			case ShaderDataType::Float32_4: return 16;
			// Matrix 2x2 (32-bit float) (16 bytes)
			case ShaderDataType::MatrixFloat2x2: return 2 * 2 * 4;
			// Matrix 3x3 (32-bit float) (36 bytes)
			case ShaderDataType::MatrixFloat3x3: return 3 * 3 * 4;
			// Matrix 4x4 (32-bit float) (64 bytes)
			case ShaderDataType::MatrixFloat4x4: return 4 * 4 * 4;
		}

		HL_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	// TODO: Make custom types for shader data types
	// I'm too lazy to do it right now, but I'll do it later
	// It should be fine for now since the platforms I'm building for mostly have 32-bit floats and etc

	struct InputElement
	{
		const char* name;
		ShaderDataType type;
	};

	enum class InputClassification
	{
		PerVertexData,
		PerInstanceData,
	};

	class InputLayout
	{
	public:
		InputLayout() = default;
		InputLayout(std::initializer_list<InputElement> elements)
		{
			m_elements = elements;
			CalculateStride();
		}

		const std::vector<InputElement>& GetElements() const { return m_elements; }

		// iterators
		std::vector<InputElement>::iterator begin() { return m_elements.begin(); }
		std::vector<InputElement>::iterator end() { return m_elements.end(); }
		std::vector<InputElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<InputElement>::const_iterator end() const { return m_elements.end(); }

		std::size_t size() const { return m_elements.size(); }
		const InputElement& operator[](std::size_t index) const { return m_elements[index]; }

		virtual ~InputLayout() = default;

		uint32_t GetStride() const { return m_Stride; }

	private:
		void CalculateStride() {
			m_Stride = 0;
			uint32_t align = 0;
			for (auto& e : m_elements) { 
				m_Stride += ShaderDataTypeSize(e.type);
				uint32_t a = ShaderDataTypeAlignment(e.type);
				if (a > align) align = a;
			}
			m_Stride = (m_Stride + align - 1) & ~(align - 1);
		}
	private:
		uint32_t m_Stride = 0;
		std::vector<InputElement> m_elements;
	};

	class InputLayouts
	{
	public:
		InputLayouts() = default;
		InputLayouts(std::initializer_list<InputLayout> layouts)
		{
			m_layouts = layouts;
		}

		// Add layout
		void AddLayout(InputLayout layout) { m_layouts.push_back(layout); }

		const std::vector<InputLayout>& GetLayouts() const { return m_layouts; }

		// iterators
		std::vector<InputLayout>::iterator begin() { return m_layouts.begin(); }
		std::vector<InputLayout>::iterator end() { return m_layouts.end(); }
		std::vector<InputLayout>::const_iterator begin() const { return m_layouts.begin(); }
		std::vector<InputLayout>::const_iterator end() const { return m_layouts.end(); }

		std::size_t size() const { return m_layouts.size(); }
		const InputLayout& operator[](std::size_t index) const { return m_layouts[index]; }

		template <int N>
		friend class SafeInputLayouts;
	private:
		std::vector<InputLayout> m_layouts;
	};

	template <int N>
	class SafeInputLayouts : public InputLayouts
	{
	public:
		SafeInputLayouts(std::array<InputLayout, N> arr)
		{
			std::vector<InputLayout> layouts(arr.begin(), arr.end());
			m_layouts = layouts;
		}
	};

	enum class BufferUsage
	{
		Static,
		Dynamic,
		Staging,
		Stream
	};

	class HELIOS_API UnsafeUniformBuffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data) = 0;

		static Ref<UnsafeUniformBuffer> Create(uint32_t binding_slot, uint32_t size);
		static Ref<UnsafeUniformBuffer> Create(uint32_t binding_slot, const void* data, uint32_t size);
	};

	template <class T>
	class HELIOS_API UniformBuffer
	{
	private:
		Ref<UnsafeUniformBuffer> m_buffer;
	public:

		//static_assert(std::is_trivially_copyable<T>::value, "UniformBuffer<T> must be trivially copyable!");
		static_assert(std::is_class<T>::value, "UniformBuffer<T> must be a struct!");

		void Bind() { m_buffer->Bind(); }
		void Unbind() { m_buffer->Unbind(); }

		void SetData(const T& data) { m_buffer->SetData((void*)&data); }
		void SetData(const T* data) { m_buffer->SetData((void*)data); }

		static Ref<UniformBuffer> Create(uint32_t binding_slot)
		{
			Ref<UniformBuffer> buff = CreateRef<UniformBuffer>();
			buff->m_buffer = UnsafeUniformBuffer::Create(binding_slot, sizeof(T));
			return buff;
			//return std::dynamic_pointer_cast<UniformBuffer>(UnsafeUniformBuffer::Create(binding_slot, sizeof(T)));
		}

		static Ref<UniformBuffer> Create(uint32_t binding_slot, const T& data)
		{
			Ref<UniformBuffer> buff = CreateRef<UniformBuffer>();
			buff->m_buffer = UnsafeUniformBuffer::Create(binding_slot, &data, sizeof(T));
			return buff;
			//return std::dynamic_pointer_cast<UniformBuffer>(UnsafeUniformBuffer::Create(binding_slot, &data, sizeof(T));
		}
	};

	class HELIOS_API UnsafeVertexBuffer
	{
	public:
		virtual void Bind(uint32_t slot = 0u) const = 0;
		virtual void Unbind(uint32_t slot = 0u) const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual void SetInputLayout(const InputLayout& layout) = 0;
		virtual const InputLayout& GetInputLayout() const = 0;

		static Ref<UnsafeVertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Static);
		static Ref<UnsafeVertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);

		//// cast back to template <class T> Ref<VertexBuffer<T>>
		//template <class T>
		//operator Ref<VertexBuffer<T>>() const { 
		//	Ref<VertexBuffer<T>> buff = CreateRef<VertexBuffer<T>>();
		//	buff->m_buffer = this;
		//	return buff;
		//}
		//
		//template <class T>
		//operator Ref<VertexBuffer<T>>() {
		//	Ref<VertexBuffer<T>> buff = CreateRef<VertexBuffer<T>>();
		//	buff->m_buffer = this;
		//	return buff;
		//}
	};

	template <class T>
	class HELIOS_API VertexBuffer
	{
	public:
		void Bind() { m_buffer->Bind(); }
		void Unbind() { m_buffer->Unbind(); }

		void SetData(const T* data, uint32_t count) { m_buffer->SetData((void*)data, sizeof(T) * count); }
		
		static Ref<VertexBuffer> Create(uint32_t count, BufferUsage usage = BufferUsage::Static)
		{
			Ref<VertexBuffer> buff = CreateRef<VertexBuffer>();
			buff->m_buffer = UnsafeVertexBuffer::Create(sizeof(T) * count, usage);
			return buff;
		}

		static Ref<VertexBuffer> Create(const T* data, uint32_t count, BufferUsage usage = BufferUsage::Static)
		{
			Ref<VertexBuffer> buff = CreateRef<VertexBuffer>();
			buff->m_buffer = UnsafeVertexBuffer::Create((void*)data, sizeof(T) * count, usage);
			return buff;
		}

		void SetInputLayout(const InputLayout& layout) { m_buffer->SetInputLayout(layout); }
		const Ref<InputLayout>& GetInputLayout() const { return m_buffer->GetInputLayout(); }

		// operator for casting to UnsafeVertexBuffer
		operator Ref<UnsafeVertexBuffer>& () { return m_buffer; }
		operator const Ref<UnsafeVertexBuffer>& () const { return m_buffer; }

		friend class UnsafeVertexBuffer;

	private:
		Ref<UnsafeVertexBuffer> m_buffer;
	};

	class HELIOS_API VertexArray
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& index_buffer) = 0;

		virtual const std::vector<Ref<UnsafeVertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const InputLayouts&  GetInputLayouts() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		struct BufferSpecification
		{
			const void* data;
			uint32_t size; // Total size of buffer in bytes
			BufferUsage usage;
		};

		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<BufferSpecification> bufferSpecifications);

		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<Ref<UnsafeVertexBuffer>> buffers);

		template <class T>
		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<Ref<VertexBuffer<T>>> buffers)
		{
			return Create(inputLayouts, (std::initializer_list<Ref<UnsafeVertexBuffer>>)buffers);
		}
	};

	class HELIOS_API IndexBuffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const uint32_t* data, uint32_t count) = 0;

		virtual uint32_t GetCount() const = 0;
		virtual std::string ToString() const = 0;

		static Ref<IndexBuffer> Create(std::initializer_list<uint32_t> indices, BufferUsage type = BufferUsage::Static);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count, BufferUsage type = BufferUsage::Static);
	};
}