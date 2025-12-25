using System;

namespace Helios;

public class Scene : IDisposable
{
    private IntPtr ScenePtr = IntPtr.Zero;
    private bool _disposed = false;

    public Scene(IntPtr scene_ptr) {
        ScenePtr = scene_ptr;
    }

    public void Dispose() {
        if (!_disposed)
        {
            // InternalDestroyScenePtr(ScenePtr);
            ScenePtr = IntPtr.Zero;
            _disposed = true;
        }
    }
}