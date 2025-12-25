#pragma once

#include "Helios/Core/Exceptions.h"
#include "pch.h"
#include "Helios/Core/Asserts.h"
#include <cstring>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>

namespace Helios {

	const std::uint32_t MAX_BUFFER_SIZE = -1u;

	enum class ShaderDataType
	{
		Unknown,
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
#pragma region 8-bit (1 byte) types
// SInt
		case ShaderDataType::Int8:
		case ShaderDataType::Int8_2:
		case ShaderDataType::Int8_4:
// UInt
		case ShaderDataType::UInt8:
		case ShaderDataType::UInt8_2:
		case ShaderDataType::UInt8_4:
// NSInt
		case ShaderDataType::NormInt8:
		case ShaderDataType::NormInt8_2:
		case ShaderDataType::NormInt8_4:
// NUInt
		case ShaderDataType::NormUInt8:
		case ShaderDataType::NormUInt8_2:
		case ShaderDataType::NormUInt8_4:
// Bool (1 byte)
		case ShaderDataType::Bool: return 1u;
#pragma endregion
#pragma region 16-bit (2 bytes) types
// SInt
		case ShaderDataType::Int16:
		case ShaderDataType::Int16_2:
		case ShaderDataType::Int16_4:
// UInt
		case ShaderDataType::UInt16:
		case ShaderDataType::UInt16_2:
		case ShaderDataType::UInt16_4:
// NSInt
 		case ShaderDataType::NormInt16:
		case ShaderDataType::NormInt16_2:
		case ShaderDataType::NormInt16_4:
// NUInt
		case ShaderDataType::NormUInt16:
		case ShaderDataType::NormUInt16_2:
		case ShaderDataType::NormUInt16_4:
// Float
		case ShaderDataType::Float16_2:
		case ShaderDataType::Float16_4:
		case ShaderDataType::Float16: return 2u;
#pragma endregion
#pragma region 32-bit (4 bytes) types
// SInt
		case ShaderDataType::Int32:
		case ShaderDataType::Int32_2:
		case ShaderDataType::Int32_3:
		case ShaderDataType::Int32_4:
// UInt
		case ShaderDataType::UInt32:
		case ShaderDataType::UInt32_2:
		case ShaderDataType::UInt32_3:
		case ShaderDataType::UInt32_4:
// Float
		case ShaderDataType::Float32_2:
		case ShaderDataType::Float32_3:
		case ShaderDataType::Float32_4:
		case ShaderDataType::Float32: return 4u;
#pragma endregion

// Matrixs
		case ShaderDataType::MatrixFloat2x2: return 8;
		case ShaderDataType::MatrixFloat3x3: return 12;
		case ShaderDataType::MatrixFloat4x4: return 16;
		case ShaderDataType::Unknown: HL_EXCEPTION(true, "Unknown ShaderDataType!");
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

			case ShaderDataType::Unknown: HL_EXCEPTION(true, "Unknown ShaderDataType!");
		}

		HL_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}


	static constexpr std::string_view StringFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
#pragma region 8-bit (1 byte) types
// SInt
		case ShaderDataType::Int8: return "Int8";
		case ShaderDataType::Int8_2: return "Int8_2";
		case ShaderDataType::Int8_4: return "Int8_4";
// UInt
		case ShaderDataType::UInt8: return "UInt8";
		case ShaderDataType::UInt8_2: return "UInt8_2";
		case ShaderDataType::UInt8_4: return "UInt8_4";
// NSInt
		case ShaderDataType::NormInt8: return "NormInt8";
		case ShaderDataType::NormInt8_2: return "NormInt8_2";
		case ShaderDataType::NormInt8_4: return "NormInt8_4";
// NUInt
		case ShaderDataType::NormUInt8: return "NormUInt8";
		case ShaderDataType::NormUInt8_2: return "NormUInt8_2";
		case ShaderDataType::NormUInt8_4: return "NormUInt8_4";
// Bool (1 byte)
		case ShaderDataType::Bool: return "Bool";
#pragma endregion
#pragma region 16-bit (2 bytes) types
// SInt
		case ShaderDataType::Int16: return "Int16";
		case ShaderDataType::Int16_2: return "Int16_2";
		case ShaderDataType::Int16_4: return "Int16_4";
// UInt
		case ShaderDataType::UInt16: return "UInt16";
		case ShaderDataType::UInt16_2: return "UInt16_2";
		case ShaderDataType::UInt16_4: return "UInt16_4";
