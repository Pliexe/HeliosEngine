using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Helios;

public class Entity : IDisposable
{
    static internal Dictionary<IntPtr, Entity> entities = [];
    
    private IntPtr EntityPtr;
    private bool _disposed = false;

    public Entity(IntPtr entity_ptr)
    {
        EntityPtr = entity_ptr;
        entities.Add(EntityPtr, this);
    }

    ~Entity() => Dispose(false);

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (!_disposed)
        {
            if (EntityPtr != IntPtr.Zero)
            {
                InternalDestroyHandlePtr(EntityPtr);
                EntityPtr = IntPtr.Zero;
            }
            _disposed = true;
        }
    }

    public static void OnDestroy(IntPtr entity_ptr) {
        var entity = entities[entity_ptr];
        if (entity != null)
        {
            entities.Remove(entity_ptr);
            entity.Dispose();
        }
    }

    public static bool operator ==(Entity a, Entity b) => IsEqual(a, b);
    public static bool operator !=(Entity a, Entity b) => !IsEqual(a, b);

    public static implicit operator IntPtr(Entity entity) => entity.EntityPtr;

    public static bool IsEqual(Entity a, Entity b) => InternalIsEqual(a.EntityPtr, b.EntityPtr);


    public string name {
        get
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            
            uint length = InternalGetEntityName(EntityPtr, null, 0u);
            if (length == 0) throw new InvalidOperationException("Failed to get the required size of the entity name");
            byte[] bytes = new byte[length];
            uint result = InternalGetEntityName(EntityPtr, bytes, (uint)bytes.Length);
            if (result == 0) throw new InvalidOperationException("Failed to get entity name");
            int nullIndex = Array.IndexOf(bytes, (byte)0);
            nullIndex = nullIndex == -1 ? bytes.Length : nullIndex;
            return System.Text.Encoding.UTF8.GetString(bytes, 0, nullIndex);
        }

        set {
            ObjectDisposedException.ThrowIf(_disposed, this);

            bool result = InternalSetEntityName(EntityPtr, value);
            if (result) throw new InvalidOperationException();
        }
    }

    public string GetName() => this.name;


    [DllImport("HeliosEngine.dll", EntryPoint = "EntityIsEqual")]
    private static extern bool InternalIsEqual(IntPtr a, IntPtr b);
    [DllImport("HeliosEngine.dll", EntryPoint = "EntityIsValid")]
    private static extern bool InternalIsValid(IntPtr a);

    [DllImport("HeliosEngine.dll", EntryPoint = "EntityHandleClone")]
    private static extern IntPtr InternalCloneHandlePtr(IntPtr a);
    [DllImport("HeliosEngine.dll", EntryPoint = "EntityHandleDestroy")]
    private static extern bool InternalDestroyHandlePtr(IntPtr a);

    [DllImport("HeliosEngine.dll", EntryPoint = "EntityGetName")]
    private static extern uint InternalGetEntityName(IntPtr entity_ptr, byte[]? bytes, uint count);

    [DllImport("HeliosEngine.dll", EntryPoint = "SetEntityName")]
    [return: MarshalAs(UnmanagedType.I1)]
    private static extern bool InternalSetEntityName(IntPtr entity_ptr, string name);

    public override bool Equals(object obj)
    {
        if (ReferenceEquals(this, obj))
        {
            return true;
        }

        if (ReferenceEquals(obj, null))
        {
            return false;
        }

        return obj is Entity entity && InternalIsEqual(entity.EntityPtr, EntityPtr);
    }

    public override int GetHashCode()
    {
        throw new NotImplementedException();
    }
}