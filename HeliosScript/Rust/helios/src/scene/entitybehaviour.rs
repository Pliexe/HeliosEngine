pub trait EntityBehaviour {
    fn on_update(&mut self) { }
    fn on_start(&mut self) { }
    fn on_awake(&mut self) { }

    fn get_entity(&self) 
    {
        // base.a;
    }
}

