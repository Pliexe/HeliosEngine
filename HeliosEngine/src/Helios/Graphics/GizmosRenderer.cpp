#include "GizmosRenderer.h"
#include "Helios/Resources/Shader.h"

namespace Helios
{
	struct GizmosData
	{
		struct GizmosVertex
		{
			Vector3 position;
			Vector4 color;
			float id;
		};

		struct TransformData
		{
			Matrix4x4 viewProjection;
		};

		Ref<Shader> gizmosShader;
	};

	GizmosData gizmosData;

	void GizmosRenderer::Begin(Matrix4x4 projection)
	{
		
	}
}