#pragma once

#include "Helios/Graphics/RenderGraph/RenderPass.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
    inline RenderPassCallback Test()
    {
        return [] (RenderContext& context)
        {
            for (auto& view : context.views)
            {
                if (view.view.family == RenderView::Family::Scene)
                {
                    auto& allocator = Graphics::GetMainDevice()->GetAllocatorForThread();
                    auto cmdList = allocator.CreateCommandList();

                    auto m_colorImage = context.resources.RequestImage(view.view.size.width, view.view.size.height, 0, 0, Image::Format::R8G8B8A8_UNORM, Image::Usage::ColorAttachment);

                    RenderPassBeginInfo passInfo;
                    passInfo.attachmentSpecifications.push_back(RenderPassBeginInfo::AttachmentInfo { .buffer = m_colorImage, .clearColor = Color::Coral });
                    cmdList->BeginRenderPass(passInfo);

                    static auto il = InputLayouts { InputLayout {
                            { "Position", ShaderDataType::Float32_3 },
                            { "TexCoord", ShaderDataType::Float32_2 },
                            // { "Normal", ShaderDataType::Float32_3 }
                        } 
                    };
                    
                    static auto mainmd = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
                        .family = ShaderModule::Family::Main,
                        .stage = ShaderModule::Stage::Pixel | ShaderModule::Stage::Vertex,
                        .storageType = ShaderModule::StorageType::Code,
                        .path = "RenderPass/Example.hsl",
                        .inputLayout = il
                    });

                    static auto matmd = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
                        .family = ShaderModule::Family::Surface,
                        .stage = ShaderModule::Stage::Vertex | ShaderModule::Stage::Pixel,
                        .storageType = ShaderModule::StorageType::Code,
                        .tag = "MaterialPixelShader",
                        .path = "Material/StandardSurface.hsl",
                    });

                    static auto matmdalt = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
                        .family = ShaderModule::Family::Surface,
                        .stage = ShaderModule::Stage::Vertex | ShaderModule::Stage::Pixel,
                        .storageType = ShaderModule::StorageType::Code,
                        .tag = "MaterialPixelShader",
                        .path = "Material/ErrorMaterial.hsl",
                    });

                    struct Vert
                    {
                        Vector3 pos;
                        Vector2 uv;
                    };

                    Vert trig[3] = {
                        {{ 0.5f, -0.5f, 0.f },   {1.0f, 0.0f}},  // right-bottom -> UV (1,0)
                        {{ -0.5f, 0.5f, 0.f },   {0.0f, 1.0f}},  // left-top    -> UV (0,1)
                        {{ -0.5f, -0.5f, 0.f },  {0.0f, 0.0f}},  // left-bottom -> UV (0,0)
                        // { 0.5f, 0.5, 0.f },
                        // { 0.5f, 0.5, 0.f },
                        // { 0.5f, 0.5, 0.f },
                    };

                    std::uint32_t indecies[3] = {
                        0, 1, 2
                    };

                    
                    struct Buff
                    {
                        Color color;
                    };

                    Buff data;
                    data.color = Color::Green;
                    
                    static auto vertBuff = VertexBuffer<Vert>::Create(trig, 3, BufferUsage::Static);
                    static auto indexBuff = IndexBuffer::Create(indecies, std::size(indecies), BufferUsage::Static);
                    static auto uniBuff = UniformBuffer<Buff>::Create();

                    // uniBuff->SetData(data);

                    auto mappedData = uniBuff->Lock();
                    mappedData->color = Color::Red;
                    uniBuff->Unlock();


                    cmdList->BindPipeline({
                        .vertexShaderModules = { mainmd },
                        .pixelShaderModules = { mainmd, matmd }
                    });

                    cmdList->BindVertexBuffers({ *vertBuff });
                    cmdList->BindIndexBuffer(indexBuff, 0);

                    // cmdList.BindUniformBuffer(matmd, 0, *uniBuff);
                    // cmdList.BindTexture2D(matmd, 1,Graphics::GetMainDevice()->GetDummyTexture2D());

                    cmdList->BindSampler(matmd, 1, Sampler::GetOrCreate(SamplerConfiguration {}));
                    // cmdList->BindTexture2D(matmd, 1, HeliosEditor::ICON_FOLDER);

                    // cmdList.Draw(3, 0);


                    static auto vertBuffAltA = VertexBuffer<Vert>::Create(trig, 3, BufferUsage::Static);
                    static auto indexBuffAltA = IndexBuffer::Create(indecies, std::size(indecies), BufferUsage::Static);
                    
                    static auto vertBuffAltB = VertexBuffer<Vert>::Create(trig, 3, BufferUsage::Static);
                    static auto indexBuffAltB = IndexBuffer::Create(indecies, std::size(indecies), BufferUsage::Static);

                    static auto testImage = Graphics::GetMainDevice()->CreateImage(
                        Image::Usage::TransferSrc | Image::Usage::TransferDst | Image::Usage::Sampled | Image::Usage::ColorAttachment,
                        Image::Format::R8G8B8A8_UNORM,
                        150,
                        150
                    );

                    {
                        // Intentionally trying to cause a crash (out of order)
                        
                        auto& allocator = Graphics::GetMainDevice()->GetAllocatorForThread();

                        auto cmdA = allocator.CreateCommandList();
                        auto cmdB = allocator.CreateCommandList();
                        auto cmdC = allocator.CreateCommandList();

                        // Attempt to cause out of order sync
                        // in first cmd read from 2 resources, prob render 2 triangles then on second one only read from 1 resource
                        // then finally read from both resources - this is light version rn

                        RenderPassBeginInfo passInfo;
                        passInfo.attachmentSpecifications.push_back(RenderPassBeginInfo::AttachmentInfo { .buffer = testImage, .clearColor = Color::Blue });
                        cmdA->BeginRenderPass(passInfo);
                        cmdB->BeginRenderPass(passInfo);
                        cmdC->BeginRenderPass(passInfo);

                        cmdA->BindPipeline({
                            .vertexShaderModules = { mainmd },
                            .pixelShaderModules = { mainmd, matmd }
                        });

                        cmdB->BindPipeline({
                            .vertexShaderModules = { mainmd },
                            .pixelShaderModules = { mainmd, matmd }
                        });

                        cmdC->BindPipeline({
                            .vertexShaderModules = { mainmd },
                            .pixelShaderModules = { mainmd, matmd }
                        });

                        cmdA->BindVertexBuffers({ *vertBuff });
                        cmdB->BindVertexBuffers({ *vertBuffAltA });
                        cmdC->BindVertexBuffers({ *vertBuffAltB });

                        cmdA->BindIndexBuffer(indexBuff, 0);
                        cmdB->BindIndexBuffer(indexBuffAltA, 0);
                        cmdC->BindIndexBuffer(indexBuffAltB, 0);

                        // cmdC->BindUniformBuffer(matmd, 0, *uniBuff);
                        cmdC->BindUniformBuffer("test", *uniBuff);

                        cmdA->Draw(3, 0);
                        cmdB->Draw(3, 0);
                        cmdC->Draw(3, 0);

                        // cmdA->BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_OBJECT);
                        // cmdA->BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_FONT);
                        
                        // cmdB->BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_FONT);
                        
                        // cmdC->BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_OBJECT);
                        // cmdC->BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_FONT);
                        
                        cmdA->EndRenderPass();
                        cmdB->EndRenderPass();
                        cmdC->EndRenderPass();

                        cmdA->Execute();
                        cmdB->Execute();
                        cmdC->Execute();
                    }


                    // Render the test image in a small rectangle
                    
                    Vert rect[4] = {
                        {{ 0.5f, -0.5f, 0.f },   {1.0f, 0.0f}},  // right-bottom -> UV (1,0)
                        {{ -0.5f, 0.5f, 0.f },   {0.0f, 1.0f}},  // left-top    -> UV (0,1)
                        {{ -0.5f, -0.5f, 0.f },  {0.0f, 0.0f}},  // left-bottom -> UV (0,0)
                        {{ 0.5f, 0.5f, 0.f },   {1.0f, 1.0f}},  // right-top    -> UV (1,1)
                    };

                    std::uint32_t indeciesRect[6] = {
                        1, 2, 3,
                        3, 2, 0,
                    };

                    static auto vertBuffRect = VertexBuffer<Vert>::Create(rect, 4, BufferUsage::Static);
                    static auto indexBuffRect = IndexBuffer::Create(indeciesRect, std::size(indeciesRect), BufferUsage::Static);

                    cmdList->BindPipeline({
                        .vertexShaderModules = { mainmd },
                        .pixelShaderModules = { mainmd, matmd },
                        // .cullMode = CullMode::Both,
                        // .fillMode = FillMode::Wireframe,
                    });

                    cmdList->BindVertexBuffers({ *vertBuffRect });
                    cmdList->BindIndexBuffer(indexBuffRect);

                    cmdList->BindImage(matmd, 1, testImage);
                    SamplerConfiguration config;
                    config.minFilter = TextureFilter::Nearest;
                    config.magFilter = TextureFilter::Nearest;
                    cmdList->BindSampler(matmd, 1, Sampler::GetOrCreate(config));
                    // cmdList.BindTexture2D(matmd, 1, HeliosEditor::ICON_FILE_OBJECT);

                    {
                        auto test = uniBuff->LockGuard();
                        test->color = Color::BlanchedAlmond;
                    }
                    cmdList->BindUniformBuffer(matmd, 0, *uniBuff);

                    // cmdList.Draw(4, 0);
                    cmdList->DrawIndexed(6, 1);

                    cmdList->EndRenderPass();
                    
                    cmdList->Execute();

                    context.AddToList<Ref<Image>>("TestImage", testImage);
                }
            }
        };
    }
}