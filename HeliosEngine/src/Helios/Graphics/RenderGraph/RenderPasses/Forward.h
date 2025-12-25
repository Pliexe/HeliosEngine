#pragma once

#include "Helios/Core/Time.h"
#include "Helios/Graphics/RenderGraph/RenderPass.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Math/Matrix.h"
#include "pch.h"
#include <memory>

namespace Helios
{
    // RenderPassCallback HELIOS_API ForwardPass();

    
    inline RenderPassCallback ForwardPass()
    {
        return [] (RenderContext& context)
        {
            for (auto& view : context.views)
            {
                if (view.objects.empty()) continue;

                if (view.view.family == RenderView::Family::Scene)
                {
                    auto& allocator = Graphics::GetMainDevice()->GetAllocatorForThread();
                    auto cmdList = allocator.CreateCommandList();

                    auto colorImage = context.resources.RequestImage(view.view.size.width, view.view.size.height, 0, 0, Image::Format::R8G8B8A8_UNORM, Image::Usage::ColorAttachment | Image::Usage::Sampled);

                    RenderPassBeginInfo passInfo;
                    passInfo.attachmentSpecifications.push_back(RenderPassBeginInfo::AttachmentInfo { .buffer = colorImage, .clearColor = Color::PaleVioletRed });
                    cmdList->BeginRenderPass(passInfo);


                //     float3 position : Position;
                //     float2 texCoord : TexCoord;
                //     float3 normal : Normal;
                    
                // #ifdef INSTANCED

                //     matrix worldViewProj : WorldViewProj;
                //     matrix worldMat : WorldProj;
                //     float4 color : Color;
                //     int entityId : EntityId;
                //     int sceneIndex : SceneIndex;

                    static auto inputLayout = InputLayouts { 
                        InputLayout {
                            { "Position", ShaderDataType::Float32_3 },
                            { "TexCoord", ShaderDataType::Float32_2 },
                            { "Normal", ShaderDataType::Float32_3 },
                        },
                        InputLayout({
                            { "ViewProjection", ShaderDataType::MatrixFloat4x4,  },
                            { "ObjectTransform", ShaderDataType::MatrixFloat4x4 },
                            { "EntityId", ShaderDataType::Int32 },
                            { "SceneIndex", ShaderDataType::Int32 },
                        }, InputClassification::PerInstanceData)
                    };
                    
                    static auto mainModule = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
                        .family = ShaderModule::Family::Main,
                        .stage = ShaderModule::Stage::Pixel | ShaderModule::Stage::Vertex,
                        .storageType = ShaderModule::StorageType::Code,
                        .path = "RenderPass/Forward.hsl",
                        .definitions = { "INSTANCED" },
                        .inputLayout = inputLayout,
                    });

                    static auto materialModule = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
                        .family = ShaderModule::Family::Surface,
                        .stage = ShaderModule::Stage::Vertex | ShaderModule::Stage::Pixel,
                        .storageType = ShaderModule::StorageType::Code,
                        .tag = "MaterialPixelShader",
                        .path = "Material/StandardSurface.hsl",
                    });

                    struct InstanceData
                    {
                        Matrix4x4 viewProj;
                        Matrix4x4 objectTransform;
                        Color color;
                        uint32 entityId;
                        uint32 sceneIndex;
                    };

                    std::cout << std::format("HELLO TEST: {} RES: {}", view.objects.size(), sizeof(InstanceData) * view.objects.size()) << std::endl;

                    auto instanceBuffer = context.resources.RequestUnsafeVertexBuffer(sizeof(InstanceData) * view.objects.size());
                    std::cout << std::format("HELLO TEST PASS: {}", instanceBuffer->GetSize()) << std::endl;
                    auto instanceBuferData = instanceBuffer->LockSpan<InstanceData>(view.objects.size());

                    std::cout << std::format("HELLO TEST DATA: {}", instanceBuferData.size()) << std::endl;

                    Ref<MaterialInstance> previousMaterial;
                    Ref<Mesh> previousMesh;

                    uint32 instanceLimit = 1024;

                    uint32 currentInstance = 0;
                    uint32 totalInstances = 0;



                    auto startTime = Time::CurrentTimeMicroseconds();
                    
                    for (auto* pObject : view.objects)
                    {
                        auto& object = *pObject;
                        if (object.material == nullptr || object.mesh == nullptr) continue;
                        
                        if (object.material.get() != previousMaterial.get())
                        {
                            // std::cout << "Rebind Pipeline" << std::endl;

                            previousMaterial = object.material;
                            
                            cmdList->BindPipeline({
                                .vertexShaderModules = { mainModule },
                                .pixelShaderModules = { mainModule, materialModule },
                            });

                            if (object.material->texture)
                            {
                                auto tex2d = std::dynamic_pointer_cast<Texture2D>(object.material->texture);
                                if (tex2d)
                                {
                                    cmdList->BindImage("surfaceTexture", tex2d->GetImage());
                                    // cmdList->BindTexture2D("surfaceTexture", tex2d);
                                    cmdList->BindSampler("surfaceSampler", Sampler::GetOrCreate(object.material->sampler));
                                }
                            }
                        }

                        if (object.mesh != previousMesh)
                        {
                            // std::cout << "Rebind Mesh" << std::endl;

                            if (previousMesh != nullptr)
                                cmdList->DrawIndexed(previousMesh->GetIndexCount(), currentInstance);

                            previousMesh = object.mesh;
                         
                            currentInstance = 0;

                            cmdList->BindVertexBuffers({
                                VertexBufferBindInfo(object.mesh->GetVertexBuffer(), 0),
                                VertexBufferBindInfo(instanceBuffer, totalInstances * sizeof(InstanceData))
                            });

                            cmdList->BindIndexBuffer(object.mesh->GetIndexBuffer());
                        }
                        
                        instanceBuferData[totalInstances].viewProj = view.view.viewTransform;
                        instanceBuferData[totalInstances].objectTransform = object.transform;
                        instanceBuferData[totalInstances].color = Color::OrangeRed;
                        instanceBuferData[totalInstances].entityId = entt::null;
                        instanceBuferData[totalInstances].sceneIndex = 0;

                        currentInstance++;
                        totalInstances++;
                    }

                    auto endTime = Time::CurrentTimeMicroseconds();

                    auto duration = (endTime - startTime) / 1000.f;

                    std::cout << "RenderGraph - Forward Pass Draw: " << duration << " ms - " << std::to_string(totalInstances) << " objects" << std::endl;

                    if (currentInstance > 0 && previousMesh != nullptr) 
                    {
                        cmdList->DrawIndexed(previousMesh->GetIndexCount(), currentInstance);
                    }

                    instanceBuffer->Unlock();

                    cmdList->EndRenderPass();
                    
                    cmdList->Execute();

                    context.AddToList<Ref<Image>>("ForwardPassImage", colorImage);
                }
            }
        };
    }
}