// NSInt
 		case ShaderDataType::NormInt16: return "NormInt16";
		case ShaderDataType::NormInt16_2: return "NormInt16_2";
		case ShaderDataType::NormInt16_4: return "NormInt16_4";
// NUInt
		case ShaderDataType::NormUInt16: return "NormUInt16";
		case ShaderDataType::NormUInt16_2: return "NormUInt16_2";
		case ShaderDataType::NormUInt16_4: return "NormUInt16_4";
// Float
		case ShaderDataType::Float16_2: return "Float16_2";
		case ShaderDataType::Float16_4: return "Float16_4";
		case ShaderDataType::Float16: return "Float16";
#pragma endregion
#pragma region 32-bit (4 bytes) types
// SInt
		case ShaderDataType::Int32: return "Int32";
		case ShaderDataType::Int32_2: return "Int32_2";
		case ShaderDataType::Int32_3: return "Int32_3";
		case ShaderDataType::Int32_4: return "Int32_4";
// UInt
		case ShaderDataType::UInt32: return "UInt32";
		case ShaderDataType::UInt32_2: return "UInt32_2";
		case ShaderDataType::UInt32_3: return "UInt32_3";
		case ShaderDataType::UInt32_4: return "UInt32_4";
// Float
		case ShaderDataType::Float32_2: return "Float32_2";
		case ShaderDataType::Float32_3: return "Float32_3";
		case ShaderDataType::Float32_4: return "Float32_4";
		case ShaderDataType::Float32:; return ":";
#pragma endregion

