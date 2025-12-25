namespace Helios
{
    public class EntityBehaviour
    {
        public EntityBehaviour() {
            
        }
        
        public Entity entity { get; init; }

        public virtual void Start() { }
        public virtual void Update() { }
        public virtual void LateUpdate() { }

        public virtual void OnEnable() { }
        public virtual void OnDisable() { }
    }
}