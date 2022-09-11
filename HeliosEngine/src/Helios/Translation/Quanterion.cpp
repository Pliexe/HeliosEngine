#include "Quanterion.h"
#include "Matrix.h"

namespace Helios
{
	Vector3 Quanterion::euler()
	{
        Matrix4x4 rotation = Matrix4x4::Rotation(*this);
        
        // Convert rotation Matrix to XYZ rotation
        
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        
        if (rotation.m[2][0] != 1 && rotation.m[2][0] != -1)
        {
            y = -asin(rotation.m[2][0]);
            x = atan2(rotation.m[2][1] / cos(y), rotation.m[2][2] / cos(y));
            z = atan2(rotation.m[1][0] / cos(y), rotation.m[0][0] / cos(y));
        }
        else
        {
            z = 0;
            if (rotation.m[2][0] == -1)
            {
                y = (float)M_PI / 2;
                x = z + atan2(rotation.m[0][1], rotation.m[0][2]);
            }
            else
            {
                y = -(float)M_PI / 2;
                x = -z + atan2(-rotation.m[0][1], -rotation.m[0][2]);
            }
        }
        
        return { -x, -y, -z };
        
        
        // float x = atan2f(rotation.matrix[1][2], rotation.matrix[2][2]);
        // float y = atan2f(-rotation.matrix[0][2], sqrtf(rotation.matrix[1][2] * rotation.matrix[1][2] + rotation.matrix[2][2] * rotation.matrix[2][2]));
        // float z = atan2f(rotation.matrix[0][1], rotation.matrix[0][0]);
        
        // return { x, y, z };
	}
}