// Matrixs
		case ShaderDataType::MatrixFloat2x2: return "MatrixFloat2x2";
		case ShaderDataType::MatrixFloat3x3: return "MatrixFloat3x3";
		case ShaderDataType::MatrixFloat4x4: return "MatrixFloat4x4";
		case ShaderDataType::Unknown: return "Unknown";
		}
		HL_ASSERT(false, "Unknown ShaderDataType!");
		return "Unknown";
	}

	// TODO: Make custom types for shader data types
	// I'm too lazy to do it right now, but I'll do it later
	// It should be fine for now since the platforms I'm building for mostly have 32-bit floats and etc

	struct InputElement
	{
		std::string_view name;
		ShaderDataType type;

		constexpr bool operator==(const InputElement&) const = default;
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
		InputLayout(std::initializer_list<InputElement> elements, InputClassification inputClassification = InputClassification::PerVertexData)
		{
			m_elements = elements;
			m_inputClassification = inputClassification;
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
		InputClassification GetInputClassification() const { return m_inputClassification; }

		bool operator==(const InputLayout&) const = default; // Items are trivially copyable, should work.

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
		InputClassification m_inputClassification;
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

		bool operator==(const InputLayouts& other) const { return m_layouts == other.m_layouts; }
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

	struct IWriteableBuffer
	{
		virtual ~IWriteableBuffer() = default;
		virtual void* Lock(size_t size, size_t offset = 0) = 0;
		virtual void Unlock() = 0;

		virtual bool IsLocked() const = 0;

		virtual size_t GetLockOffset() const = 0;
		virtual size_t GetLockSize() const = 0;

		// Helpers
		template <class T>
		T* Lock(size_t offset = 0) { 
			// static_assert(std::is_trivially_copyable_v<T>, "IWriteableBuffer::Lock<T> requires a trivially copyable type");
			return static_cast<T*>(Lock(sizeof(T), offset));
		}

		template <class T>
		std::span<T> LockSpan(size_t count, size_t offset = 0) { 
			// static_assert(std::is_trivially_copyable_v<T>, "IWriteableBuffer::LockSpan<T> requires a trivially copyable type");
			return std::span<T>(static_cast<T*>(Lock(count * sizeof(T), offset)), count); 
		}
	};

	template <typename DataType = void*, bool IsSpan = false>
	struct WriteGuard
	{
		WriteGuard(Ref<IWriteableBuffer> ptr, auto&&... args) requires (!IsSpan) : m_ptr(ptr)
		{
			if constexpr (std::is_same_v<DataType, void*>)
				m_data = m_ptr->Lock(args...);
			else
				m_data = m_ptr->template Lock<DataType>(args...);
		}

		WriteGuard(Ref<IWriteableBuffer> ptr, size_t count, size_t offset = 0) requires (IsSpan) : m_ptr(ptr)
		{
			static_assert(!std::is_same_v<DataType, void*>, "WriteGuard<DataType, true> requires DataType to not be void*");
			m_data = m_ptr->template LockSpan<DataType>(count, offset);
		}

		template <typename Buffer, typename = std::enable_if_t<
			std::is_convertible_v<
				decltype(std::declval<Buffer>()->GetRaw()), 
				Ref<IWriteableBuffer>
			>>
		>
		WriteGuard(Buffer bufferWrapper, size_t size, size_t offset = 0) : WriteGuard(bufferWrapper->GetRaw(), size, offset) {}


		~WriteGuard() { 
			m_ptr->Unlock();
		}

		// if raw ptr

		void* data() requires(std::is_same_v<DataType, void*>) { return m_data; }

		// If ptr
		DataType* operator->() requires (!IsSpan && !std::is_same_v<DataType, void*>) { return m_data; }
		DataType& operator*() requires (!IsSpan && !std::is_same_v<DataType, void*>) { return *m_data; }
		DataType* operator->() const requires (!IsSpan && !std::is_same_v<DataType, void*>) { return m_data; }
		DataType& operator*() const requires (!IsSpan && !std::is_same_v<DataType, void*>) { return *m_data; }

		operator DataType*() requires (!IsSpan && !std::is_same_v<DataType, void*>) { return m_data; }
		operator DataType*() const requires (!IsSpan && !std::is_same_v<DataType, void*>) { return m_data; }

		// If span
		std::span<DataType> span() requires (IsSpan) { return m_data; }
		std::span<DataType> span() const requires (IsSpan) { return m_data; }

		size_t size() const requires (IsSpan) { return m_data.size(); }

		operator std::span<DataType>() requires (IsSpan) { return m_data; }
		operator std::span<DataType>() const requires (IsSpan) { return m_data; }

		DataType& operator[](size_t index) requires (IsSpan) { return m_data[index]; }
		DataType& operator[](size_t index) const requires (IsSpan) { return m_data[index]; }

	private:
		std::conditional_t<IsSpan, std::span<DataType>, std::conditional_t<std::is_same_v<DataType, void*>, void*, DataType*>> m_data;
		Ref<IWriteableBuffer> m_ptr;
	};
	

	// NVM I"LL NOT DO THIS BUT LEAVE IT FOR THE FUTURE IF I NEED IT
	// struct RawBufferWriter
	// {
	// 	virtual ~RawBufferWriter() = default;
	// 	virtual void* Lock() = 0;
	// 	virtual void Unlock() = 0;

	// 	virtual bool IsLocked() const = 0;

	// 	virtual size_t GetBlockOffset() const = 0;
	// 	virtual size_t GetBlockSize() const = 0;
	// };

	// template <class T>
	// struct BufferWriter : public RawBufferWriter
	// {
	// 	virtual ~BufferWriter() = default;
	// 	virtual T* Lock() override { return static_cast<T*>(RawBufferWriter::Lock()); }
	// 	virtual void Unlock() override { RawBufferWriter::Unlock(); }
	// };

	// template <class T>
	// struct ArrayBufferWriter
	// {
	// 	~ArrayBufferWriter() = default;
	// 	std::span<T> Lock() { return std::span<T>(m_writer.Lock(), GetCount()); }
	// 	void Unlock() { m_writer.Unlock(); }

	// 	bool IsLocked() const { return m_writer.IsLocked(); }

	// 	size_t GetBlockOffset() const { return m_writer.GetBlockOffset(); }
	// 	size_t GetBlockSize() const { return m_writer.GetBlockSize(); }

	// 	size_t GetCount() const { return m_writer.GetBlockSize() / sizeof(T); }

	// private:
	// 	BufferWriter<T> m_writer;
	// };

	enum class BufferUsage
	{
		Static,
		Dynamic,
		Staging,
		Stream
	};

	class HELIOS_API UnsafeUniformBuffer : public IWriteableBuffer
	{
	public:
        virtual ~UnsafeUniformBuffer() = default;

		virtual void SetData(const void* data) = 0;

		virtual std::size_t GetSize() = 0;

		static Ref<UnsafeUniformBuffer> Create(std::uint32_t size);
		static Ref<UnsafeUniformBuffer> Create(const void* data, std::uint32_t size);
	};

	template <class T>
	class HELIOS_API UniformBuffer
	{
	private:
		Ref<UnsafeUniformBuffer> m_buffer;

        friend Ref<UniformBuffer<T>> RequestUniformBuffer();
	public:

		//static_assert(std::is_trivially_copyable<T>::value, "UniformBuffer<T> must be trivially copyable!");
		static_assert(std::is_class<T>::value, "UniformBuffer<T> must be a struct!");

		void SetData(const T& data) { m_buffer->SetData((void*)&data); }
		void SetData(const T* data) { m_buffer->SetData((void*)data); }

		T* Lock(size_t offset = 0) { return m_buffer->Lock<T>(offset); }
		std::span<T> LockSpan(size_t count, size_t offset = 0) { return m_buffer->LockSpan<T>(count, offset); }

		template <bool IsSpan = false>
		WriteGuard<T, IsSpan> LockGuard(size_t offsetCount = 0) requires(!IsSpan)
		{
			return WriteGuard<T, IsSpan>(std::static_pointer_cast<IWriteableBuffer>(m_buffer), offsetCount * sizeof(T));
		}

		template <bool IsSpan = false>
		WriteGuard<T, IsSpan> LockGuard(size_t count, size_t offsetCount = 0) requires(IsSpan)
		{
			return WriteGuard<T, IsSpan>(std::static_pointer_cast<IWriteableBuffer>(m_buffer), count, offsetCount * sizeof(T));
		}

		void Unlock() { m_buffer->Unlock(); }

		static Ref<UniformBuffer> Create()
		{
			Ref<UniformBuffer> buff = CreateRef<UniformBuffer>();
			buff->m_buffer = UnsafeUniformBuffer::Create(sizeof(T));
			return buff;
			//return std::dynamic_pointer_cast<UniformBuffer>(UnsafeUniformBuffer::Create(binding_slot, sizeof(T)));
		}

		static Ref<UniformBuffer> Create(const T& data)
		{
			Ref<UniformBuffer> buff = CreateRef<UniformBuffer>();
			buff->m_buffer = UnsafeUniformBuffer::Create(&data, sizeof(T));
			return buff;
			//return std::dynamic_pointer_cast<UniformBuffer>(UnsafeUniformBuffer::Create(binding_slot, &data, sizeof(T));
		}

		operator Ref<UnsafeUniformBuffer> () { return m_buffer; }
		operator const Ref<UnsafeUniformBuffer> () const { return m_buffer; }

		Ref<UnsafeUniformBuffer> GetRaw() { return m_buffer; }
	};

	class HELIOS_API UnsafeVertexBuffer : public IWriteableBuffer
	{
	public:
		virtual ~UnsafeVertexBuffer() = default;
		virtual void SetData(const void* data, uint32_t size) = 0;
		virtual uint32_t GetSize() const = 0;

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

		void SetData(const T* data, uint32_t count) { m_count = count; m_buffer->SetData((void*)data, sizeof(T) * count); }
		
		static Ref<VertexBuffer> Create(uint32_t count, BufferUsage usage = BufferUsage::Static)
		{
			Ref<VertexBuffer> buff = CreateRef<VertexBuffer>();
			buff->m_count = count;
			buff->m_buffer = UnsafeVertexBuffer::Create(sizeof(T) * count, usage);
			return buff;
		}

		static Ref<VertexBuffer> Create(const T* data, uint32_t count, BufferUsage usage = BufferUsage::Static)
		{
			Ref<VertexBuffer> buff = CreateRef<VertexBuffer>();
			buff->m_count = count;
			buff->m_buffer = UnsafeVertexBuffer::Create((void*)data, sizeof(T) * count, usage);
			return buff;
		}

		uint32_t GetCount() const { return m_count; }

		Ref<UnsafeVertexBuffer> GetRaw() const { return m_buffer; }

		// operator for casting to UnsafeVertexBuffer
		operator Ref<UnsafeVertexBuffer>& () { return m_buffer; }
		operator const Ref<UnsafeVertexBuffer>& () const { return m_buffer; }

		friend class UnsafeVertexBuffer;

	private:
		Ref<UnsafeVertexBuffer> m_buffer;
		uint32_t m_count;
	};

	class HELIOS_API IndexBuffer
	{
	public:
		virtual void SetData(const uint32_t* data, uint32_t count) = 0;

		virtual uint32 GetCount() const = 0;
		virtual std::string ToString() const = 0;

		static Ref<IndexBuffer> Create(std::initializer_list<uint32_t> indices, BufferUsage usage = BufferUsage::Static);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count, BufferUsage usage = BufferUsage::Static);
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
		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<BufferSpecification> bufferSpecifications, Ref<UnsafeVertexBuffer> instanceBuffer);
		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<Ref<UnsafeVertexBuffer>> buffers);

		template <class T>
		static Ref<VertexArray> Create(const InputLayouts& inputLayouts, std::initializer_list<Ref<VertexBuffer<T>>> buffers)
		{
			return Create(inputLayouts, (std::initializer_list<Ref<UnsafeVertexBuffer>>)buffers);
		}
	};

	
}