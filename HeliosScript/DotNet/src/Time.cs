using System.Runtime.InteropServices;

namespace Helios;

public class Time
{

    [DllImport("HeliosEngine.dll", EntryPoint = "helios_time_delta_time")]
    private static extern float __helios_internal_time_delta_time();

    [DllImport("HeliosEngine.dll", EntryPoint = "helios_time_frames_per_second")]
    private static extern float __helios_internal_time_frames_per_second();

    [DllImport("HeliosEngine.dll", EntryPoint = "helios_time_passed_time")]
    private static extern double __helios_internal_time_passed_time();

    [DllImport("HeliosEngine.dll", EntryPoint = "helios_time_current_time_microseconds")]
    private static extern ulong __helios_internal_time_current_time_microseconds();


    public static float framesPerSecond => __helios_internal_time_frames_per_second();
    public static double passedTime => __helios_internal_time_passed_time();
    public static ulong currentTimeMicroseconds => __helios_internal_time_current_time_microseconds();

    public static float deltaTime => __helios_internal_time_delta_time();
}