using System;

namespace Helios;

[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]    
public struct Vector2
{
    public float x;
    public float y;
}

[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]    
public struct Vector3(float x, float y, float z)
{
    public float x = x;
    public float y = y;
    public float z = z;


    #region Constants
    public static Vector3 Right => new(1f, 0f, 0f);
        public static Vector3 Left => new(-1f, 0f, 0f);
        public static Vector3 Up => new(0f, 1f, 0f);
        public static Vector3 Down => new(0f, -1f, 0f);

        public static Vector3 Forward => new(0f, 0f, 1f);
        public static Vector3 Backward => new(0f, 0f, -1f);

        public static Vector3 Zero => new(0f, 0f, 0f);
        public static Vector3 One => new(1f, 1f, 1f);
#endregion

#region Operators
        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z - b.z);
        }

        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }

        public static Vector3 operator /(Vector3 a, float b)
        {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }

        public static Vector3 operator *(float a, Vector3 b)
        {
            return new Vector3(a * b.x, a * b.y, a * b.z);
        }

        public static Vector3 operator -(Vector3 a)
        {
            return new Vector3(-a.x, -a.y, -a.z);
        }
#endregion

#region Math Functions
        public static float Dot(Vector3 a, Vector3 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        public static float SqrLength(Vector3 a)
        {
            return a.x * a.x + a.y * a.y + a.z * a.z;
        }

        public static float Length(Vector3 a)
        {
            return Magnitude(a);
        }

        public static Vector3 Cross(Vector3 a, Vector3 b)
        {
            return new Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
        }

        public static float Magnitude(Vector3 a)
        {
            return MathF.Sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
        }

        public static float Distance(Vector3 a, Vector3 b)
        {
            return Magnitude(a - b);
        }
#endregion
}